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
		// Constructor/Destructor:
		ComputePipeline(VkPipelineLayout vkPipelineLayout, const std::vector<char>& computeCode, const std::string& debugName);
		~ComputePipeline();

	private: // Methods:
		void CreatePipeline(VkPipelineLayout vkPipelineLayout, const VkShaderModule& computeShaderModule);
	};
}