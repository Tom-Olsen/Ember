#ifndef __INCLUDE_GUARD_shadowPipeline_h__
#define __INCLUDE_GUARD_shadowPipeline_h__
#include "pipeline.h"
#include <vector>
#include <vulkan/vulkan.h>



namespace emberEngine
{
	// Forward declarations:
	struct VertexInputDescriptions;



	/// <summary>
	/// Pipeline associated with the ShadowRenderPass materials.
	/// </summary>
	class ShadowPipeline : public Pipeline
	{
	public: // Methods:
		ShadowPipeline
		(const std::vector<char>& vertexCode,
		 const std::vector<VkDescriptorSetLayoutBinding>& vkDescriptorSetLayoutBindings,
		 const VertexInputDescriptions* const pVertexInputDescriptions);
		~ShadowPipeline();

	private: // Methods:
		void CreatePipelineLayout(const std::vector<VkDescriptorSetLayoutBinding>& vkDescriptorSetLayoutBindings);
		void CreatePipeline(const VkShaderModule& vertexShaderModule, const VertexInputDescriptions* const pVertexInputDescriptions);
	};
}



#endif // __INCLUDE_GUARD_shadowPipeline_h__