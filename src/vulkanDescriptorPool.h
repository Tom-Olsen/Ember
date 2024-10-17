#pragma once
#ifndef __INCLUDE_GUARD_vulkanDescriptorPool_h__
#define __INCLUDE_GUARD_vulkanDescriptorPool_h__
#include <vulkan/vulkan.h>
#include "vulkanLogicalDevice.h"



class VulkanDescriptorPool
{
public: // Members:
	VkDescriptorPool descriptorPool;

private: // Members:
	VulkanLogicalDevice* logicalDevice;

public: // Methods:
	VulkanDescriptorPool(VulkanLogicalDevice* logicalDevice);
	~VulkanDescriptorPool();
};



#endif // __INCLUDE_GUARD_vulkanDescriptorPool_h__