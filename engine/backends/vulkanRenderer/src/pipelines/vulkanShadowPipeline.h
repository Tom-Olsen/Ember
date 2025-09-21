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
	/// Pipeline associated with the ShadowRenderPass materials.
	/// </summary>
	class ShadowPipeline : public Pipeline
	{
	public: // Methods:
		ShadowPipeline(const std::vector<char>& vertexCode, std::vector<DescriptorSetLayoutBinding>& descriptorSetLayoutBindings, VertexInputDescriptions* pVertexInputDescriptions);
		~ShadowPipeline();

	private: // Methods:
		void CreatePipelineLayout(std::vector<DescriptorSetLayoutBinding>& descriptorSetLayoutBindings);
		void CreatePipeline(const VkShaderModule& vertexShaderModule, VertexInputDescriptions* pVertexInputDescriptions);
	};
}