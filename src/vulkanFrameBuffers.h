#ifndef __INCLUDE_GUARD_vulkanFrameBuffers_h__
#define __INCLUDE_GUARD_vulkanFrameBuffers_h__
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

private: // Members:
	std::vector<VkFramebuffer> framebuffers;
	VulkanContext* context;

public: // Methods:
	VulkanFrameBuffers(VulkanContext* context, VulkanDepthImage* depthImage, VulkanMsaaImage* msaaImage);
	~VulkanFrameBuffers();
	VkFramebuffer& operator[](size_t index);
};



#endif // __INCLUDE_GUARD_vulkanFrameBuffers_h__