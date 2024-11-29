#ifndef __INCLUDE_GUARD_shadingPipeline_h__
#define __INCLUDE_GUARD_shadingPipeline_h__
#include <string>
#include "pipeline.h"
#include "vulkanContext.h"
#include "shadingRenderPass.h"
#include "mathf.h"



/// <summary>
/// Pipeline associated with the ShadingRenderPass materials.
/// </summary>
class ShadingPipeline : public Pipeline
{
public: // Methods:
	ShadingPipeline(VulkanContext* context,
		const std::vector<char>& vertexCode,
		const std::vector<char>& fragmentCode,
		const std::vector<VkDescriptorSetLayoutBinding>& bindings);
	~ShadingPipeline();

private: // Methods:
	void CreatePipelineLayout(const std::vector<VkDescriptorSetLayoutBinding>& bindings);
	void CreatePipeline(const VkShaderModule& vertexShaderModule, const VkShaderModule& fragmentShaderModule);
};



#endif // __INCLUDE_GUARD_shadingPipeline_h__