#ifndef __INCLUDE_GUARD_computePipeline_h__
#define __INCLUDE_GUARD_computePipeline_h__
#include "pipeline.h"
#include <vector>
#include <vulkan/vulkan.h>



namespace emberEngine
{
	// Forward declarations:
	struct VertexInputDescriptions;
	struct VulkanContext;



	/// <summary>
	/// Pipeline for compute shaders.
	/// </summary>
	class ComputePipeline : public Pipeline
	{
	public: // Methods:
		ComputePipeline(VulkanContext* pContext,
			const std::vector<char>& computeCode,
			const std::vector<VkDescriptorSetLayoutBinding>& vkDescriptorSetLayoutBindings);
		~ComputePipeline();

	private: // Methods:
		void CreatePipelineLayout(const std::vector<VkDescriptorSetLayoutBinding>& vkDescriptorSetLayoutBindings);
		void CreatePipeline(const VkShaderModule& computeShaderModule);
	};
}



#endif // __INCLUDE_GUARD_computePipeline_h__