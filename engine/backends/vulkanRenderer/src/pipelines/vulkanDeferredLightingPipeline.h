#pragma once
#include "vulkanPipeline.h"
#include <string>
#include <vector>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	class DeferredLightingPipeline : public Pipeline
	{
	public: // Methods:
		// Constructor/Destructor:
		DeferredLightingPipeline(
			VkPipelineLayout vkPipelineLayout,
			const std::vector<char>& vertexCode,
			const std::vector<char>& fragmentCode,
			const std::vector<VkVertexInputBindingDescription>& vertexBindings,
			const std::vector<VkVertexInputAttributeDescription>& vertexAttributes,
			const std::string& debugName);
		~DeferredLightingPipeline();

	private: // Methods:
		void CreatePipeline(
			VkPipelineLayout vkPipelineLayout,
			const VkShaderModule& vertexShaderModule,
			const VkShaderModule& fragmentShaderModule,
			const std::vector<VkVertexInputBindingDescription>& vertexBindings,
			const std::vector<VkVertexInputAttributeDescription>& vertexAttributes);
	};
}