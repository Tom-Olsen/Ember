#ifndef __INCLUDE_GUARD_shadowPipeline_h__
#define __INCLUDE_GUARD_shadowPipeline_h__
#include <string>
#include "pipeline.h"
#include "vulkanContext.h"
#include "shadowRenderPass.h"
#include "mathf.h"



/// <summary>
/// Pipeline associated with the ShadowRenderPass.
/// </summary>
class ShadowPipeline : public Pipeline
{
public: // Methods:
	ShadowPipeline(VulkanContext* context,
		const std::vector<char>& vertexCode,
		const std::vector<VkDescriptorSetLayoutBinding>& bindings);
	~ShadowPipeline();

private: // Methods:
	void CreatePipelineLayout(const std::vector<VkDescriptorSetLayoutBinding>& bindings);
	VkShaderModule CreateShaderModule(const std::vector<char>& code);
	void CreatePipeline(const VkShaderModule& vertexShaderModule);
};



#endif // __INCLUDE_GUARD_shadowPipeline_h__