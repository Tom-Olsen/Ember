#pragma once
#ifndef __INCLUDE_GUARD_vulkanDepthImage_h__
#define __INCLUDE_GUARD_vulkanDepthImage_h__
#include <vulkan/vulkan.h>
#include "vulkanContext.h"
#include "vmaImage.h"




class VulkanDepthImage
{
public: // Members:
	std::unique_ptr<VmaImage> image;
	static VkFormat depthFormat;

private: // Members:
	VulkanContext* context;

public: // Methods:
	VulkanDepthImage(VulkanContext* context);
	~VulkanDepthImage();

private: // Methods:
	void TransitionLayout(const VkImageSubresourceRange& subresourceRange);
};



#endif // __INCLUDE_GUARD_vulkanDepthImage_h__