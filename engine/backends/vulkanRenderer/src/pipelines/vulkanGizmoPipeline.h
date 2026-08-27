#pragma once
#include "commonGizmoRenderMode.h"
#include "vulkanPipeline.h"
#include <string>
#include <vector>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	class GizmoPipeline : public Pipeline
	{
	public: // Methods:
		// Constructor/Destructor:
		GizmoPipeline(
			VkPipelineLayout vkPipelineLayout,
			emberCommon::GizmoRenderMode renderMode,
			const std::vector<char>& vertexCode,
			const std::vector<char>& fragmentCode,
			const std::vector<VkVertexInputBindingDescription>& vertexBindings,
			const std::vector<VkVertexInputAttributeDescription>& vertexAttributes,
			const std::string& debugName);
		~GizmoPipeline();

	private: // Methods:
		void CreatePipeline(
			VkPipelineLayout vkPipelineLayout,
			emberCommon::GizmoRenderMode renderMode,
			const VkShaderModule& vertexShaderModule,
			const VkShaderModule& fragmentShaderModule,
			const std::vector<VkVertexInputBindingDescription>& vertexBindings,
			const std::vector<VkVertexInputAttributeDescription>& vertexAttributes);
	};
}