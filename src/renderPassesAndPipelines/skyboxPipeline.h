#ifndef __INCLUDE_GUARD_skyboxPipeline_h__
#define __INCLUDE_GUARD_skyboxPipeline_h__
#include "pipeline.h"
#include <vector>
#include <vulkan/vulkan.h>



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
		const std::vector<VkDescriptorSetLayoutBinding>& bindings,
		const std::vector<VkVertexInputBindingDescription>& bindingDescriptions,
		const std::vector<VkVertexInputAttributeDescription>& attributeDescriptions);
	~SkyboxPipeline();

private: // Methods:
	void CreatePipelineLayout(const std::vector<VkDescriptorSetLayoutBinding>& bindings);
	void CreatePipeline(const VkShaderModule& vertexShaderModule, const VkShaderModule& fragmentShaderModule,
		const std::vector<VkVertexInputBindingDescription>& bindingDescriptions,
		const std::vector<VkVertexInputAttributeDescription>& attributeDescriptions);
};



#endif // __INCLUDE_GUARD_skyboxPipeline_h__