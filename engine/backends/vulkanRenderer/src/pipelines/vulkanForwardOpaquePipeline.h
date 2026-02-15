#pragma once
#include "vulkanPipeline.h"
#include <vector>



// Forward declarations:
typedef struct VkShaderModule_T* VkShaderModule;



namespace vulkanRendererBackend
{
	class ForwardOpaquePipeline : public Pipeline
	{
	public: // Methods:
		ForwardOpaquePipeline(const std::string& name, const std::vector<char>& vertexCode, const std::vector<char>& fragmentCode, const std::vector<VkDescriptorSetLayout>& vkDescriptorSetLayouts, const std::vector<VkVertexInputBindingDescription>& vertexBindings, const std::vector<VkVertexInputAttributeDescription>& vertexAttributes);
		~ForwardOpaquePipeline();

	private: // Methods:
		void CreatePipelineLayout(const std::vector<VkDescriptorSetLayout>& vkDescriptorSetLayouts);
		void CreatePipeline(const VkShaderModule& vertexShaderModule, const VkShaderModule& fragmentShaderModule, const std::vector<VkVertexInputBindingDescription>& vertexBindings, const std::vector<VkVertexInputAttributeDescription>& vertexAttributes);
	};
}