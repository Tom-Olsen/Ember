#ifndef __INCLUDE_GUARD_vulkanRenderpass_h__
#define __INCLUDE_GUARD_vulkanRenderpass_h__
#include "vulkanContext.h"



/// <summary>
/// VulkanRenderpass wrapper.
/// </summary>
class VulkanRenderpass
{
public: // Members:
	VkRenderPass renderpass;

private: // Members:
	VulkanContext* context;

public: // Methods:
	VulkanRenderpass(VulkanContext* context, VkSampleCountFlagBits msaaSamples);
	~VulkanRenderpass();
};



#endif // __INCLUDE_GUARD_vulkanRenderpass_h__