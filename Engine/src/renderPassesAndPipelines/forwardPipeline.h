#ifndef __INCLUDE_GUARD_forwardPipeline_h__
#define __INCLUDE_GUARD_forwardPipeline_h__
#include "pipeline.h"
#include <vector>
#include <vulkan/vulkan.h>



namespace emberEngine
{
	// Forward declarations:
	struct VertexInputDescriptions;



	/// <summary>
	/// Pipeline associated with the ForwardRenderPass materials.
	/// </summary>
	class ForwardPipeline : public Pipeline
	{
	public: // Methods:
		ForwardPipeline
		(const std::vector<char>& vertexCode,
		 const std::vector<char>& fragmentCode,
		 const std::vector<VkDescriptorSetLayoutBinding>& vkDescriptorSetLayoutBindings,
		 const VertexInputDescriptions* const pVertexInputDescriptions);
		~ForwardPipeline();

	private: // Methods:
		void CreatePipelineLayout(const std::vector<VkDescriptorSetLayoutBinding>& vkDescriptorSetLayoutBindings);
		void CreatePipeline(const VkShaderModule& vertexShaderModule, const VkShaderModule& fragmentShaderModule, const VertexInputDescriptions* const pVertexInputDescriptions);
	};
}



#endif // __INCLUDE_GUARD_forwardPipeline_h__