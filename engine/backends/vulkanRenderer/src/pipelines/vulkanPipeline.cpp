#include "vulkanPipeline.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Virtual Destructor:
	Pipeline::~Pipeline()
	{
		Cleanup();
	}



	// Move semantics:
	Pipeline::Pipeline(Pipeline&& other) noexcept
	{
		MoveFrom(other);
	}
	Pipeline& Pipeline::operator=(Pipeline&& other) noexcept
	{
		if (this != &other)
		{
			Cleanup();
			MoveFrom(other);
		}
		return *this;
	}

	

	// Public methods:
	const VkDescriptorSetLayout& Pipeline::GetVkDescriptorSetLayout(size_t set) const
	{
		return m_descriptorSetLayouts[set];
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
		VKA(vkCreateShaderModule(Context::GetVkDevice(), &createInfo, nullptr, &shaderModule));
		return shaderModule;
	}



	// Private methods:
	void Pipeline::Cleanup()
	{
		vkDestroyPipeline(Context::GetVkDevice(), m_pipeline, nullptr);
		vkDestroyPipelineLayout(Context::GetVkDevice(), m_pipelineLayout, nullptr);
		for (int i = 0; i < m_descriptorSetLayouts.size(); i++)
			vkDestroyDescriptorSetLayout(Context::GetVkDevice(), m_descriptorSetLayouts[i], nullptr);
	}
	void Pipeline::MoveFrom(Pipeline& other) noexcept
	{
		m_descriptorSetLayouts = other.m_descriptorSetLayouts;
		m_pipelineLayout = other.m_pipelineLayout;
		m_pipeline = other.m_pipeline;

		other.m_descriptorSetLayouts.clear();
		other.m_pipelineLayout = VK_NULL_HANDLE;
		other.m_pipeline = VK_NULL_HANDLE;
	}
}