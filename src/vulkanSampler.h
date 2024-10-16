#pragma once
#ifndef __INCLUDE_GUARD_vulkanSampler_h__
#define __INCLUDE_GUARD_vulkanSampler_h__
#include <vulkan/vulkan.h>
#include "vulkanContext.h"



class VulkanSampler
{
public: // Members:
	VkSampler sampler;

private: // Members:
	VulkanContext* context;

public: // Methods:
	VulkanSampler(VulkanContext* context);
	~VulkanSampler();
};



#endif // __INCLUDE_GUARD_vulkanSampler_h__