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
	class ShadowPipeline : public Pipeline
	{
	public: // Methods:
		ShadowPipeline(const std::string& name, uint32_t shadowMapResolution, const std::vector<char>& vertexCode, const std::vector<VkDescriptorSetLayout>& vkDescriptorSetLayouts, const std::vector<VkVertexInputBindingDescription>& vertexBindings, const std::vector<VkVertexInputAttributeDescription>& vertexAttributes);
		~ShadowPipeline();

	private: // Methods:
		void CreatePipelineLayout(const std::vector<VkDescriptorSetLayout>& vkDescriptorSetLayouts);
		void CreatePipeline(uint32_t shadowMapResolution, const VkShaderModule& vertexShaderModule, const std::vector<VkVertexInputBindingDescription>& vertexBindings, const std::vector<VkVertexInputAttributeDescription>& vertexAttributes);
	};
}