#include "preRenderCompute.h"
#include "computeCall.h"
#include "computeShader.h"
#include "logger.h"
#include "shaderProperties.h"
#include "poolManager.h"
#include "vulkanAccessMasks.h"



namespace vulkanRendererBackend
{
	// Static members:
	bool PreRender::s_isInitialized = false;
	uint32_t PreRender::s_callIndex = 0;
	std::vector<ComputeCall> PreRender::s_staticComputeCalls;
	std::vector<ComputeCall> PreRender::s_dynamicComputeCalls;
	std::vector<ComputeCall*> PreRender::s_computeCallPointers;



	// Initialization/Cleanup:
	void PreRender::Init()
	{
		if (s_isInitialized)
			return;
		s_isInitialized = true;
	}
	void PreRender::Clear()
	{
		ResetComputeCalls();
	}



	// Workload recording:
	ShaderProperties* PreRender::RecordComputeShader(ComputeShader* pComputeShader, Uint3 threadCount)
	{
		if (!pComputeShader)
		{
			LOG_ERROR("compute::PreRender::RecordComputeShader(...) failed. pComputeShader is nullptr.");
			return nullptr;
		}
		if (threadCount[0] == 0 || threadCount[1] == 0 || threadCount[2] == 0)
		{
			LOG_ERROR("compute::PreRender::RecordComputeShader(...) failed. threadCount has 0 entry.");
			return nullptr;
		}

		// Setup compute call:
		ShaderProperties* pShaderProperties = PoolManager::CheckOutShaderProperties(pComputeShader);
		ComputeCall computeCall = { s_callIndex, threadCount, pComputeShader, pShaderProperties, accessMask::none::none, accessMask::none::none };
		s_dynamicComputeCalls.push_back(computeCall);
		s_callIndex++;

		// By returning pShaderProperties, we allow user to change the shader properties of the compute call:
		return pShaderProperties;
	}
	void PreRender::RecordComputeShader(ComputeShader* pComputeShader, ShaderProperties* pShaderProperties, Uint3 threadCount)
	{
		if (!pComputeShader)
		{
			LOG_ERROR("compute::PreRender::RecordComputeShader(...) failed. pComputeShader is nullptr.");
			return;
		}
		if (!pShaderProperties)
		{
			LOG_ERROR("compute::PreRender::RecordComputeShader(...) failed. pShaderProperties is nullptr.");
			return;
		}
		if (threadCount[0] == 0 || threadCount[1] == 0 || threadCount[2] == 0)
		{
			LOG_ERROR("compute::PreRender::RecordComputeShader(...) failed. threadCount has 0 entry.");
			return;
		}

		// Setup compute call:
		ComputeCall computeCall = { s_callIndex, threadCount, pComputeShader, pShaderProperties, accessMask::none::none, accessMask::none::none };
		s_staticComputeCalls.push_back(computeCall);
		s_callIndex++;
	}
	void PreRender::RecordBarrier(VkAccessFlags2 srcAccessMask, VkAccessFlags2 dstAccessMask)
	{
		ComputeCall computeCall = { s_callIndex, Uint3::zero, nullptr, nullptr, srcAccessMask, dstAccessMask };
		s_staticComputeCalls.push_back(computeCall);
		s_callIndex++;
	}



	// Management:
	std::vector<ComputeCall*>& PreRender::GetComputeCallPointers()
	{
		// Populate draw call pointers vector according to callIndex:
		s_computeCallPointers.resize(s_staticComputeCalls.size() + s_dynamicComputeCalls.size());
		for (auto& computeCall : s_staticComputeCalls)
			s_computeCallPointers[computeCall.callIndex] = &computeCall;
		for (auto& computeCall : s_dynamicComputeCalls)
			s_computeCallPointers[computeCall.callIndex] = &computeCall;

		return s_computeCallPointers;
	}
	void PreRender::ResetComputeCalls()
	{
		// Return all pShaderProperties of compute calls back to the corresponding pool:
		for (ComputeCall& computeCall : s_dynamicComputeCalls)
			PoolManager::ReturnShaderProperties(computeCall.pComputeShader, computeCall.pShaderProperties);

		// Remove all computeCalls so next frame can start fresh:
		s_staticComputeCalls.clear();
		s_dynamicComputeCalls.clear();
		s_computeCallPointers.clear();
		s_callIndex = 0;
	}
}