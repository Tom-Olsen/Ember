#pragma once
#include "vulkanPipeline.h"
#include "vulkanDescriptorSetLayoutBinding.h"
#include <vector>



// Forward declarations:
struct VkDescriptorSetLayoutBinding;
typedef struct VkShaderModule_T* VkShaderModule;



namespace vulkanRendererBackend
{
	// Forward declarations:
	struct VertexInputDescriptions;



	/// <summary>
	/// Pipeline associated with the skybox materials.
	/// </summary>
	class SkyboxPipeline : public Pipeline
	{
	public: // Methods:
		SkyboxPipeline(const std::string& name, const std::vector<char>& vertexCode, const std::vector<char>& fragmentCode, std::vector<DescriptorSetLayoutBinding>& descriptorSetLayoutBindings, VertexInputDescriptions* pVertexInputDescriptions);
		~SkyboxPipeline();

	private: // Methods:
		void CreatePipelineLayout(std::vector<DescriptorSetLayoutBinding>& descriptorSetLayoutBindings);
		void CreatePipeline(const VkShaderModule& vertexShaderModule, const VkShaderModule& fragmentShaderModule, VertexInputDescriptions* pVertexInputDescriptions);
	};
}