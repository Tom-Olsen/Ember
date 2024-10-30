#ifndef __INCLUDE_GUARD_pipeline_h__
#define __INCLUDE_GUARD_pipeline_h__
#include "vulkanContext.h"



/// <summary>
/// Parent class for all pipelines.
/// </summary>
class Pipeline
{
public: // Members:
	VkDescriptorSetLayout descriptorSetLayout;
	VkPipelineLayout pipelineLayout;
	VkPipeline pipeline;

protected: // Members:
	VulkanContext* context;

public: // Methods:
	~Pipeline();
};



#endif // __INCLUDE_GUARD_pipeline_h__