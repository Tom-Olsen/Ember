#include "compute.h"
#include "computeShader.h"
#include "logger.h"
#include "shaderProperties.h"



namespace emberEngine
{
	// Static members:
	bool Compute::s_isInitialized = false;
	std::vector<ComputeCall> Compute::s_computeCalls;
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
		ComputeCall computeCall = { threadCount, pComputeShader, pShaderProperties };
		s_computeCalls.push_back(computeCall);

		// By returning pShaderProperties, we allow user to change the shader properties of the compute call:
		return pShaderProperties;
	}
	void Compute::ResetComputeCalls()
	{
		// Return all pShaderProperties of compute calls back to the corresponding pool:
		for (ComputeCall& computeCall : s_computeCalls)
			s_shaderPropertiesPoolMap[computeCall.pComputeShader].Release(computeCall.pShaderProperties);

		// Shrink all pools back to max number of computeCalls of last frame:
		for (auto& [_, pool] : s_shaderPropertiesPoolMap)
			pool.ShrinkToFit();

		// Remove all computeCalls so next frame can start fresh:
		s_computeCalls.clear();
	}


	// Getters:
	std::vector<ComputeCall*>* Compute::GetComputeCallPointers()
	{
		// By returning a vector of pointers this allows us to do sorting first if needed.

		// Populate sorted draw call pointers vector:
		s_computeCallPointers.clear();
		s_computeCallPointers.reserve(s_computeCalls.size());
		for (auto& computeCall : s_computeCalls)
			s_computeCallPointers.push_back(&computeCall);

		// Soirt compute call pointers here if needed.

		return &s_computeCallPointers;
	}
}