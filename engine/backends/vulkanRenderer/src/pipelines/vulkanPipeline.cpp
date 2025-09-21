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
		VKA(vkCreateShaderModule(Context::GetVkDevice(), &createInfo, nullptr, &shaderModule));
		return shaderModule;
	}



	// Private methods:
	void Pipeline::Cleanup()
	{
		vkDestroyDescriptorSetLayout(Context::GetVkDevice(), m_descriptorSetLayout, nullptr);
		vkDestroyPipelineLayout(Context::GetVkDevice(), m_pipelineLayout, nullptr);
		vkDestroyPipeline(Context::GetVkDevice(), m_pipeline, nullptr);
	}
	void Pipeline::MoveFrom(Pipeline& other) noexcept
	{
		m_descriptorSetLayout = other.m_descriptorSetLayout;
		m_pipelineLayout = other.m_pipelineLayout;
		m_pipeline = other.m_pipeline;

		other.m_descriptorSetLayout = VK_NULL_HANDLE;
		other.m_pipelineLayout = VK_NULL_HANDLE;
		other.m_pipeline = VK_NULL_HANDLE;
	}
}