#include "pipeline.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"



namespace emberEngine
{
	// Virtual Destructor:
	Pipeline::~Pipeline()
	{
		vkDestroyDescriptorSetLayout(VulkanContext::GetVkDevice(), m_descriptorSetLayout, nullptr);
		vkDestroyPipelineLayout(VulkanContext::GetVkDevice(), m_pipelineLayout, nullptr);
		vkDestroyPipeline(VulkanContext::GetVkDevice(), m_pipeline, nullptr);
	}



	// Public methods:
	const VkDescriptorSetLayout& Pipeline::GetVkDescriptorSetLayout() const
	{
		return m_descriptorSetLayout;
	}
	const VkPipelineLayout& Pipeline::GetVkPipelineLayout() const
	{
		return m_pipelineLayout;
	}
	const VkPipeline& Pipeline::GetVkPipeline() const
	{
		return m_pipeline;
	}



	// Protected methods:
	VkShaderModule Pipeline::CreateShaderModule(const std::vector<char>& code)
	{
		VkShaderModuleCreateInfo createInfo = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
		createInfo.codeSize = code.size();
		createInfo.pCode = (uint32_t*)(code.data());

		VkShaderModule shaderModule;
		VKA(vkCreateShaderModule(VulkanContext::GetVkDevice(), &createInfo, nullptr, &shaderModule));
		return shaderModule;
	}
}