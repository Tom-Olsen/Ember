#pragma once
#include "vulkanPipeline.h"
#include <vector>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Forward declarations:
	struct VertexInputDescriptions;



	/// <summary>
	/// Pipeline associated with the PresentRenderPass materials.
	/// </summary>
	class PresentPipeline : public Pipeline
	{
	public: // Methods:
		PresentPipeline
		(const std::vector<char>& vertexCode,
			const std::vector<char>& fragmentCode,
			const std::vector<VkDescriptorSetLayoutBinding>& vkDescriptorSetLayoutBindings,
			const VertexInputDescriptions* const pVertexInputDescriptions);
		~PresentPipeline();

	private: // Methods:
		void CreatePipelineLayout(const std::vector<VkDescriptorSetLayoutBinding>& vkDescriptorSetLayoutBindings);
		void CreatePipeline(const VkShaderModule& vertexShaderModule, const VkShaderModule& fragmentShaderModule, const VertexInputDescriptions* const pVertexInputDescriptions);
	};
}