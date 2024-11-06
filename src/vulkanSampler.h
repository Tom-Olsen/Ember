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
	~VulkanSampler();
	VkPhysicalDeviceProperties GetDeviceProperties();

	// Static specialised constructors:
	static VulkanSampler* ColorSampler(VulkanContext* context, const std::string& name);
	static VulkanSampler* ShadowSampler(VulkanContext* context, const std::string& name);

private: // Methods:
	VulkanSampler(VulkanContext* context, const std::string& name);
};



#endif // __INCLUDE_GUARD_vulkanSampler_h__