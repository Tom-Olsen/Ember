#pragma once
#include "vulkanPipeline.h"
#include <vector>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	/// <summary>
	/// Renders finallized image into swapchain via presentRenderPass.
	/// </summary>
	class PresentPipeline : public Pipeline
	{
	public: // Methods:
		// Constructor/Destructor:
		PresentPipeline(
			VkPipelineLayout vkPipelineLayout,
			const std::vector<char>& vertexCode,
			const std::vector<char>& fragmentCode,
			const std::vector<VkVertexInputBindingDescription>& vertexBindings,
			const std::vector<VkVertexInputAttributeDescription>& vertexAttributes,
			const std::string& debugName);
		~PresentPipeline();

	private: // Methods:
		void CreatePipeline(
			VkPipelineLayout vkPipelineLayout,
			const VkShaderModule& vertexShaderModule,
			const VkShaderModule& fragmentShaderModule,
			const std::vector<VkVertexInputBindingDescription>& vertexBindings,
			const std::vector<VkVertexInputAttributeDescription>& vertexAttributes);
	};
}