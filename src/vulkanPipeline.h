#ifndef __INCLUDE_GUARD_vulkanPipeline_h__
#define __INCLUDE_GUARD_vulkanPipeline_h__
#include <string>
#include "vulkanLogicalDevice.h"
#include "vulkanRenderpass.h"
#include "glmTypes.h"



class VulkanPipeline
{
public: // Members:
	VkPipeline pipeline;
	VkPipelineLayout pipelineLayout;

private: // Members:
	VulkanLogicalDevice* logicalDevice;
	VulkanRenderpass* renderpass;

public: // Methods:
	VulkanPipeline(VulkanLogicalDevice* logicalDevice, VulkanRenderpass* renderpass, const std::string& vertexShaderFilename, const std::string& fragmentShaderFilename);
	~VulkanPipeline();

private: // Methods:
	VkShaderModule CreateShaderModule(std::string shaderFilename);
	void CreatePipeline(const VkShaderModule& vertexShaderModule, const VkShaderModule& fragmentShaderModule);
};



#endif // __INCLUDE_GUARD_vulkanPipeline_h__