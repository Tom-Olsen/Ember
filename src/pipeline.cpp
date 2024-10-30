#include "pipeline.h"



// Destructor:
Pipeline::~Pipeline()
{
	vkDestroyDescriptorSetLayout(context->LogicalDevice(), descriptorSetLayout, nullptr);
	vkDestroyPipelineLayout(context->LogicalDevice(), pipelineLayout, nullptr);
	vkDestroyPipeline(context->LogicalDevice(), pipeline, nullptr);
}