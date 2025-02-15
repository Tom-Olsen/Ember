#include "compute.h"
#include "computeShader.h"
#include "computeUnit.h"
#include "logger.h"
#include "shaderProperties.h"
#include "vulkanContext.h"



namespace emberEngine
{
	// Static members:
	uint32_t Compute::s_dispatchIndex = 0;
	bool Compute::s_isInitialized = false;
	std::vector<ComputeUnit*> Compute::s_computeUnit;



	// Initialization/Cleanup:
	void Compute::Init(VulkanContext* pContext)
	{
		if (s_isInitialized)
			return;
		s_isInitialized = true;

		s_computeUnit.resize(10);
		for (uint32_t i = 0; i < 10; i++)
			s_computeUnit[i] = new ComputeUnit();
	}
	void Compute::Clear()
	{
		for (uint32_t i = 0; i < s_computeUnit.size(); i++)
			delete s_computeUnit[i];
		s_computeUnit.clear();
	}



	// Public methods:
	// Dispatch calls:
	ShaderProperties* Compute::Dispatch(ComputeShader* pComputeShader, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
	{
		if (!pComputeShader)
		{
			LOG_ERROR("Compute::Dispatch(...) failed. pComputeShader is nullptr.");
			return nullptr;
		}

		DoubleCapacityIfNeeded();

		// Setup current dispatch call:
		ComputeUnit* pComputeUnit = s_computeUnit[s_dispatchIndex];
		pComputeUnit->SetComputeShader(pComputeShader);

		// By returning the computeShaders shaderProperties, we allow user to change the shader properties of this dispatch call:
		ShaderProperties* pShaderProperties = s_computeUnit[s_dispatchIndex]->GetShaderProperties();
		s_dispatchIndex++;
		return pShaderProperties;
	}



	// Private methods:
	void Compute::DoubleCapacityIfNeeded()
	{
		uint32_t oldSize = static_cast<uint32_t>(s_computeUnit.size());
		if (s_dispatchIndex >= oldSize)
		{
			uint32_t newSize = 2 * oldSize;
			s_computeUnit.resize(newSize);
			for (uint32_t i = oldSize; i < newSize; i++)
				s_computeUnit[i] = new ComputeUnit();
		}
	}
	void Compute::ReduceCapacity()
	{
		uint32_t oldSize = static_cast<uint32_t>(s_computeUnit.size());
		uint32_t newSize = s_dispatchIndex; // dispatchIndex is one past the last active element, so no +1 needed.
		if (oldSize > newSize)
		{
			for (uint32_t i = newSize; i < oldSize; i++)
				delete s_computeUnit[i];
			s_computeUnit.resize(newSize);
		}
	}
}