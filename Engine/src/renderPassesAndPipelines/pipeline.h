#ifndef __INCLUDE_GUARD_pipeline_h__
#define __INCLUDE_GUARD_pipeline_h__
#include <vector>
#include <vulkan/vulkan.h>



namespace emberEngine
{
	/// <summary>
	/// Polymorphic parent class for all pipelines.
	/// </summary>
	class Pipeline
	{
	protected: // Members:
		VkDescriptorSetLayout m_descriptorSetLayout;
		VkPipelineLayout m_pipelineLayout;
		VkPipeline m_pipeline;

	public: // Methods:
		virtual ~Pipeline();
		const VkDescriptorSetLayout& GetVkDescriptorSetLayout() const;
		const VkPipelineLayout& GetVkPipelineLayout() const;
		const VkPipeline& GetVkPipeline() const;

	protected: // Methods:
		VkShaderModule CreateShaderModule(const std::vector<char>& code);
	};
}



#endif // __INCLUDE_GUARD_pipeline_h__