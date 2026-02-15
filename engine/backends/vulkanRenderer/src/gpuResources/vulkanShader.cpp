#include "vulkanShader.h"
#include "logger.h"
#include "vulkanMacros.h"
#include "vulkanPipeline.h"
#include <fstream>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Protected methods:
	// Constructor:
	Shader::Shader()
	{

	}



	// Public methods:
	// Destructor:
	Shader::~Shader()
	{
		for (int i = 0; i < m_vkDescriptorSetLayouts.size(); i++)
			vkDestroyDescriptorSetLayout(Context::GetVkDevice(), m_vkDescriptorSetLayout, nullptr);
		vkDestroyPipelineLayout(Context::GetVkDevice(), m_vkPipelineLayout, nullptr);
	}



	// Movable:
	Shader::Shader(Shader&& other) noexcept
	{
		m_name = std::move(other.m_name);
		m_vkDescriptorSetLayouts = std::move(other.m_vkDescriptorSetLayouts);
		m_vkPipelineLayout = other.m_vkPipelineLayout;
		other.m_vkPipelineLayout = VK_NULL_HANDLE;
		m_pPipelines = std::move(other.m_pPipelines);
		m_shaderReflection = other.m_shaderReflection;	// Ember::ToDO: use move semantics here aswell. ShaderReflection needs move semantics.
	}
	Shader& Shader::operator=(Shader&& other) noexcept
	{
		if (this != &other)
		{
			m_name = std::move(other.m_name);
			m_vkDescriptorSetLayouts = std::move(other.m_vkDescriptorSetLayouts);
			m_vkPipelineLayout = other.m_vkPipelineLayout;
			other.m_vkPipelineLayout = VK_NULL_HANDLE;
			m_pPipelines = std::move(other.m_pPipelines);
			m_shaderReflection = other.m_shaderReflection;
		}
		return *this;
	}



	// Create descriptor sets:
	void Shader::CreateDescriptorSetLayout()
	{
		m_shaderReflection.CreateDescriptorSets();

		const std::vector<emberSpirvReflect::DescriptorSet>& descriptorSets = m_shaderReflection.GetDescriptorSets();
		m_vkDescriptorSetLayouts.clear();
		m_vkDescriptorSetLayouts.resize(descriptorSets.size());

		for (size_t i = 0; i < descriptorSets.size(); i++)
		{
			const std::vector<VkDescriptorSetLayoutBinding>& layoutBindings = descriptorSets[i].GetVkDescriptorSetLayoutBindings();
			VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
			descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
			descriptorSetLayoutCreateInfo.pBindings = layoutBindings.empty() ? nullptr : layoutBindings.data();
			VKA(vkCreateDescriptorSetLayout(Context::GetVkDevice(), &descriptorSetLayoutCreateInfo, nullptr, &m_vkDescriptorSetLayouts[i]));
		}
	}



	// Getters:
	const std::string& Shader::GetName() const
	{
		return m_name;
	}
	const emberSpirvReflect::ShaderReflection& const Shader::GetShaderReflection() const
	{
		return m_shaderReflection;
	}
	const VkPipelineLayout& Material::GetVkPipelineLayout() const
	{
		return m_pipelineLayout;
	}



	// Debugging:
	void Shader::PrintShaderInfo() const
	{
		LOG_TRACE("ShaderInfo: {}\n{}", m_name, m_shaderReflection.ToString());
	}
}