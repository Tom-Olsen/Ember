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
	/// Pipeline associated with the PresentRenderPass materials.
	/// </summary>
	class PresentPipeline : public Pipeline
	{
	public: // Methods:
		PresentPipeline(const std::vector<char>& vertexCode, const std::vector<char>& fragmentCode, std::vector<DescriptorSetLayoutBinding>& descriptorSetLayoutBindings, VertexInputDescriptions* pVertexInputDescriptions);
		~PresentPipeline();

	private: // Methods:
		void CreatePipelineLayout(std::vector<DescriptorSetLayoutBinding>& descriptorSetLayoutBindings);
		void CreatePipeline(const VkShaderModule& vertexShaderModule, const VkShaderModule& fragmentShaderModule, VertexInputDescriptions* pVertexInputDescriptions);
	};
}