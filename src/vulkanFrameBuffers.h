#ifndef __INCLUDE_GUARD_vulkanFrameBuffers_h__
#define __INCLUDE_GUARD_vulkanFrameBuffers_h__
#include <vulkan/vulkan.h>
#include <memory>
#include <vector>
#include "vulkanLogicalDevice.h"
#include "vulkanSurface.h"
#include "vulkanSwapchain.h"
#include "vulkanRenderpass.h"


class VulkanFrameBuffers
{
private: // Members:
	std::vector<VkFramebuffer> frameBuffers;
	VulkanLogicalDevice* logicalDevice;
	VulkanSurface* surface;
	VulkanRenderpass* renderpass;


public: // Members:
	size_t size;

public: // Methods:
	VulkanFrameBuffers(VulkanLogicalDevice* logicalDevice, VulkanSurface* surface, VulkanSwapchain* swapchain, VulkanRenderpass* renderpass);
	~VulkanFrameBuffers();
	void Recreate(VulkanSwapchain* swapchain);
	VkFramebuffer& operator[](size_t index);
};
#endif // __INCLUDE_GUARD_vulkanFrameBuffers_h__