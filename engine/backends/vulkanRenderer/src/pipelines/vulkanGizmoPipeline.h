#pragma once
#include "commonPipelineState.h"
#include "vulkanPipeline.h"
#include <string>
#include <vector>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	/// <summary>
	/// Pipeline for rendering gizmos into the gizmo render pass.
	/// </summary>
	class GizmoPipeline : public Pipeline
	{
	public: // Methods:
		// Constructor/Destructor:
		GizmoPipeline(
			const std::string& name,
			VkPipelineLayout vkPipelineLayout,
			emberCommon::RenderMode renderMode,
			const std::vector<char>& vertexCode,
			const std::vector<char>& fragmentCode,
			const std::vector<VkVertexInputBindingDescription>& vertexBindings,
			const std::vector<VkVertexInputAttributeDescription>& vertexAttributes);
		~GizmoPipeline();

	private: // Methods:
		void CreatePipeline(
			VkPipelineLayout vkPipelineLayout,
			emberCommon::RenderMode renderMode,
			const VkShaderModule& vertexShaderModule,
			const VkShaderModule& fragmentShaderModule,
			const std::vector<VkVertexInputBindingDescription>& vertexBindings,
			const std::vector<VkVertexInputAttributeDescription>& vertexAttributes);
	};
}