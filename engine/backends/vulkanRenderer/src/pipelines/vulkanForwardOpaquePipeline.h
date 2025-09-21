#pragma once
#include "vulkanPipeline.h"
#include "vulkanDescriptorSetLayoutBinding.h"
#include <vector>



// Forward declarations:
typedef struct VkShaderModule_T* VkShaderModule;



namespace vulkanRendererBackend
{
	// Forward declarations:
	struct VertexInputDescriptions;



	/// <summary>
	/// Opaque pipeline associated with the ForwardRenderPass materials.
	/// </summary>
	class ForwardOpaquePipeline : public Pipeline
	{
	public: // Methods:
		ForwardOpaquePipeline(const std::vector<char>& vertexCode, const std::vector<char>& fragmentCode, std::vector<DescriptorSetLayoutBinding>& descriptorSetLayoutBindings, VertexInputDescriptions* pVertexInputDescriptions);
		~ForwardOpaquePipeline();

	private: // Methods:
		void CreatePipelineLayout(std::vector<DescriptorSetLayoutBinding>& descriptorSetLayoutBindings);
		void CreatePipeline(const VkShaderModule& vertexShaderModule, const VkShaderModule& fragmentShaderModule, VertexInputDescriptions* pVertexInputDescriptions);
	};
}