#include "vulkanContext.h"
#include "dearImGui.h"
#include "logger.h"
#include "vulkanMacros.h"



namespace emberEngine
{
	// Static members:
	bool VulkanContext::s_isInitialized = false;
	std::unique_ptr<SdlWindow> VulkanContext::pWindow = nullptr;
	std::unique_ptr<VulkanInstance> VulkanContext::pInstance = nullptr;
	std::unique_ptr<VulkanPhysicalDevice> VulkanContext::pPhysicalDevice = nullptr;
	std::unique_ptr<VulkanSurface> VulkanContext::pSurface = nullptr;
	std::unique_ptr<VulkanLogicalDevice> VulkanContext::pLogicalDevice = nullptr;
	std::unique_ptr<VulkanMemoryAllocator> VulkanContext::pAllocator = nullptr;
	std::unique_ptr< VulkanAllocationTracker> VulkanContext::pAllocationTracker = nullptr;
	std::unique_ptr<VulkanDescriptorPool> VulkanContext::pDescriptorPool = nullptr;
	std::unique_ptr<VulkanSwapchain> VulkanContext::pSwapchain = nullptr;
	uint32_t VulkanContext::framesInFlight;
	uint32_t VulkanContext::frameIndex;
	uint64_t VulkanContext::absoluteFrameIndex;
	VkSampleCountFlagBits VulkanContext::msaaSamples;



	// Initialization and cleanup:
	void VulkanContext::Init(uint32_t framesInFlightValue, VkSampleCountFlagBits msaaSamplesValue, uint32_t windowWidth, uint32_t windowHeight, bool vSyncEnabled)
	{
		if (s_isInitialized)
			return;
		s_isInitialized = true;

		framesInFlight = framesInFlightValue;
		frameIndex = 0;
		absoluteFrameIndex = 0;

		// Window:
		pWindow = std::make_unique<SdlWindow>(windowWidth, windowHeight);

		// Get instance extensions:
		std::vector<const char*> instanceExtensions;
		#if defined(VALIDATION_LAYERS_ACTIVE)
		instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		instanceExtensions.push_back(VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME);
		#endif
		pWindow->AddSdlInstanceExtensions(instanceExtensions);		// sdl instance extensions
		DearImGui::AddImGuiInstanceExtensions(instanceExtensions);	// add instance extensions for docking feature

		// and more ...

		// Get device extensions:
		std::vector<const char*> deviceExtensions;
		deviceExtensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
		// These extensions are integrated in Vulkan 1.2 as features, but ImGui relies on them as an extensions => Validation Layer warnings.
		deviceExtensions.emplace_back(VK_EXT_SHADER_VIEWPORT_INDEX_LAYER_EXTENSION_NAME);	
		deviceExtensions.emplace_back(VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME);	
		// and more ...

		// Create vulkan context:
		pInstance = std::make_unique<VulkanInstance>(instanceExtensions);
		pPhysicalDevice = std::make_unique<VulkanPhysicalDevice>(pInstance.get());
		pSurface = std::make_unique<VulkanSurface>(pInstance.get(), pPhysicalDevice.get(), pWindow.get(), vSyncEnabled);
		pLogicalDevice = std::make_unique<VulkanLogicalDevice>(pPhysicalDevice.get(), pSurface.get(), deviceExtensions);
		pAllocator = std::make_unique<VulkanMemoryAllocator>(pInstance.get(), pLogicalDevice.get(), pPhysicalDevice.get());
		pAllocationTracker = std::make_unique< VulkanAllocationTracker>();
		pDescriptorPool = std::make_unique<VulkanDescriptorPool>(pLogicalDevice.get());
		pSwapchain = std::make_unique<VulkanSwapchain>(pLogicalDevice.get(), pSurface.get(), VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);

		msaaSamples = std::min(msaaSamplesValue, pPhysicalDevice->GetMaxMsaaSamples());
	}
	void VulkanContext::Clear()
	{
		WaitDeviceIdle();
	}



	// Getters:
	SDL_Window* const VulkanContext::GetSDL_Window()
	{
		return pWindow->GetSDL_Window();
	}
	const VkInstance& VulkanContext::GetVkInstance()
	{
		return pInstance->GetVkInstance();
	}
	const VkPhysicalDevice& VulkanContext::GetVkPhysicalDevice()
	{
		return pPhysicalDevice->GetVkPhysicalDevice();
	}
	const VkSurfaceKHR& VulkanContext::GetVkSurfaceKHR()
	{
		return pSurface->GetVkSurfaceKHR();
	}
	const VkDevice& VulkanContext::GetVkDevice()
	{
		return pLogicalDevice->GetVkDevice();
	}
	const VmaAllocator& VulkanContext::GetVmaAllocator()
	{
		return pAllocator->GetVmaAllocator();
	}
	const VkDescriptorPool& VulkanContext::GetVkDescriptorPool()
	{
		return pDescriptorPool->GetVkDescriptorPool();
	}
	const VkSwapchainKHR& VulkanContext::GetVkSwapchainKHR()
	{
		return pSwapchain->GetVkSwapchainKHR();
	}
	bool VulkanContext::DepthClampEnabled()
	{
		return pPhysicalDevice->SupportsDepthClamp();
	}
	bool VulkanContext::DepthBiasClampEnabled()
	{
		return pPhysicalDevice->SupportsDepthBiasClamp();
	}



	// Public frame logic:
	void VulkanContext::UpdateFrameIndex()
	{
		frameIndex = (frameIndex + 1) % framesInFlight;
		absoluteFrameIndex++;
	}
	void VulkanContext::ResetFrameIndex()
	{
		frameIndex = 0;
	}
	void VulkanContext::WaitDeviceIdle()
	{
		VKA(vkDeviceWaitIdle(pLogicalDevice->GetVkDevice()));
	}
}