#ifndef __INCLUDE_GUARD_forwardPipeline_h__
#define __INCLUDE_GUARD_forwardPipeline_h__
#include <string>
#include "pipeline.h"
#include "vulkanContext.h"
#include "forwardRenderPass.h"
#include "glmTypes.h"
#include "pushConstantObject.h"



/// <summary>
/// Pipeline associated with the ForwardRenderPass.
/// </summary>
class ForwardPipeline : public Pipeline
{
public: // Methods:
	ForwardPipeline(VulkanContext* context, VkRenderPass* renderPass,
		const std::vector<char>& vertexCode,
		const std::vector<char>& fragmentCode,
		const std::vector<VkDescriptorSetLayoutBinding>& bindings);
	~ForwardPipeline();

private: // Methods:
	void CreatePipelineLayout(const std::vector<VkDescriptorSetLayoutBinding>& bindings);
	VkShaderModule CreateShaderModule(const std::vector<char>& code);
	void CreatePipeline(VkRenderPass* renderPass, const VkShaderModule& vertexShaderModule, const VkShaderModule& fragmentShaderModule);
};



#endif // __INCLUDE_GUARD_forwardPipeline_h__