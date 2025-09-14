#include "vulkanAsyncCompute.h"
#include "logger.h"
#include "vulkanAccessMasks.h"
#include "vulkanCommandPool.h"
#include "vulkanComputeCall.h"
#include "vulkanComputeSession.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"
#include "vulkanPoolManager.h"
#include <stdexcept>



namespace vulkanRendererBackend
{
	// Static members:
	bool Async::s_isInitialized = false;
	int Async::s_sessionCount;
	std::vector<CommandPool> Async::s_pCommandPools;
	std::vector<ComputeSession> Async::s_computeSessions;
	std::vector<VkFence> Async::s_fences;
	std::queue<uint32_t> Async::s_freeIndices;



	// Initialization/Cleanup:
	void Async::Init(int sessionCount)
	{
		if (s_isInitialized)
			return;
		s_isInitialized = true;

		// Set session count:
		s_sessionCount = std::max(1, sessionCount);

		// Resize/Reserve vectors:
		s_pCommandPools.reserve(s_sessionCount);
		s_computeSessions.resize(s_sessionCount);
		s_fences.resize(s_sessionCount);

		for (int i = 0; i < s_sessionCount; i++)
		{
			// Create command pools:
			s_pCommandPools.emplace_back(0, Context::logicalDevice.GetComputeQueue());

			// Create fences:
			VkFenceCreateInfo fenceInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
			VKA(vkCreateFence(Context::GetVkDevice(), &fenceInfo, nullptr, &s_fences[i]));

			// Populate index queue:
			s_freeIndices.push(i);
		}
	}
	void Async::Clear()
	{
		// Empty queue:
		std::queue<uint32_t> empty;
		std::swap(s_freeIndices, empty);

		// Delete fences:
		for (int i = 0; i < s_sessionCount; i++)
			vkDestroyFence(Context::GetVkDevice(), s_fences[i], nullptr);

		// Delete compute session:
		s_computeSessions.clear();

		// Delete command pool:
		s_pCommandPools.clear();
	}



	// Dispatch logic:
	uint32_t Async::CreateComputeSession()
	{
		if (s_freeIndices.empty())
			throw std::runtime_error("compute::Async::CreateComputeSession(...) has run out of free sessionIDs (sessionCount=" + std::to_string(s_sessionCount) + ").You can increase sessionCount in compute::Async::Init(...).");

		uint32_t sessionID = s_freeIndices.front();
		s_freeIndices.pop();
		s_computeSessions[sessionID].state = ComputeSession::State::recording;

		return sessionID;
	}
	void Async::DispatchComputeSession(uint32_t sessionID, float time, float deltaTime)
	{
		if (sessionID < 0 || sessionID > s_sessionCount - 1)
			throw std::out_of_range("compute::Async::DispatchComputeSession(...) sessionID out of range.");
		if (s_computeSessions[sessionID].state == ComputeSession::State::recording)
		{
			s_computeSessions[sessionID].Dispatch(s_pCommandPools[sessionID].GetPrimaryVkCommandBuffer(), s_fences[sessionID], time, deltaTime);
			s_computeSessions[sessionID].state = ComputeSession::State::running;
		}
		else if (s_computeSessions[sessionID].state == ComputeSession::State::idle)
			LOG_WARN("Trying to dispatch compute session which is still in idle state (forgot to call CreateComputeSession() first).");
		else // (s_computeSessions[sessionID].state == ComputeSession::State::running)
			LOG_WARN("Trying to dispatch compute session which is still in running state (already dispatched).");
	}
	bool Async::IsFinished(uint32_t sessionID)
	{
		if (sessionID < 0 || sessionID > s_sessionCount - 1)
			throw std::out_of_range("compute::Async::IsFinished(...) sessionID out of range.");

		if (s_computeSessions[sessionID].state != ComputeSession::State::running)
			return true;	// not running => finished=true, and no reset required.

		VkResult result = vkGetFenceStatus(Context::GetVkDevice(), s_fences[sessionID]);
		bool isFinished = (result == VK_SUCCESS) ? true : false;

		if (isFinished)
			ResetComputeSession(sessionID);

		return isFinished;
	}
	void Async::WaitForFinish(uint32_t sessionID)
	{
		if (sessionID < 0 || sessionID > s_sessionCount - 1)
			throw std::out_of_range("compute::Async::WaitForFinish(...) sessionID out of range.");

		if (s_computeSessions[sessionID].state != ComputeSession::State::running)
			return;	// not running => finished, no reset required.

		VKA(vkWaitForFences(Context::GetVkDevice(), 1, &s_fences[sessionID], VK_TRUE, UINT64_MAX));
		ResetComputeSession(sessionID);
	}



