#ifndef __INCLUDE_GUARD_vulkanFrameBuffers_h__
#define __INCLUDE_GUARD_vulkanFrameBuffers_h__
#include <vulkan/vulkan.h>
#include <memory>
#include <vector>
#include "vulkanContext.h"
#include "vulkanSwapchain.h"
#include "vulkanRenderpass.h"
#include "vulkanDepthImage.h"
#include "vulkanMsaaImage.h"



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
	VulkanContext* context;

public: // Methods:
	VulkanFrameBuffers(VulkanContext* context, VulkanDepthImage* depthImage, VulkanMsaaImage* msaaImage);
	~VulkanFrameBuffers();
	VkFramebuffer& operator[](size_t index);
};



#endif // __INCLUDE_GUARD_vulkanFrameBuffers_h__