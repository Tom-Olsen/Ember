#ifndef __INCLUDE_GUARD_vulkanRendererBackend_vulkanForwardTransparentPipeline_h__
#define __INCLUDE_GUARD_vulkanRendererBackend_vulkanForwardTransparentPipeline_h__
#include "vulkanPipeline.h"
#include <vector>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Forward declarations:
	struct VertexInputDescriptions;



	/// <summary>
	/// Pipeline associated with the ForwardRenderPass materials.
	/// </summary>
	class ForwardTransparentPipeline : public Pipeline
	{
	public: // Methods:
		ForwardTransparentPipeline
		(const std::vector<char>& vertexCode,
			const std::vector<char>& fragmentCode,
			const std::vector<VkDescriptorSetLayoutBinding>& vkDescriptorSetLayoutBindings,
			const VertexInputDescriptions* const pVertexInputDescriptions);
		~ForwardTransparentPipeline();

	private: // Methods:
		void CreatePipelineLayout(const std::vector<VkDescriptorSetLayoutBinding>& vkDescriptorSetLayoutBindings);
		void CreatePipeline(const VkShaderModule& vertexShaderModule, const VkShaderModule& fragmentShaderModule, const VertexInputDescriptions* const pVertexInputDescriptions);
	};
}



#endif // __INCLUDE_GUARD_vulkanRendererBackend_vulkanForwardTransparentPipeline_h__