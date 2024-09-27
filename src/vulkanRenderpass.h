#ifndef __INCLUDE_GUARD_vulkanRenderpass_h__
#define __INCLUDE_GUARD_vulkanRenderpass_h__
#include "vulkanLogicalDevice.h"



/// <summary>
/// VulkanRenderpass wrapper.
/// </summary>
class VulkanRenderpass
{
public: // Members:
	VkRenderPass renderpass;

private: // Members:
	VulkanLogicalDevice* logicalDevice;

public: // Methods:
	VulkanRenderpass(VulkanLogicalDevice* logicalDevice, VkFormat surfaceFormat, VkSampleCountFlagBits msaaSamples);
	~VulkanRenderpass();
};



#endif // __INCLUDE_GUARD_vulkanRenderpass_h__