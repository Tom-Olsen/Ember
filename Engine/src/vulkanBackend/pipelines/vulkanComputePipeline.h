#ifndef __INCLUDE_GUARD_vulkanComputePipeline_h__
#define __INCLUDE_GUARD_vulkanComputePipeline_h__
#include "vulkanPipeline.h"
#include <vector>
#include <vulkan/vulkan.h>



namespace emberEngine
{
	namespace vulkanBackend
	{
		// Forward declarations:
		struct VertexInputDescriptions;



		/// <summary>
		/// Pipeline for compute shaders.
		/// </summary>
		class ComputePipeline : public Pipeline
		{
		public: // Methods:
			ComputePipeline(const std::vector<char>& computeCode, const std::vector<VkDescriptorSetLayoutBinding>& vkDescriptorSetLayoutBindings);
			~ComputePipeline();

		private: // Methods:
			void CreatePipelineLayout(const std::vector<VkDescriptorSetLayoutBinding>& vkDescriptorSetLayoutBindings);
			void CreatePipeline(const VkShaderModule& computeShaderModule);
		};
	}
}



#endif // __INCLUDE_GUARD_vulkanComputePipeline_h__