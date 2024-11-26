#ifndef __INCLUDE_GUARD_forwardPipeline_h__
#define __INCLUDE_GUARD_forwardPipeline_h__
#include <string>
#include "pipeline.h"
#include "vulkanContext.h"
#include "forwardRenderPass.h"
#include "mathf.h"



/// <summary>
/// Pipeline associated with the ForwardRenderPass materials.
/// </summary>
class ForwardPipeline : public Pipeline
{
public: // Methods:
	ForwardPipeline(VulkanContext* context,
		const std::vector<char>& vertexCode,
		const std::vector<char>& fragmentCode,
		const std::vector<VkDescriptorSetLayoutBinding>& bindings);
	~ForwardPipeline();

private: // Methods:
	void CreatePipeline(const VkShaderModule& vertexShaderModule, const VkShaderModule& fragmentShaderModule);
};



#endif // __INCLUDE_GUARD_forwardPipeline_h__