#ifndef __INCLUDE_GUARD_vulkanPipelineLayout_h__
#define __INCLUDE_GUARD_vulkanPipelineLayout_h__
#include "vulkanLogicalDevice.h"



class VulkanPipelineLayout
{
public: // Members:
	VkPipelineLayout pipelineLayout;
	VkDescriptorSetLayout descriptorSetLayout;

private: // Members:
	VulkanLogicalDevice* logicalDevice;

public: // Methods:
	VulkanPipelineLayout(VulkanLogicalDevice* logicalDevice);
	~VulkanPipelineLayout();
};



#endif // __INCLUDE_GUARD_vulkanPipelineLayout_h__