	// Workload recording:
	ShaderProperties* Async::RecordComputeShader(uint32_t sessionID, ComputeShader* pComputeShader, Uint3 threadCount)
	{
		if (sessionID < 0 || sessionID > s_sessionCount - 1)
			throw std::out_of_range("compute::Async::RecordComputeShader(...) sessionID out of range.");

		if (!pComputeShader)
		{
			LOG_ERROR("compute::Async::RecordComputeShader(...) failed. pComputeShader is nullptr.");
			return nullptr;
		}
		if (threadCount[0] == 0 || threadCount[1] == 0 || threadCount[2] == 0)
		{
			LOG_ERROR("compute::Async::RecordComputeShader(...) failed. threadCount has 0 entry.");
			return nullptr;
		}
		if (s_computeSessions[sessionID].state != ComputeSession::State::recording)
		{
			LOG_ERROR("compute::Async::RecordComputeShader(...) failed. sessionID: {} is not in recording state.", sessionID);
			return nullptr;
		}

		// Setup compute call:
		ShaderProperties* pShaderProperties = PoolManager::CheckOutShaderProperties((Shader*)pComputeShader);
		ComputeCall computeCall = { 0, threadCount, pComputeShader, pShaderProperties, accessMask::none::none, accessMask::none::none };
		s_computeSessions[sessionID].RecordComputeCall(computeCall);

		// By returning pShaderProperties, we allow user to change the shader properties of the compute call:
		return pShaderProperties;
	}
	void Async::RecordComputeShader(uint32_t sessionID, ComputeShader* pComputeShader, ShaderProperties* pShaderProperties, Uint3 threadCount)
	{
		if (!pComputeShader)
		{
			LOG_ERROR("compute::Async::RecordComputeShader(...) failed. pComputeShader is nullptr.");
			return;
		}
		if (!pShaderProperties)
		{
			LOG_ERROR("compute::Async::RecordComputeShader(...) failed. pShaderProperties is nullptr.");
			return;
		}
		if (threadCount[0] == 0 || threadCount[1] == 0 || threadCount[2] == 0)
		{
			LOG_ERROR("compute::Async::RecordComputeShader(...) failed. threadCount has 0 entry.");
			return;
		}
		if (s_computeSessions[sessionID].state != ComputeSession::State::recording)
		{
			LOG_ERROR("compute::Async::RecordComputeShader(...) failed. sessionID: {} is not in recording state.", sessionID);
			return;
		}

		// Setup compute call:
		ComputeCall computeCall = { 0, threadCount, pComputeShader, pShaderProperties, accessMask::none::none, accessMask::none::none };
		s_computeSessions[sessionID].RecordComputeCall(computeCall);
	}
	void Async::RecordBarrier(uint32_t sessionID, VkAccessFlags2 srcAccessMask, VkAccessFlags2 dstAccessMask)
	{
		if (s_computeSessions[sessionID].state != ComputeSession::State::recording)
		{
			LOG_ERROR("compute::Async::RecordComputeShader(...) failed. sessionID: {} is not in recording state.", sessionID);
			return;
		}

		ComputeCall computeCall = { 0, Uint3::zero, nullptr, nullptr, srcAccessMask, dstAccessMask };
		s_computeSessions[sessionID].RecordComputeCall(computeCall);
	}



	// Private methods:
	void Async::ResetComputeSession(uint32_t sessionID)
	{
		// Return all pShaderProperties of compute calls back to the corresponding pool:
		for (ComputeCall& computeCall : s_computeSessions[sessionID].GetComputeCalls())
			PoolManager::ReturnShaderProperties((Shader*)computeCall.pComputeShader, computeCall.pShaderProperties);

		s_pCommandPools[sessionID].ResetPools();
		s_computeSessions[sessionID].state = ComputeSession::State::idle;
		s_computeSessions[sessionID].ResetComputeCalls();
		VKA(vkResetFences(Context::GetVkDevice(), 1, &s_fences[sessionID]));
		s_freeIndices.push(sessionID);
	}
}