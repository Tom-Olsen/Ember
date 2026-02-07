#pragma once
#include "vulkanPipeline.h"
#include <vector>



// Forward declarations:
typedef struct VkShaderModule_T* VkShaderModule;



namespace vulkanRendererBackend
{
	/// <summary>
	/// Pipeline for compute shaders.
	/// </summary>
	class ComputePipeline : public Pipeline
	{
	public: // Methods:
		ComputePipeline(const std::string& name, const std::vector<char>& computeCode, const emberSpirvReflect::ShaderReflection& shaderReflection);
		~ComputePipeline();

	private: // Methods:
		void CreatePipelineLayout(std::vector<DescriptorSetLayoutBinding>& descriptorSetLayoutBindings);
		void CreatePipeline(const VkShaderModule& computeShaderModule);
	};
}