#ifndef __INCLUDE_GUARD_vulkanContext_h__
#define __INCLUDE_GUARD_vulkanContext_h__
#include <vulkan\vulkan.h>
#include "sdlWindow.h"
#include "vulkanInstance.h"
#include "vulkanPhysicalDevice.h"
#include "vulkanSurface.h"
#include "vulkanLogicalDevice.h"
#include "vulkanMemoryAllocator.h"
#include "vulkanDescriptorPool.h"
#include "vulkanSwapchain.h"



class VulkanContext
{
public: // Members:
	std::unique_ptr<SdlWindow> window;
	std::unique_ptr<VulkanInstance> instance;
	std::unique_ptr<VulkanPhysicalDevice> physicalDevice;
	std::unique_ptr<VulkanSurface> surface;
	std::unique_ptr<VulkanLogicalDevice> logicalDevice;
	std::unique_ptr<VulkanMemoryAllocator> allocator;
	std::unique_ptr<VulkanDescriptorPool> descriptorPool;
	std::unique_ptr<VulkanSwapchain> swapchain;
	uint32_t framesInFlight;
	uint32_t frameIndex;
	VkSampleCountFlagBits msaaSamples;

public: // Methods:
	VulkanContext(uint32_t framesInFlight, VkSampleCountFlagBits msaaSamples);
	~VulkanContext();

	// Getters:
	SDL_Window* Window();
	VkInstance& Instance();
	VkPhysicalDevice& PhysicalDevice();
	VkSurfaceKHR& Surface();
	VkDevice& LogicalDevice();
	VmaAllocator& Allocator();
	VkDescriptorPool& DescriptorPool();
	VkSwapchainKHR& Swapchain();
	bool DepthClampEnabled();
	bool DepthBiasEnabled();

	// Frame logic:
	void UpdateFrameIndex();
	void ResetFrameIndex();
};



#endif // __INCLUDE_GUARD_vulkanContext_h__