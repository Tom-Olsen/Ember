#pragma once
#ifndef __INCLUDE_GUARD_vulkanDescriptorPool_h__
#define __INCLUDE_GUARD_vulkanDescriptorPool_h__
#include <vulkan/vulkan.h>
#include "vulkanContext.h"
#include "vulkanUniformBuffer.h"
#include "texture2d.h"
#include "memory"



class VulkanDescriptorPool
{
public: // Members:
	VkDescriptorPool descriptorPool;

private: // Members:
	VulkanContext* context;

public: // Methods:
	VulkanDescriptorPool(size_t framesInFlight, VulkanContext* context);
	~VulkanDescriptorPool();
};



#endif // __INCLUDE_GUARD_vulkanDescriptorPool_h__