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
	/// Pipeline for compute shaders.
	/// </summary>
	class ComputePipeline : public Pipeline
	{
	public: // Methods:
		ComputePipeline(const std::string& name, const std::vector<char>& computeCode, std::vector<DescriptorSetLayoutBinding>& descriptorSetLayoutBindings);
		~ComputePipeline();

	private: // Methods:
		void CreatePipelineLayout(std::vector<DescriptorSetLayoutBinding>& descriptorSetLayoutBindings);
		void CreatePipeline(const VkShaderModule& computeShaderModule);
	};
}