#ifndef __INCLUDE_GUARD_shadingPipeline_h__
#define __INCLUDE_GUARD_shadingPipeline_h__
#include "pipeline.h"
#include <vector>
#include <vulkan/vulkan.h>



struct VertexInputDescriptions;
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
		const std::vector<VkDescriptorSetLayoutBinding>& vkDescriptorSetLayoutBindings,
		const VertexInputDescriptions* const pVertexInputDescriptions);
	~ShadingPipeline();

private: // Methods:
	void CreatePipelineLayout(const std::vector<VkDescriptorSetLayoutBinding>& vkDescriptorSetLayoutBindings);
	void CreatePipeline(const VkShaderModule& vertexShaderModule, const VkShaderModule& fragmentShaderModule, const VertexInputDescriptions* const pVertexInputDescriptions);
};



#endif // __INCLUDE_GUARD_shadingPipeline_h__