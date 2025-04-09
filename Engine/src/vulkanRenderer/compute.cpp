#include "compute.h"
#include "computeShader.h"
#include "logger.h"
#include "shaderProperties.h"
#include "vulkanUtility.h"
#include <vulkan/vulkan.h>



namespace emberEngine
{
	// Static members:
	bool Compute::s_isInitialized = false;
	uint32_t Compute::s_callIndex = 0;
	std::vector<ComputeCall> Compute::s_staticComputeCalls;
	std::vector<ComputeCall> Compute::s_dynamicComputeCalls;
	std::vector<ComputeCall*> Compute::s_computeCallPointers;
	std::unordered_map<ComputeShader*, ResourcePool<ShaderProperties, 10>> Compute::s_shaderPropertiesPoolMap;



	// Initialization/Cleanup:
	void Compute::Init()
	{
		if (s_isInitialized)
			return;
		s_isInitialized = true;
	}
	void Compute::Clear()
	{
		ResetComputeCalls();
		s_shaderPropertiesPoolMap.clear();
	}



	// Public methods:
	// Dispatch calls:
	ShaderProperties* Compute::Dispatch(ComputeShader* pComputeShader, uint32_t threadCountX, uint32_t threadCountY, uint32_t threadCountZ)
	{
		return Dispatch(pComputeShader, Uint3(threadCountX, threadCountY, threadCountZ));
	}
	ShaderProperties* Compute::Dispatch(ComputeShader* pComputeShader, Uint3 threadCount)
	{
		if (!pComputeShader)
		{
			LOG_ERROR("Compute::Dispatch(...) failed. pComputeShader is nullptr.");
			return nullptr;
		}

		// Setup compute call:
		threadCount = Uint3::Max(Uint3::one, threadCount);
		ShaderProperties* pShaderProperties = s_shaderPropertiesPoolMap[pComputeShader].Acquire((Shader*)pComputeShader);
		ComputeCall computeCall = { s_callIndex, threadCount, pComputeShader, pShaderProperties, VK_ACCESS_2_NONE, VK_ACCESS_2_NONE };
		s_dynamicComputeCalls.push_back(computeCall);
		s_callIndex++;

		// By returning pShaderProperties, we allow user to change the shader properties of the compute call:
		return pShaderProperties;
	}
	void Compute::Dispatch(ComputeShader* pComputeShader, ShaderProperties* pShaderProperties, uint32_t threadCountX, uint32_t threadCountY, uint32_t threadCountZ)
	{
		Dispatch(pComputeShader, pShaderProperties, Uint3(threadCountX, threadCountY, threadCountZ));
	}
	void Compute::Dispatch(ComputeShader* pComputeShader, ShaderProperties* pShaderProperties, Uint3 threadCount)
	{
		if (!pComputeShader)
			LOG_ERROR("Compute::Dispatch(...) failed. pComputeShader is nullptr.");

		// Setup compute call:
		threadCount = Uint3::Max(Uint3::one, threadCount);
		ComputeCall computeCall = { s_callIndex, threadCount, pComputeShader, pShaderProperties, VK_ACCESS_2_NONE, VK_ACCESS_2_NONE };
		s_staticComputeCalls.push_back(computeCall);
		s_callIndex++;
	}

	// Barriers:
	void Compute::Barrier(VkAccessFlags2 srcAccessMask, VkAccessFlags2 dstAccessMask)
	{
		ComputeCall computeCall = { s_callIndex, Uint3::zero, nullptr, nullptr, srcAccessMask, dstAccessMask };
		s_staticComputeCalls.push_back(computeCall);
		s_callIndex++;
	}



	// Management:
	std::vector<ComputeCall*>& Compute::GetComputeCallPointers()
	{
		// Populate draw call pointers vector according to callIndex:
		s_computeCallPointers.clear();
		s_computeCallPointers.resize(s_staticComputeCalls.size() + s_dynamicComputeCalls.size());
		for (auto& computeCall : s_staticComputeCalls)
			s_computeCallPointers[computeCall.callIndex] = &computeCall;
		for (auto& computeCall : s_dynamicComputeCalls)
			s_computeCallPointers[computeCall.callIndex] = &computeCall;

		return s_computeCallPointers;
	}
	void Compute::ResetComputeCalls()
	{
		// Return all pShaderProperties of compute calls back to the corresponding pool:
		for (ComputeCall& computeCall : s_dynamicComputeCalls)
			s_shaderPropertiesPoolMap[computeCall.pComputeShader].Release(computeCall.pShaderProperties);

		// Shrink all pools back to max number of computeCalls of last frame:
		for (auto& [_, pool] : s_shaderPropertiesPoolMap)
			pool.ShrinkToFit();

		// Remove all computeCalls so next frame can start fresh:
		s_staticComputeCalls.clear();
		s_dynamicComputeCalls.clear();
		s_callIndex = 0;
	}
	void Compute::PrintComputeCalls()
	{
		for (int i = 0; i < s_computeCallPointers.size(); i++)
		{
			if (s_computeCallPointers[i] == nullptr)
				break;

			LOG_TRACE(s_computeCallPointers[i]->ToString());
		}
	}
}