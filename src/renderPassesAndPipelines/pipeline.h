#ifndef __INCLUDE_GUARD_pipeline_h__
#define __INCLUDE_GUARD_pipeline_h__
#include "vulkanContext.h"



/// <summary>
/// Polymorphic parent class for all pipelines.
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
	virtual ~Pipeline();

protected: // Methods:
	void CreatePipelineLayout(const std::vector<VkDescriptorSetLayoutBinding>& bindings, VkShaderStageFlags flags);
	VkShaderModule CreateShaderModule(const std::vector<char>& code);
};



#endif // __INCLUDE_GUARD_pipeline_h__