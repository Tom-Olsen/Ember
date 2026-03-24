#pragma once
#include "commonPipelineState.h"
#include "vulkanPipeline.h"
#include <vector>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	/// <summary>
	/// For rendering to forward renderpass render target.
	/// </summary>
	class ForwardPipeline : public Pipeline
	{
	public: // Methods:
		ForwardPipeline(
			const std::string& name,
			VkPipelineLayout vkPipelineLayout,
			emberCommon::RenderMode renderMode,
			const std::vector<char>& vertexCode,
			const std::vector<char>& fragmentCode,
			const std::vector<VkVertexInputBindingDescription>& vertexBindings,
			const std::vector<VkVertexInputAttributeDescription>& vertexAttributes);
		~ForwardPipeline();

	private: // Methods:
		void CreatePipeline(
			VkPipelineLayout vkPipelineLayout,
			const VkShaderModule& vertexShaderModule,
			const VkShaderModule& fragmentShaderModule,
			const std::vector<VkVertexInputBindingDescription>& vertexBindings,
			const std::vector<VkVertexInputAttributeDescription>& vertexAttributes,
			emberCommon::RenderMode renderMode);
	};
}