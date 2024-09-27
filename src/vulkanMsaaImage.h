#pragma once
#ifndef __INCLUDE_GUARD_vulkanMsaaImage_h__
#define __INCLUDE_GUARD_vulkanMsaaImage_h__
#include <vulkan/vulkan.h>
#include "vulkanLogicalDevice.h"
#include "vulkanPhysicalDevice.h"
#include "vulkanSurface.h"



class VulkanMsaaImage
{
public: // Members:
	int width;
	int height;
	VkImage image;
	VkDeviceMemory memory;
	VkImageView imageView;

private: // Members:
	VulkanLogicalDevice* logicalDevice;
	VulkanPhysicalDevice* physicalDevice;

public: // Methods:
	VulkanMsaaImage(VulkanLogicalDevice* logicalDevice, VulkanPhysicalDevice* physicalDevice, VulkanSurface* surface);
	~VulkanMsaaImage();
};



#endif // __INCLUDE_GUARD_vulkanMsaaImage_h__