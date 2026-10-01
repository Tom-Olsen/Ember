#include "gpuBackend.h"
#include "iGpuBackend.h"
#include <stdexcept>



namespace emberCore
{
	// Static members:
	std::unique_ptr<emberBackendInterface::IGpuBackend> GpuBackend::s_pIGpuBackend;



	// Private methods:
	// Initialization/Cleanup:
	void GpuBackend::Init(emberBackendInterface::IGpuBackend* pIGpuBackend)
	{
		if (s_pIGpuBackend != nullptr)
			return;
		if (pIGpuBackend == nullptr)
			throw std::runtime_error("GpuBackend::Init(...) failed. pIGpuBackend is nullptr.");

		s_pIGpuBackend = std::unique_ptr<emberBackendInterface::IGpuBackend>(pIGpuBackend);
	}
	void GpuBackend::Clear()
	{
		s_pIGpuBackend.reset();
	}



	// Getters:
	emberBackendInterface::ICompute* GpuBackend::GetComputeInterface()
	{
		if (s_pIGpuBackend == nullptr)
			throw std::runtime_error("GpuBackend::GetComputeInterface() failed. GpuBackend is not initialized.");

		emberBackendInterface::ICompute* pICompute = s_pIGpuBackend->GetCompute();
		if (pICompute == nullptr)
			throw std::runtime_error("GpuBackend::GetComputeInterface() failed. Compute interface is nullptr.");
		return pICompute;
	}
	emberBackendInterface::IDefaultGpuResources* GpuBackend::GetDefaultGpuResourcesInterface()
	{
		if (s_pIGpuBackend == nullptr)
			throw std::runtime_error("GpuBackend::GetDefaultGpuResourcesInterface() failed. GpuBackend is not initialized.");

		emberBackendInterface::IDefaultGpuResources* pIDefaultGpuResources = s_pIGpuBackend->GetDefaultGpuResources();
		if (pIDefaultGpuResources == nullptr)
			throw std::runtime_error("GpuBackend::GetDefaultGpuResourcesInterface() failed. Default gpu resources interface is nullptr.");
		return pIDefaultGpuResources;
	}
	emberBackendInterface::IGpuResourceFactory* GpuBackend::GetGpuResourceFactoryInterface()
	{
		if (s_pIGpuBackend == nullptr)
			throw std::runtime_error("GpuBackend::GetGpuResourceFactoryInterface() failed. GpuBackend is not initialized.");

		emberBackendInterface::IGpuResourceFactory* pIGpuResourceFactory = s_pIGpuBackend->GetGpuResourceFactory();
		if (pIGpuResourceFactory == nullptr)
			throw std::runtime_error("GpuBackend::GetGpuResourceFactoryInterface() failed. Gpu resource factory interface is nullptr.");
		return pIGpuResourceFactory;
	}
	emberBackendInterface::IRenderer* GpuBackend::GetRendererInterface()
	{
		if (s_pIGpuBackend == nullptr)
			throw std::runtime_error("GpuBackend::GetRendererInterface() failed. GpuBackend is not initialized.");

		emberBackendInterface::IRenderer* pIRenderer = s_pIGpuBackend->GetRenderer();
		if (pIRenderer == nullptr)
			throw std::runtime_error("GpuBackend::GetRendererInterface() failed. Renderer interface is nullptr.");
		return pIRenderer;
	}
}