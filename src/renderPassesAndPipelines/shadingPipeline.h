#ifndef __INCLUDE_GUARD_shadingPipeline_h__
#define __INCLUDE_GUARD_shadingPipeline_h__
#include "pipeline.h"
#include <vector>
#include <vulkan/vulkan.h>



struct VulkanContext;



/// <summary>
/// Pipeline associated with the ShadingRenderPass materials.
/// </summary>
class ShadingPipeline : public Pipeline
{
public: // Methods:
	ShadingPipeline(VulkanContext* pContext,
		const std::vector<char>& vertexCode,
		const std::vector<char>& fragmentCode,
		const std::vector<VkDescriptorSetLayoutBinding>& bindings);
	~ShadingPipeline();

private: // Methods:
	void CreatePipelineLayout(const std::vector<VkDescriptorSetLayoutBinding>& bindings);
	void CreatePipeline(const VkShaderModule& vertexShaderModule, const VkShaderModule& fragmentShaderModule);
};



#endif // __INCLUDE_GUARD_shadingPipeline_h__