#ifndef __INCLUDE_GUARD_vulkanContext_h__
#define __INCLUDE_GUARD_vulkanContext_h__
// Direct include instead of forward declaration,
// as this is the main header file that includes all other Vulkan-related headers.
#include "sdlWindow.h"
#include "vulkanDescriptorPool.h"
#include "vulkanInstance.h"
#include "vulkanLogicalDevice.h"
#include "vulkanMemoryAllocator.h"
#include "vulkanPhysicalDevice.h"
#include "vulkanSurface.h"
#include "vulkanSwapchain.h"
#include <vulkan/vulkan.h>
#include <memory>



namespace emberEngine
{

	/// <summary>
	/// VulkanContext is a utility structure designed to encapsulate and manage
	/// all components required for a Vulkan-based rendering application.
	/// It integrates multiple Vulkan resources and functionalities into a cohesive framework: <para/>
	/// - SdlWindow:				SDL_Window management and event handling. <para/>
	/// - VulkanInstance:			VkInstance and validation layers. <para/>
	/// - VulkanPhysicalDevice:		physical device selection and feature support. <para/>
	/// - VulkanSurface:			VkSurface and present mode (mailbox, FIFO, etc.). <para/>
	/// - VulkanLogicalDevice:		VkDevice and queues (graphics, present, compute, transfer). <para/>
	/// - VulkanMemoryAllocator:	VmaAllocator for flexible memory allocation pools. <para/>
	/// - VulkanDescriptorPool:		VkDescriptorPool settings. <para/>
	/// - VulkanSwapchain:			VkSwapchainKHR, spwapchain images, image views, and recreation. <para/>
	/// - framesInFlight:			Number of frames in flight for synchronization. <para/>
	/// - frameIndex:				Current frame index for synchronization. <para/>
	/// - msaaSamples:				Msaa level, clamped to the maximum supported by the physical device. <para/>
	/// </summary>
	struct VulkanContext
	{
	public: // Members:
		std::unique_ptr<SdlWindow> pWindow;
		std::unique_ptr<VulkanInstance> pInstance;
		std::unique_ptr<VulkanPhysicalDevice> pPhysicalDevice;
		std::unique_ptr<VulkanSurface> pSurface;
		std::unique_ptr<VulkanLogicalDevice> pLogicalDevice;
		std::unique_ptr<VulkanMemoryAllocator> pAllocator;
		std::unique_ptr<VulkanDescriptorPool> pDescriptorPool;
		std::unique_ptr<VulkanSwapchain> pSwapchain;
		uint32_t framesInFlight;
		uint32_t frameIndex;
		VkSampleCountFlagBits msaaSamples;

	public: // Methods:
		VulkanContext(uint32_t framesInFlight, VkSampleCountFlagBits msaaSamples);
		~VulkanContext();

		// Getters:
		SDL_Window* const GetSDL_Window() const;
		const VkInstance& GetVkInstance() const;
		const VkPhysicalDevice& GetVkPhysicalDevice() const;
		const VkSurfaceKHR& GetVkSurfaceKHR() const;
		const VkDevice& GetVkDevice() const;
		const VmaAllocator& GetVmaAllocator() const;
		const VkDescriptorPool& GetVkDescriptorPool() const;
		const VkSwapchainKHR& GetVkSwapchainKHR() const;
		bool DepthClampEnabled() const;
		bool DepthBiasEnabled() const;

		// Frame logic:
		void UpdateFrameIndex();
		void ResetFrameIndex();
		void WaitDeviceIdle();
	};
}



#endif // __INCLUDE_GUARD_vulkanContext_h__