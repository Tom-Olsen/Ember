#pragma once
#ifndef __INCLUDE_GUARD_vulkanSampler_h__
#define __INCLUDE_GUARD_vulkanSampler_h__
#include <vulkan/vulkan.h>
#include "vulkanLogicalDevice.h"
#include "vulkanPhysicalDevice.h"



class VulkanSampler
{
public: // Members:
	VkSampler sampler;

private: // Members:
	VulkanLogicalDevice* logicalDevice;
	VulkanPhysicalDevice* physicalDevice;

public: // Methods:
	VulkanSampler(VulkanLogicalDevice* logicalDevice, VulkanPhysicalDevice* physicalDevice);
	~VulkanSampler();
};



#endif // __INCLUDE_GUARD_vulkanSampler_h__