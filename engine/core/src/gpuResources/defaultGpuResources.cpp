#include "defaultGpuResources.h"
#include "iComputeShader.h"
#include "iDefaultGpuResources.h"
#include <stdexcept>
#include <utility>



namespace emberCore
{
	// Static members:
	bool DefaultGpuResources::s_isInitialized = false;
	emberBackendInterface::IDefaultGpuResources* DefaultGpuResources::s_pIDefaultGpuResources = nullptr;



	// Private methods:
	// Initialization/Cleanup:
	void DefaultGpuResources::Init(emberBackendInterface::IDefaultGpuResources* pDefaultGpuResources)
	{
		if (s_isInitialized)
			return;
		if (pDefaultGpuResources == nullptr)
			throw std::runtime_error("DefaultGpuResources::Init(...) failed. pDefaultGpuResources is nullptr.");

		s_pIDefaultGpuResources = pDefaultGpuResources;
		s_isInitialized = true;
	}
	void DefaultGpuResources::Clear()
	{
		s_pIDefaultGpuResources = nullptr;
		s_isInitialized = false;
	}



	// Built-in compute shaders:
	void DefaultGpuResources::InitializeBuiltInComputeShaders(
		std::unique_ptr<emberBackendInterface::IComputeShader> pGammaCorrectionComputeShader,
		std::unique_ptr<emberBackendInterface::IComputeShader> pOutlineCompositeComputeShader,
		std::unique_ptr<emberBackendInterface::IComputeShader> pOutlineHorizontalMaskExpansionComputeShader,
		std::unique_ptr<emberBackendInterface::IComputeShader> pOutlineVerticalMaskExpansionComputeShader)
	{
		s_pIDefaultGpuResources->InitializeBuiltInComputeShaders(
			std::move(pGammaCorrectionComputeShader),
			std::move(pOutlineCompositeComputeShader),
			std::move(pOutlineHorizontalMaskExpansionComputeShader),
			std::move(pOutlineVerticalMaskExpansionComputeShader));
	}
}