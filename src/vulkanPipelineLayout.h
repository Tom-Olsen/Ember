#ifndef __INCLUDE_GUARD_vulkanPipelineLayout_h__
#define __INCLUDE_GUARD_vulkanPipelineLayout_h__
#include "vulkanLogicalDevice.h"



class VulkanPipelineLayout
{
private: // Members:
	VulkanLogicalDevice* logicalDevice;
	VkDescriptorSetLayout descriptorSetLayout;

public: // Members:
	VkPipelineLayout pipelineLayout;

public: // Methods:
	VulkanPipelineLayout(VulkanLogicalDevice* logicalDevice);
	~VulkanPipelineLayout();
};



#endif // __INCLUDE_GUARD_vulkanPipelineLayout_h__