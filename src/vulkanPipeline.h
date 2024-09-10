#ifndef __INCLUDE_GUARD_vulkanPipeline_h__
#define __INCLUDE_GUARD_vulkanPipeline_h__
#include <string>
#include "vulkanLogicalDevice.h"
#include "vulkanPipelineLayout.h"
#include "vulkanRenderpass.h"
#include "glmTypes.h"



/// <summary>
/// Pipeline is immutable and must be recreated from scratch if any of its state changes.
/// It is also expensive to create, so it is recommended to create them at initialization and not during rendering.
/// The only exception are the dynamic states, which can be changed without recreating the pipeline.
/// These include viewport, scissor, line width, depth bias, blend constants, and stencil compare mask.
/// </summary>
class VulkanPipeline
{
public: // Members:
	VkPipeline pipeline;

private: // Members:
	VulkanLogicalDevice* logicalDevice;
	VulkanPipelineLayout* pipelineLayout;
	VulkanRenderpass* renderpass;

public: // Methods:
	VulkanPipeline(VulkanLogicalDevice* logicalDevice, VulkanPipelineLayout* pipelineLayout, VulkanRenderpass* renderpass, const std::string& vertexShaderFilename, const std::string& fragmentShaderFilename);
	~VulkanPipeline();

private: // Methods:
	VkShaderModule CreateShaderModule(std::string shaderFilename);
	void CreatePipeline(const VkShaderModule& vertexShaderModule, const VkShaderModule& fragmentShaderModule);
};



#endif // __INCLUDE_GUARD_vulkanPipeline_h__