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
		const std::vector<VkDescriptorSetLayoutBinding>& bindings);
	~ShadowPipeline();

private: // Methods:
	void CreatePipelineLayout(const std::vector<VkDescriptorSetLayoutBinding>& bindings);
	void CreatePipeline(const VkShaderModule& vertexShaderModule);
};



#endif // __INCLUDE_GUARD_shadowPipeline_h__