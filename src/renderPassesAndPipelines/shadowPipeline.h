#ifndef __INCLUDE_GUARD_shadowPipeline_h__
#define __INCLUDE_GUARD_shadowPipeline_h__
#include "pipeline.h"
#include <vector>
#include <vulkan/vulkan.h>



struct VulkanContext;



/// <summary>
/// Pipeline associated with the ShadowRenderPass materials.
/// </summary>
class ShadowPipeline : public Pipeline
{
public: // Methods:
	ShadowPipeline(VulkanContext* pContext,
		const std::vector<char>& vertexCode,
		const std::vector<VkDescriptorSetLayoutBinding>& descriptorSetBindings,
		const std::vector<VkVertexInputBindingDescription>& inputBindingDescriptions,
		const std::vector<VkVertexInputAttributeDescription>& inputAttributeDescriptions);
	~ShadowPipeline();

private: // Methods:
	void CreatePipelineLayout(const std::vector<VkDescriptorSetLayoutBinding>& descriptorSetBindings);
	void CreatePipeline(const VkShaderModule& vertexShaderModule,
		const std::vector<VkVertexInputBindingDescription>& inputBindingDescriptions,
		const std::vector<VkVertexInputAttributeDescription>& inputAttributeDescriptions);
};



#endif // __INCLUDE_GUARD_shadowPipeline_h__