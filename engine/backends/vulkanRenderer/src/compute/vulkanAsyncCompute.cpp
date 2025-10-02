#include "vulkanAsyncCompute.h"
#include "emberMath.h"
#include "logger.h"
#include "vulkanAccessMask.h"
#include "vulkanCommandPool.h"
#include "vulkanComputeCall.h"
#include "vulkanComputeSession.h"
#include "vulkanComputeShader.h"
#include "vulkanConvertAccessMask.h"
#include "vulkanContext.h"
#include "vulkanLogicalDevice.h"
#include "vulkanMacros.h"
#include "vulkanPoolManager.h"
#include "vulkanShaderProperties.h"
#include <stdexcept>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	Async::Async(uint16_t sessionCount)
	{
		// Set session count:
		m_sessionCount = math::Max((uint16_t)1, sessionCount);

		// Resize/Reserve vectors:
		m_pCommandPools.reserve(m_sessionCount);
		m_computeSessions.resize(m_sessionCount);
		m_fences.resize(m_sessionCount);

		for (uint16_t i = 0; i < m_sessionCount; i++)
		{
			// Create command pools:
			m_pCommandPools.emplace_back(0, Context::GetLogicalDevice()->GetComputeQueue());

			// Create fences:
			VkFenceCreateInfo fenceInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
			VKA(vkCreateFence(Context::GetVkDevice(), &fenceInfo, nullptr, &m_fences[i]));

			// Populate index queue:
			m_freeIndices.push(i);
		}
	}
	Async::~Async()
	{
		// Empty queue:
		std::queue<uint16_t> empty;
		std::swap(m_freeIndices, empty);

		// Delete fences:
		for (int i = 0; i < m_sessionCount; i++)
			vkDestroyFence(Context::GetVkDevice(), m_fences[i], nullptr);

		// Delete compute session:
		m_computeSessions.clear();

		// Delete command pool:
		m_pCommandPools.clear();
	}



	// Dispatch logic:
	uint16_t Async::CreateComputeSession()
	{
		if (m_freeIndices.empty())
			throw std::runtime_error("compute::Async::CreateComputeSession(...) has run out of free sessionIDs (sessionCount=" + std::to_string(m_sessionCount) + ").You can increase sessionCount in compute::Async::Init(...).");

		uint16_t sessionID = m_freeIndices.front();
		m_freeIndices.pop();
		m_computeSessions[sessionID].state = ComputeSession::State::recording;

		return sessionID;
	}
	void Async::DispatchComputeSession(uint16_t sessionID, float time, float deltaTime)
	{
		if (sessionID < 0 || sessionID > m_sessionCount - 1)
			throw std::out_of_range("compute::Async::DispatchComputeSession(...) sessionID out of range.");
		if (m_computeSessions[sessionID].state == ComputeSession::State::recording)
		{
			m_computeSessions[sessionID].Dispatch(m_pCommandPools[sessionID].GetPrimaryVkCommandBuffer(), m_fences[sessionID], time, deltaTime);
			m_computeSessions[sessionID].state = ComputeSession::State::running;
		}
		else if (m_computeSessions[sessionID].state == ComputeSession::State::idle)
			LOG_WARN("Trying to dispatch compute session which is still in idle state (forgot to call CreateComputeSession() first).");
		else // (m_computeSessions[sessionID].state == ComputeSession::State::running)
			LOG_WARN("Trying to dispatch compute session which is still in running state (already dispatched).");
	}
	bool Async::IsFinished(uint16_t sessionID)
	{
		if (sessionID < 0 || sessionID > m_sessionCount - 1)
			throw std::out_of_range("compute::Async::IsFinished(...) sessionID out of range.");

		if (m_computeSessions[sessionID].state != ComputeSession::State::running)
			return true;	// not running => finished=true, and no reset required.

		VkResult result = vkGetFenceStatus(Context::GetVkDevice(), m_fences[sessionID]);
		bool isFinished = (result == VK_SUCCESS) ? true : false;

		if (isFinished)
			ResetComputeSession(sessionID);

		return isFinished;
	}
	void Async::WaitForFinish(uint16_t sessionID)
	{
		if (sessionID < 0 || sessionID > m_sessionCount - 1)
			throw std::out_of_range("compute::Async::WaitForFinish(...) sessionID out of range.");

		if (m_computeSessions[sessionID].state != ComputeSession::State::running)
			return;	// not running => finished, no reset required.

		VKA(vkWaitForFences(Context::GetVkDevice(), 1, &m_fences[sessionID], VK_TRUE, UINT64_MAX));
		ResetComputeSession(sessionID);
	}



	// Workload recording:
	void Async::RecordComputeShader(uint16_t sessionID, emberBackendInterface::IComputeShader* pIComputeShader, emberBackendInterface::IShaderProperties* pIShaderProperties, Uint3 threadCount)
	{
		if (0 < sessionID || sessionID > m_sessionCount - 1)
		{
			LOG_ERROR("compute::Async::RecordComputeShader(...) sessionID out of range.");
			return;
		}
		if (!pIComputeShader)
		{
			LOG_ERROR("compute::Async::RecordComputeShader(...) failed. pIComputeShader is nullptr.");
			return;
		}
		if (!pIShaderProperties)
		{
			LOG_ERROR("compute::Async::RecordComputeShader(...) failed. pIShaderProperties is nullptr.");
			return;
		}
		if (threadCount[0] == 0 || threadCount[1] == 0 || threadCount[2] == 0)
		{
			LOG_ERROR("compute::Async::RecordComputeShader(...) failed. threadCount has 0 entry.");
			return;
		}
		if (m_computeSessions[sessionID].state != ComputeSession::State::recording)
		{
			LOG_ERROR("compute::Async::RecordComputeShader(...) failed. sessionID: {} is not in recording state.", sessionID);
			return;
		}

		// Setup compute call:
		ComputeCall computeCall = { 0, threadCount, static_cast<ComputeShader*>(pIComputeShader), static_cast<ShaderProperties*>(pIShaderProperties), AccessMasks::None::none, AccessMasks::None::none };
		m_computeSessions[sessionID].RecordComputeCall(computeCall);
	}
	emberBackendInterface::IShaderProperties* Async::RecordComputeShader(uint16_t sessionID, emberBackendInterface::IComputeShader* pIComputeShader, Uint3 threadCount)
	{
		ShaderProperties* pIShaderProperties = PoolManager::CheckOutShaderProperties(static_cast<Shader*>(static_cast<ComputeShader*>(pIComputeShader)));
		RecordComputeShader(sessionID, pIComputeShader, static_cast<emberBackendInterface::IShaderProperties*>(pIShaderProperties), threadCount);
		return static_cast<emberBackendInterface::IShaderProperties*>(pIShaderProperties);
	}
	void Async::RecordBarrier(uint16_t sessionID, emberCommon::ComputeShaderAccessMask srcAccessMask, emberCommon::ComputeShaderAccessMask dstAccessMask)
	{
		if (m_computeSessions[sessionID].state != ComputeSession::State::recording)
		{
			LOG_ERROR("compute::Async::RecordComputeShader(...) failed. sessionID: {} is not in recording state.", sessionID);
			return;
		}
		
		ComputeCall computeCall = { 0, Uint3::zero, nullptr, nullptr, CommonToVulkanAccessMask(srcAccessMask), CommonToVulkanAccessMask(dstAccessMask) };
		m_computeSessions[sessionID].RecordComputeCall(computeCall);
	}



	// Private methods:
	void Async::ResetComputeSession(uint16_t sessionID)
	{
		// Return all pShaderProperties of compute calls back to the corresponding pool:
		for (ComputeCall& computeCall : m_computeSessions[sessionID].GetComputeCalls())
			PoolManager::ReturnShaderProperties((Shader*)computeCall.pComputeShader, computeCall.pShaderProperties);

		m_pCommandPools[sessionID].ResetPools();
		m_computeSessions[sessionID].state = ComputeSession::State::idle;
		m_computeSessions[sessionID].ResetComputeCalls();
		VKA(vkResetFences(Context::GetVkDevice(), 1, &m_fences[sessionID]));
		m_freeIndices.push(sessionID);
	}
}