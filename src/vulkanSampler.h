#pragma once
#ifndef __INCLUDE_GUARD_vulkanSampler_h__
#define __INCLUDE_GUARD_vulkanSampler_h__
#include <vulkan/vulkan.h>
#include <string>
#include "vulkanContext.h"



class VulkanSampler
{
public: // Members:
	VkSampler sampler;
	std::string name;

private: // Members:
	VulkanContext* context;

public: // Methods:
	VulkanSampler(VulkanContext* context, std::string name);
	~VulkanSampler();
};



#endif // __INCLUDE_GUARD_vulkanSampler_h__