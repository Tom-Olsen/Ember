#include "vulkanAsyncCompute.h"
#include "emberMath.h"
#include "logger.h"
#include "vulkanAccessMask.h"
#include "vulkanCommandPool.h"
#include "vulkanComputeCall.h"
#include "vulkanComputeSession.h"
#include "vulkanComputeShader.h"
#include "vulkanContext.h"
#include "vulkanConvertComputeAccessMask.h"
#include "vulkanDescriptorSetBinding.h"
#include "vulkanDeviceQueue.h"
#include "vulkanLogicalDevice.h"
#include "vulkanMacros.h"
#include "vulkanPoolManager.h"
#include <stdexcept>
#include <utility>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	Async::Async(uint32_t maxSessionCount)
	{
		// Set session count:
		m_maxSessionCount = math::Max((uint32_t)1, maxSessionCount);

		// Resize/Reserve vectors:
		m_pCommandPools.reserve(m_maxSessionCount);
		m_computeSessions.resize(m_maxSessionCount);
		const auto& computeQueues = Context::GetLogicalDevice()->GetComputeQueues();
		m_pendingSessionCounts.resize(computeQueues.size());

		for (uint32_t i = 0; i < m_maxSessionCount; i++)
		{
			// Command pools can be used by every compute queue in the same family, so simply use first compute queue:
			m_pCommandPools.emplace_back(0, *computeQueues.front());

			// Populate index queue:
			m_freeIndices.push(i);
		}
	}
	Async::~Async()
	{
		Cleanup();
	}



	// Movable:
	Async::Async(Async&& other) noexcept
	{
		MoveFrom(other);
	}
	Async& Async::operator=(Async&& other) noexcept
	{
		if (this != &other)
		{
			Cleanup();
			MoveFrom(other);
		}
		return *this;
	}



	// Dispatch logic:
	uint32_t Async::CreateComputeSession()
	{
		if (m_freeIndices.empty())
			throw std::runtime_error("compute::Async::CreateComputeSession(...) has run out of free sessionIDs (maxSessionCount=" + std::to_string(m_maxSessionCount) + "). You can increase maxSessionCount in compute::Async::Init(...).");

		uint32_t sessionID = m_freeIndices.front();
		m_freeIndices.pop();
		m_computeSessions[sessionID].state = ComputeSession::State::recording;

		return sessionID;
	}
	void Async::DispatchComputeSession(uint32_t sessionID, float time, float deltaTime)
	{
		if (sessionID >= m_maxSessionCount)
			throw std::out_of_range("compute::Async::DispatchComputeSession(...) sessionID '" + std::to_string(sessionID) + "' out of range.");
		if (m_computeSessions[sessionID].state == ComputeSession::State::recording)
		{
			CollectFinishedSessions();
			uint32_t queueIndex = SelectQueue();
			ComputeSession& session = m_computeSessions[sessionID];
			const DeviceQueue& queue = *Context::GetLogicalDevice()->GetComputeQueues()[queueIndex];
			uint64_t signalValue = session.Dispatch(m_pCommandPools[sessionID].GetPrimaryVkCommandBuffer(), queue, time, deltaTime);
			m_pendingSessionCounts[queueIndex]++;
			session.queueIndex = queueIndex;
			session.completionValue = signalValue;
			session.state = ComputeSession::State::running;
			m_nextQueueIndex = (queueIndex + 1) % static_cast<uint32_t>(m_pendingSessionCounts.size());
		}
		else if (m_computeSessions[sessionID].state == ComputeSession::State::idle)
			LOG_WARN("Trying to dispatch compute session which is still in idle state (forgot to call CreateComputeSession() first).");
		else // (m_computeSessions[sessionID].state == ComputeSession::State::running)
			LOG_WARN("Trying to dispatch compute session which is still in running state (already dispatched).");
	}
	bool Async::IsFinished(uint32_t sessionID)
	{
		if (sessionID >= m_maxSessionCount)
			throw std::out_of_range("compute::Async::IsFinished(...) sessionID '" + std::to_string(sessionID) + "' out of range.");

		if (m_computeSessions[sessionID].state != ComputeSession::State::running)
			return true;	// not running => finished=true, and no reset required.

		const ComputeSession& session = m_computeSessions[sessionID];
		const DeviceQueue& queue = *Context::GetLogicalDevice()->GetComputeQueues()[session.queueIndex];
		bool isFinished = queue.GetCompletedValue() >= session.completionValue;

		if (isFinished)
			ResetComputeSession(sessionID);

		return isFinished;
	}
	void Async::WaitForFinish(uint32_t sessionID)
	{
		if (sessionID >= m_maxSessionCount)
			throw std::out_of_range("compute::Async::WaitForFinish(...) sessionID '" + std::to_string(sessionID) + "' out of range.");

		if (m_computeSessions[sessionID].state != ComputeSession::State::running)
			return;	// not running => finished, no reset required.

		const ComputeSession& session = m_computeSessions[sessionID];
		const DeviceQueue& queue = *Context::GetLogicalDevice()->GetComputeQueues()[session.queueIndex];
		queue.WaitForValue(session.completionValue);
		ResetComputeSession(sessionID);
	}
	void Async::CollectFinishedSessions()
	{
		// Query each active queue once, rather than querying once per session.
		const auto& computeQueues = Context::GetLogicalDevice()->GetComputeQueues();
		std::vector<uint64_t> completedValues(computeQueues.size());
		for (uint32_t queueIndex = 0; queueIndex < computeQueues.size(); queueIndex++)
		{
			if (m_pendingSessionCounts[queueIndex] > 0)
				completedValues[queueIndex] = computeQueues[queueIndex]->GetCompletedValue();
		}
		for (uint32_t sessionID = 0; sessionID < m_maxSessionCount; sessionID++)
		{
			if (m_computeSessions[sessionID].state != ComputeSession::State::running)
				continue;

			const ComputeSession& session = m_computeSessions[sessionID];
			if (completedValues[session.queueIndex] >= session.completionValue)
				ResetComputeSession(sessionID);
		}
	}



	// Workload recording:
	emberBackendInterface::IDescriptorSetBinding* Async::RecordComputeShader(uint32_t sessionID, emberBackendInterface::IComputeShader* pIComputeShader, Uint3 threadCount)
	{
		// Record dynamic compute call.
		if (sessionID >= m_maxSessionCount)
		{
			LOG_ERROR("compute::Async::RecordComputeShader(...) sessionID '{}' out of range.", sessionID);
			return nullptr;
		}
		if (!pIComputeShader)
		{
			LOG_ERROR("compute::Async::RecordComputeShader(...) failed. pIComputeShader is nullptr.");
			return nullptr;
		}
		ComputeShader* pComputeShader = static_cast<ComputeShader*>(pIComputeShader);
		if (pComputeShader->GetFeatures().ReadsSceneColor() || pComputeShader->GetFeatures().WritesSceneColor())
		{
			LOG_ERROR("compute::Async::RecordComputeShader(...) '{}' failed. Async compute shaders cannot access scene color textures.", pComputeShader->GetDebugName());
			return nullptr;
		}
		if (threadCount[0] == 0 || threadCount[1] == 0 || threadCount[2] == 0)
		{
			LOG_ERROR("compute::Async::RecordComputeShader(...) '{}' failed. threadCount has 0 entry.", pComputeShader->GetDebugName());
			return nullptr;
		}
		if (m_computeSessions[sessionID].state != ComputeSession::State::recording)
		{
			LOG_ERROR("compute::Async::RecordComputeShader(...) failed. sessionID: {} is not in recording state.", sessionID);
			return nullptr;
		}

		// Setup compute call:
		DescriptorSetBindingHandle descriptorSetBindingHandle = PoolManager::CheckOutCallDescriptorSetBindingHandle(static_cast<Shader*>(pComputeShader));
		ComputeCall computeCall = { threadCount, ShaderHandle(*pComputeShader), descriptorSetBindingHandle, AccessMasks::None::none, AccessMasks::None::none };
		m_computeSessions[sessionID].RecordComputeCall(computeCall);
		pComputeShader->AddPendingUse();
		return static_cast<emberBackendInterface::IDescriptorSetBinding*>(descriptorSetBindingHandle.Get());
	}
	void Async::RecordBarrier(uint32_t sessionID, emberBackendInterface::ComputeBarrierFlag srcBarrierFlags, emberBackendInterface::ComputeBarrierFlag dstBarrierFlags)
	{
		if (sessionID >= m_maxSessionCount)
		{
			LOG_ERROR("compute::Async::RecordBarrier(...) sessionID '{}' out of range.", sessionID);
			return;
		}
		if (m_computeSessions[sessionID].state != ComputeSession::State::recording)
		{
			LOG_ERROR("compute::Async::RecordBarrier(...) failed. sessionID: {} is not in recording state.", sessionID);
			return;
		}
		
		ComputeCall computeCall = { Uint3::zero, ShaderHandle(), DescriptorSetBindingHandle(), ComputeBarrierFlagsToVulkanAccessMask(srcBarrierFlags), ComputeBarrierFlagsToVulkanAccessMask(dstBarrierFlags) };
		m_computeSessions[sessionID].RecordComputeCall(computeCall);
	}



	// Debugging:
	void Async::PrintSessionsState()
	{
		std::queue<uint32_t> copy = m_freeIndices;
		std::string freeSessions;
		while (!copy.empty())
		{
			freeSessions += std::to_string(copy.front()) + " ";
			copy.pop();
		}
		LOG_INFO("Free sessions: {}", freeSessions);
	}



	// Private methods:
	// Cleanup:
	void Async::Cleanup()
	{
		// Complete running sessions before releasing bindings or command pools.
		for (uint32_t sessionID = 0; sessionID < m_maxSessionCount; sessionID++)
		{
			if (m_computeSessions[sessionID].state == ComputeSession::State::running)
				WaitForFinish(sessionID);
			else if (m_computeSessions[sessionID].state == ComputeSession::State::recording)
				ResetComputeSession(sessionID);
		}
		m_pendingSessionCounts.clear();
		m_computeSessions.clear();
		m_pCommandPools.clear();
		m_freeIndices = {};
		m_maxSessionCount = 0;
		m_nextQueueIndex = 0;
	}
	void Async::MoveFrom(Async& other) noexcept
	{
		m_maxSessionCount = std::exchange(other.m_maxSessionCount, 0);
		m_nextQueueIndex = std::exchange(other.m_nextQueueIndex, 0);
		m_pCommandPools = std::move(other.m_pCommandPools);
		m_computeSessions = std::move(other.m_computeSessions);
		m_pendingSessionCounts = std::move(other.m_pendingSessionCounts);
		m_freeIndices = std::move(other.m_freeIndices);
		other.m_pCommandPools.clear();
		other.m_computeSessions.clear();
		other.m_pendingSessionCounts.clear();
		other.m_freeIndices = {};
	}



	// Helpers:
	uint32_t Async::SelectQueue()
	{
		uint32_t queueCount = static_cast<uint32_t>(m_pendingSessionCounts.size());
		if (queueCount == 0)
			throw std::runtime_error("Async compute has no queues.");

		// Use the next queue if it is idle:
		if (m_pendingSessionCounts[m_nextQueueIndex] == 0)
			return m_nextQueueIndex;

		// Pick the least busy queue, retaining round-robin order for ties:
		uint32_t selectedQueueIndex = m_nextQueueIndex;
		for (uint32_t offset = 1; offset < queueCount; offset++)
		{
			uint32_t queueIndex = (m_nextQueueIndex + offset) % queueCount;
			if (m_pendingSessionCounts[queueIndex] < m_pendingSessionCounts[selectedQueueIndex])
				selectedQueueIndex = queueIndex;
		}
		return selectedQueueIndex;
	}
	void Async::ResetComputeSession(uint32_t sessionID)
	{
		// Return all bindings back to the corresponding pool:
		for (ComputeCall& computeCall : m_computeSessions[sessionID].GetComputeCalls())
		{
			PoolManager::ReturnCallDescriptorSetBinding(computeCall.callDescriptorSetBindingHandle);
			if (!computeCall.IsBarrier())
				computeCall.GetComputeShader()->RemovePendingUse();
		}

		m_pCommandPools[sessionID].ResetPools();
		ComputeSession& session = m_computeSessions[sessionID];
		if (session.state == ComputeSession::State::running)
			m_pendingSessionCounts[session.queueIndex]--;
		session.state = ComputeSession::State::idle;
		session.queueIndex = UINT32_MAX;
		session.completionValue = 0;
		session.ResetComputeCalls();
		m_freeIndices.push(sessionID);
	}
}