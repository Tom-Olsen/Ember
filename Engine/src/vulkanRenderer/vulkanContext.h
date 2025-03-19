#ifndef __INCLUDE_GUARD_vulkanContext_h__
#define __INCLUDE_GUARD_vulkanContext_h__
// Direct include instead of forward declaration,
// as this is the main header file that includes all other Vulkan-related headers.
#include "sdlWindow.h"
#include "vulkanAllocationTracker.h"
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
	private: // Members:
		static bool s_isInitialized;

	public: // Members:
		static std::unique_ptr<SdlWindow> pWindow;
		static std::unique_ptr<VulkanInstance> pInstance;
		static std::unique_ptr<VulkanPhysicalDevice> pPhysicalDevice;
		static std::unique_ptr<VulkanSurface> pSurface;
		static std::unique_ptr<VulkanLogicalDevice> pLogicalDevice;
		static std::unique_ptr<VulkanMemoryAllocator> pAllocator;
		static std::unique_ptr<VulkanAllocationTracker> pAllocationTracker;
		static std::unique_ptr<VulkanDescriptorPool> pDescriptorPool;
		static std::unique_ptr<VulkanSwapchain> pSwapchain;
		static uint32_t framesInFlight;
		static uint32_t frameIndex;
		static VkSampleCountFlagBits msaaSamples;

	public: // Methods:
		static void Init(uint32_t framesInFlightValue, VkSampleCountFlagBits msaaSamplesValue, uint32_t windowWidth, uint32_t windowHeight);
		static void Clear();

		// Getters:
		static SDL_Window* const GetSDL_Window();
		static const VkInstance& GetVkInstance();
		static const VkPhysicalDevice& GetVkPhysicalDevice();
		static const VkSurfaceKHR& GetVkSurfaceKHR();
		static const VkDevice& GetVkDevice();
		static const VmaAllocator& GetVmaAllocator();
		static const VkDescriptorPool& GetVkDescriptorPool();
		static const VkSwapchainKHR& GetVkSwapchainKHR();
		static bool DepthClampEnabled();
		static bool DepthBiasClampEnabled();

		// Frame logic:
		static void UpdateFrameIndex();
		static void ResetFrameIndex();
		static void WaitDeviceIdle();
	};
}



#endif // __INCLUDE_GUARD_vulkanContext_h__