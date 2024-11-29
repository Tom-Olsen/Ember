#ifndef __INCLUDE_GUARD_skyboxPipeline_h__
#define __INCLUDE_GUARD_skyboxPipeline_h__
#include <string>
#include "pipeline.h"
#include "vulkanContext.h"
#include "shadingRenderPass.h"
#include "mathf.h"



/// <summary>
/// Pipeline associated with the skybox materials.
/// </summary>
class SkyboxPipeline : public Pipeline
{
public: // Methods:
	SkyboxPipeline(VulkanContext* context,
		const std::vector<char>& vertexCode,
		const std::vector<char>& fragmentCode,
		const std::vector<VkDescriptorSetLayoutBinding>& bindings);
	~SkyboxPipeline();

private: // Methods:
	void CreatePipelineLayout(const std::vector<VkDescriptorSetLayoutBinding>& bindings);
	void CreatePipeline(const VkShaderModule& vertexShaderModule, const VkShaderModule& fragmentShaderModule);
};



#endif // __INCLUDE_GUARD_skyboxPipeline_h__