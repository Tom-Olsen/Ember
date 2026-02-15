#pragma once
#include "vulkanPipeline.h"
#include <vector>



// Forward declarations:
typedef struct VkShaderModule_T* VkShaderModule;



namespace vulkanRendererBackend
{
	class ComputePipeline : public Pipeline
	{
	public: // Methods:
		ComputePipeline(const std::string& name, const std::vector<char>& computeCode, const std::vector<VkDescriptorSetLayout>& vkDescriptorSetLayouts);
		~ComputePipeline();

	private: // Methods:
		void CreatePipelineLayout(const std::vector<VkDescriptorSetLayout>& vkDescriptorSetLayouts);
		void CreatePipeline(const VkShaderModule& computeShaderModule);
	};
}