#pragma once
#include "vulkanPipeline.h"
#include <vector>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	/// <summary>
	/// For rendering to shadowMap atlas of shadow renderpass.
	/// </summary>
	class ShadowPipeline : public Pipeline
	{
	public: // Methods:
		ShadowPipeline(
			const std::string& name,
			VkPipelineLayout vkPipelineLayout,
			uint32_t shadowMapResolution,
			const std::vector<char>& vertexCode,
			const std::vector<VkVertexInputBindingDescription>& vertexBindings,
			const std::vector<VkVertexInputAttributeDescription>& vertexAttributes);
		~ShadowPipeline();

	private: // Methods:
		void CreatePipeline(
			uint32_t shadowMapResolution,
			VkPipelineLayout vkPipelineLayout,
			const VkShaderModule& vertexShaderModule,
			const std::vector<VkVertexInputBindingDescription>& vertexBindings,
			const std::vector<VkVertexInputAttributeDescription>& vertexAttributes);
	};
}