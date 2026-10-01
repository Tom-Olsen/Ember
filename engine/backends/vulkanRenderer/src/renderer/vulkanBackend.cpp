#include "vulkanBackend.h"
#include "vulkanCompute.h"
#include "vulkanDefaultGpuResources.h"
#include "vulkanGpuResourceFactory.h"
#include "vulkanRenderer.h"



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	VulkanBackend::VulkanBackend(const emberCommon::RendererCreateInfo& createInfo, emberBackendInterface::IWindow* pIWindow)
	{
		bool isInfrastructureInitialized = false;
		bool isRenderingInitialized = false;
		try
		{
			// Ember::ToDo:
			// Renderer initialization is split (InitializeInfrastructure + InitializeRendering) because DefaultGpuResources must be created
			// after the low-level GPU runtime is initialized, but before descriptor layouts and other rendering resources that consume the defaults.
			// A future GpuRuntime class could own the low-level backend state separately, allowing DefaultGpuResources and Renderer to use
			// normal constructor/destructor initialization without requiring these two initialization phases.

			m_pRenderer = std::make_unique<Renderer>(createInfo, pIWindow);
			m_pRenderer->InitializeInfrastructure();
			isInfrastructureInitialized = true;

			m_pDefaultGpuResources = std::make_unique<DefaultGpuResources>();
			m_pRenderer->InitializeRendering();
			isRenderingInitialized = true;

			m_pGpuResourceFactory = std::make_unique<GpuResourceFactory>(createInfo.shadowMapResolution);
			m_pCompute = std::make_unique<Compute>();
		}
		catch (...)
		{
			m_pCompute.reset();
			m_pGpuResourceFactory.reset();
			if (isRenderingInitialized)
				m_pRenderer->ClearRendering();
			m_pDefaultGpuResources.reset();
			if (isInfrastructureInitialized)
				m_pRenderer->ClearInfrastructure();
			m_pRenderer.reset();
			throw;
		}
	}
	VulkanBackend::~VulkanBackend()
	{
		m_pCompute.reset();
		m_pGpuResourceFactory.reset();
		m_pRenderer->ClearRendering();
		m_pDefaultGpuResources.reset();
		m_pRenderer->ClearInfrastructure();
		m_pRenderer.reset();
	}



	// Getters:
	emberBackendInterface::IRenderer* VulkanBackend::GetRenderer()
	{
		return m_pRenderer.get();
	}
	emberBackendInterface::ICompute* VulkanBackend::GetCompute()
	{
		return m_pCompute.get();
	}
	emberBackendInterface::IGpuResourceFactory* VulkanBackend::GetGpuResourceFactory()
	{
		return m_pGpuResourceFactory.get();
	}
	emberBackendInterface::IDefaultGpuResources* VulkanBackend::GetDefaultGpuResources()
	{
		return m_pDefaultGpuResources.get();
	}
}