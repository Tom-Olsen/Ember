#include "vulkanAsyncCompute.h"
#include "logger.h"
#include "vulkanCommandPool.h"
#include "vulkanComputeStage.h"
#include "vulkanContext.h"
#include "vulkanDeviceQueue.h"
#include "vulkanLogicalDevice.h"
#include "vulkanRenderer.h"
#include <span>
#include <stdexcept>
#include <utility>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	Async::Async()
	{
		constexpr uint32_t initialSessionCount = 8;
		m_pCommandPools.reserve(initialSessionCount);
		m_computeSessions.reserve(initialSessionCount);
		m_pendingSessionCounts.resize(Context::GetLogicalDevice()->GetComputeQueues().size());

		for (uint32_t sessionIndex = 0; sessionIndex < initialSessionCount; sessionIndex++)
			AddSession();
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
		if (m_pendingSessionCounts.empty())
			throw std::runtime_error("Async compute has no queues.");

		if (m_freeIndices.empty())
			CollectFinishedSessions();
		if (m_freeIndices.empty())
			AddSession();

		uint32_t sessionID = m_freeIndices.front();
		m_freeIndices.pop();
		m_computeSessions[sessionID]->computeQueue.Reset();
		m_computeSessions[sessionID]->state = Session::State::recording;

		return sessionID;
	}
	void Async::DispatchComputeSession(uint32_t sessionID, float time, float deltaTime)
	{
		if (sessionID >= m_computeSessions.size())
			throw std::out_of_range("compute::Async::DispatchComputeSession(...) sessionID '" + std::to_string(sessionID) + "' out of range.");
		if (m_computeSessions[sessionID]->state == Session::State::recording)
		{
			CollectFinishedSessions();

			// Select least busy compute queue:
			uint32_t queueIndex = SelectQueue();
			Session& session = *m_computeSessions[sessionID];
			const DeviceQueue& queue = *Context::GetLogicalDevice()->GetComputeQueues()[queueIndex];

			// Update descriptor set bindings:
			session.computeQueue.UpdateShaderData(0);

			// Record commands:
			VkCommandBuffer commandBuffer = m_pCommandPools[sessionID].GetPrimaryVkCommandBuffer();
			const auto& staticDescriptorSets = Context::GetRenderer()->GetStaticDescriptorSets(0);
			ComputeStage::Record(commandBuffer, session.computeQueue.GetComputeCalls(), 0, time, deltaTime, std::span<const VkDescriptorSet>(staticDescriptorSets.data(), 1));
			session.computeQueue.Close();

			// Submit commands:
			VkCommandBufferSubmitInfo commandBufferInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO };
			commandBufferInfo.commandBuffer = commandBuffer;
			VkSubmitInfo2 submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO_2 };
			submitInfo.commandBufferInfoCount = 1;
			submitInfo.pCommandBufferInfos = &commandBufferInfo;
			uint64_t signalValue = queue.Submit(submitInfo);

			// Session and queue management:
			Context::MarkDeviceBusy();
			m_pendingSessionCounts[queueIndex]++;
			session.queueIndex = queueIndex;
			session.completionValue = signalValue;
			session.state = Session::State::running;
			m_nextQueueIndex = (queueIndex + 1) % static_cast<uint32_t>(m_pendingSessionCounts.size());
		}
		else if (m_computeSessions[sessionID]->state == Session::State::idle)
			LOG_WARN("Trying to dispatch compute session which is still in idle state (forgot to call CreateComputeSession() first).");
		else // (m_computeSessions[sessionID]->state == Session::State::running)
			LOG_WARN("Trying to dispatch compute session which is still in running state (already dispatched).");
	}
	bool Async::IsFinished(uint32_t sessionID)
	{
		if (sessionID >= m_computeSessions.size())
			throw std::out_of_range("compute::Async::IsFinished(...) sessionID '" + std::to_string(sessionID) + "' out of range.");

		if (m_computeSessions[sessionID]->state != Session::State::running)
			return true;	// not running => finished=true, and no reset required.

		const Session& session = *m_computeSessions[sessionID];
		const DeviceQueue& queue = *Context::GetLogicalDevice()->GetComputeQueues()[session.queueIndex];
		bool isFinished = queue.GetCompletedValue() >= session.completionValue;

		if (isFinished)
			ResetComputeSession(sessionID);

		return isFinished;
	}
	void Async::WaitForFinish(uint32_t sessionID)
	{
		if (sessionID >= m_computeSessions.size())
			throw std::out_of_range("compute::Async::WaitForFinish(...) sessionID '" + std::to_string(sessionID) + "' out of range.");

		if (m_computeSessions[sessionID]->state != Session::State::running)
			return;	// not running => finished, no reset required.

		const Session& session = *m_computeSessions[sessionID];
		const DeviceQueue& queue = *Context::GetLogicalDevice()->GetComputeQueues()[session.queueIndex];
		queue.WaitForValue(session.completionValue);
		ResetComputeSession(sessionID);
	}
	void Async::CollectFinishedSessions()
	{
		// Query each active queue once, rather than querying once per session.
		const auto& computeQueues = Context::GetLogicalDevice()->GetComputeQueues();
		std::vector<uint64_t> completedValues(computeQueues.size());
		for (uint32_t queueIndex = 0; queueIndex < m_pendingSessionCounts.size(); queueIndex++)
		{
			if (m_pendingSessionCounts[queueIndex] > 0)
				completedValues[queueIndex] = computeQueues[queueIndex]->GetCompletedValue();
		}
		for (uint32_t sessionID = 0; sessionID < m_computeSessions.size(); sessionID++)
		{
			if (m_computeSessions[sessionID]->state != Session::State::running)
				continue;

			const Session& session = *m_computeSessions[sessionID];
			if (completedValues[session.queueIndex] >= session.completionValue)
				ResetComputeSession(sessionID);
		}
	}



	// Workload recording:
	emberBackendInterface::ICompute::IQueue& Async::GetRecordingQueue(uint32_t sessionID)
	{
		if (sessionID >= m_computeSessions.size())
			throw std::out_of_range("Async::GetRecordingQueue(...) sessionID is out of range.");
		Session& session = *m_computeSessions[sessionID];
		if (session.state != Session::State::recording)
			throw std::logic_error("Async::GetRecordingQueue(...) session is not recording.");
		return session.computeQueue;
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
		for (uint32_t sessionID = 0; sessionID < m_computeSessions.size(); sessionID++)
		{
			if (m_computeSessions[sessionID]->state == Session::State::running)
				WaitForFinish(sessionID);
			else if (m_computeSessions[sessionID]->state == Session::State::recording)
				ResetComputeSession(sessionID);
		}
		m_pendingSessionCounts.clear();
		m_computeSessions.clear();
		m_pCommandPools.clear();
		m_freeIndices = {};
		m_nextQueueIndex = 0;
	}
	void Async::MoveFrom(Async& other) noexcept
	{
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
	void Async::AddSession()
	{
		if (m_computeSessions.size() >= UINT32_MAX)
			throw std::overflow_error("Async compute session IDs exhausted.");

		uint32_t sessionID = static_cast<uint32_t>(m_computeSessions.size());
		m_computeSessions.push_back(std::make_unique<Session>());
		try
		{
			// All compute queues share a family, so every session can use the first queue's family:
			m_pCommandPools.emplace_back(0, *Context::GetLogicalDevice()->GetComputeQueues().front());
			m_freeIndices.push(sessionID);
		}
		catch (...)
		{
			// Keep the session, command-pool, and free-ID storage consistent if allocation fails.
			if (m_pCommandPools.size() > sessionID)
				m_pCommandPools.pop_back();
			m_computeSessions.pop_back();
			throw;
		}
	}
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
		Session& session = *m_computeSessions[sessionID];
		session.computeQueue.Reset();
		session.computeQueue.Close();
		m_pCommandPools[sessionID].ResetPools();
		if (session.state == Session::State::running)
			m_pendingSessionCounts[session.queueIndex]--;
		session.state = Session::State::idle;
		session.queueIndex = UINT32_MAX;
		session.completionValue = 0;
		m_freeIndices.push(sessionID);
	}
}