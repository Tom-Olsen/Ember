#ifndef __INCLUDE_GUARD_vulkanFrameBuffers_h__
#define __INCLUDE_GUARD_vulkanFrameBuffers_h__
#include <vulkan/vulkan.h>
#include <memory>
#include <vector>
#include "vulkanLogicalDevice.h"
#include "vulkanSurface.h"
#include "vulkanSwapchain.h"
#include "vulkanRenderpass.h"



/// <summary>
/// VulkanFrameBuffer class owns framebuffers.
/// Size = swapchain image count.
/// </summary>
class VulkanFrameBuffers
{
public: // Members:
	size_t size;

private: // Members:
	std::vector<VkFramebuffer> frameBuffers;
	VulkanLogicalDevice* logicalDevice;
	VulkanSurface* surface;
	VulkanRenderpass* renderpass;

public: // Methods:
	VulkanFrameBuffers(VulkanLogicalDevice* logicalDevice, VulkanSurface* surface, VulkanSwapchain* swapchain, VulkanRenderpass* renderpass);
	~VulkanFrameBuffers();
	void Recreate(VulkanSwapchain* swapchain, VulkanRenderpass* renderpass);
	VkFramebuffer& operator[](size_t index);
};



#endif // __INCLUDE_GUARD_vulkanFrameBuffers_h__