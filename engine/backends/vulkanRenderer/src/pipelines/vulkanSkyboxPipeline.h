#pragma once
#include "vulkanPipeline.h"
#include <vector>



// Forward declarations:
typedef struct VkShaderModule_T* VkShaderModule;



namespace vulkanRendererBackend
{
	// Forward declarations:
	struct VertexInputDescriptions;



	template<typename vertexLayout>
	class SkyboxPipeline : public Pipeline
	{
	public: // Methods:
		SkyboxPipeline(const std::string& name, const std::vector<char>& vertexCode, const std::vector<char>& fragmentCode, const std::vector<VkDescriptorSetLayout>& vkDescriptorSetLayouts, const std::vector<VkVertexInputBindingDescription>& vertexBindings, const std::vector<VkVertexInputAttributeDescription>& vertexAttributes);
		~SkyboxPipeline();

	private: // Methods:
		void CreatePipelineLayout(const std::vector<VkDescriptorSetLayout>& vkDescriptorSetLayouts);
		void CreatePipeline(const VkShaderModule& vertexShaderModule, const VkShaderModule& fragmentShaderModule, const std::vector<VkVertexInputBindingDescription>& vertexBindings, const std::vector<VkVertexInputAttributeDescription>& vertexAttributes);
	};
}