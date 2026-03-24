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
		ComputePipeline(const std::string& name, VkPipelineLayout vkPipelineLayout, const std::vector<char>& computeCode);
		~ComputePipeline();

	private: // Methods:
		void CreatePipeline(VkPipelineLayout vkPipelineLayout, const VkShaderModule& computeShaderModule);
	};
}