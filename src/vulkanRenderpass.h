#ifndef __INCLUDE_GUARD_vulkanRenderpass_h__
#define __INCLUDE_GUARD_vulkanRenderpass_h__
#include "vulkanLogicalDevice.h"
#include "vulkanSurface.h"



/// <summary>
/// VulkanRenderpass wrapper.
/// </summary>
class VulkanRenderpass
{
public: // Members:
	VkRenderPass renderpass;

private: // Members:
	VulkanLogicalDevice* logicalDevice;
	VulkanSurface* surface;

public: // Methods:
	VulkanRenderpass(VulkanLogicalDevice* logicalDevice, VulkanSurface* surface, VkSampleCountFlagBits msaaSamples);
	~VulkanRenderpass();
};



#endif // __INCLUDE_GUARD_vulkanRenderpass_h__