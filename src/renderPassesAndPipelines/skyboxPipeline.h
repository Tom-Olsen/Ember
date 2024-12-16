#ifndef __INCLUDE_GUARD_skyboxPipeline_h__
#define __INCLUDE_GUARD_skyboxPipeline_h__
#include "pipeline.h"
#include <vector>
#include <vulkan/vulkan.h>



struct VertexInputDescriptions;
struct VulkanContext;



/// <summary>
/// Pipeline associated with the skybox materials.
/// </summary>
class SkyboxPipeline : public Pipeline
{
public: // Methods:
	SkyboxPipeline(VulkanContext* pContext,
		const std::vector<char>& vertexCode,
		const std::vector<char>& fragmentCode,
		const std::vector<VkDescriptorSetLayoutBinding>& vkDescriptorSetLayoutBindings,
		const VertexInputDescriptions* const pVertexInputDescriptions);
	~SkyboxPipeline();

private: // Methods:
	void CreatePipelineLayout(const std::vector<VkDescriptorSetLayoutBinding>& vkDescriptorSetLayoutBindings);
	void CreatePipeline(const VkShaderModule& vertexShaderModule, const VkShaderModule& fragmentShaderModule, const VertexInputDescriptions* const pVertexInputDescriptions);
};



#endif // __INCLUDE_GUARD_skyboxPipeline_h__