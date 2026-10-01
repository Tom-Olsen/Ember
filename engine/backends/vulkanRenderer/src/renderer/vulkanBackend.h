#pragma once
#include "commonRendererCreateInfo.h"
#include "iGpuBackend.h"
#include "vulkanRendererExport.h"
#include <memory>



// Forward declarations:
namespace emberBackendInterface
{
	class IWindow;
}



namespace vulkanRendererBackend
{
	// Forward declarations:
	class Compute;
	class DefaultGpuResources;
	class GpuResourceFactory;
	class Renderer;



	class VULKAN_RENDERER_API VulkanBackend : public emberBackendInterface::IGpuBackend
	{
	private: // Members:
		std::unique_ptr<Renderer> m_pRenderer;
		std::unique_ptr<DefaultGpuResources> m_pDefaultGpuResources;
		std::unique_ptr<GpuResourceFactory> m_pGpuResourceFactory;
		std::unique_ptr<Compute> m_pCompute;

	public: // Methods:
		// Constructor/Destructor:
		VulkanBackend(const emberCommon::RendererCreateInfo& createInfo, emberBackendInterface::IWindow* pIWindow);
		~VulkanBackend() override;

		// Non-copyable:
		VulkanBackend(const VulkanBackend&) = delete;
		VulkanBackend& operator=(const VulkanBackend&) = delete;

		// Non-movable:
		VulkanBackend(VulkanBackend&&) = delete;
		VulkanBackend& operator=(VulkanBackend&&) = delete;

		// Getters:
		emberBackendInterface::IRenderer* GetRenderer() override;
		emberBackendInterface::ICompute* GetCompute() override;
		emberBackendInterface::IGpuResourceFactory* GetGpuResourceFactory() override;
		emberBackendInterface::IDefaultGpuResources* GetDefaultGpuResources() override;
	};
}