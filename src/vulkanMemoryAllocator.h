#pragma once
#ifndef __INCLUDE_GUARD_vulkanMemoryAllocator_h__
#define __INCLUDE_GUARD_vulkanMemoryAllocator_h__
#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 1
#define VMA_VULKAN_VERSION 1003000
#include "vk_mem_alloc.h"
#include "vulkanInstance.h"
#include "vulkanLogicalDevice.h"
#include "vulkanPhysicalDevice.h"



class VulkanMemoryAllocator
{
public: // Members:
	VmaAllocator allocator;

private: // Members:

public: // Methods:
	VulkanMemoryAllocator(VulkanInstance* instance, VulkanLogicalDevice* logicalDevice, VulkanPhysicalDevice* physicalDevice);
	~VulkanMemoryAllocator();
private: // Methods:
};



#endif // __INCLUDE_GUARD_vulkanMemoryAllocator_h__