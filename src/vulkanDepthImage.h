#pragma once
#ifndef __INCLUDE_GUARD_vulkanDepthImage_h__
#define __INCLUDE_GUARD_vulkanDepthImage_h__
#include <vulkan/vulkan.h>
#include "vulkanLogicalDevice.h"
#include "vulkanPhysicalDevice.h"
#include "vulkanSurface.h"




class VulkanDepthImage
{
public: // Members:
	VkImage image;
	VkDeviceMemory memory;
	VkImageView imageView;
	static VkFormat format;

private: // Members:
	VulkanLogicalDevice* logicalDevice;
	VulkanPhysicalDevice* physicalDevice;
	VulkanSurface* surface;

public: // Methods:
	VulkanDepthImage(VulkanLogicalDevice* logicalDevice, VulkanPhysicalDevice* physicalDevice, VulkanSurface* surface);
	~VulkanDepthImage();

private: // Methods:
	void CreateImage(VkImageType imageType, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, uint32_t width, uint32_t height, uint32_t depth);
	void AllocateAndBindMemory();
	void CreateImageView(VkFormat format, const VkImageSubresourceRange& subresourceRange);
	void TransitionLayout(const VkImageSubresourceRange& subresourceRange);
};



#endif // __INCLUDE_GUARD_vulkanDepthImage_h__