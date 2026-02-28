#include "vulkanShader.h"
#include "logger.h"
#include "vulkanDescriptorSetBinding.h"
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
		m_shaderReflection.CreateDescriptorSetReflections();

		const std::vector<emberSpirvReflect::DescriptorSetReflection>& descriptorSetReflections = m_shaderReflection.GetDescriptorSetReflections();
		m_vkDescriptorSetLayouts.clear();
		m_vkDescriptorSetLayouts.resize(DESCRIPTOR_SET_COUNT);

		// Sets GLOBAL_SET(0), SCENE_SET(1), and FRAME_SET(2) represent constant data and get bound to every shader:
		//m_vkDescriptorSetLayouts[0] = GlobalSetLayout();
		//m_vkDescriptorSetLayouts[1] = SceneSetLayout();
		//m_vkDescriptorSetLayouts[2] = FrameSetLayout();

		// Only SHADER_SET(3) and DRAW_SET(4) are retrieved via reflection:
		for (size_t i = 3; i < SET_COUNT; i++)
		{
			const std::vector<VkDescriptorSetLayoutBinding>& layoutBindings = descriptorSetReflections[i].GetVkDescriptorSetLayoutBindings();
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
	const VkPipelineLayout& Shader::GetVkPipelineLayout() const
	{
		return m_vkPipelineLayout;
	}
	const DescriptorSetBinding* Shader::GetDescriptorSetBinding() const
	{
		return m_pShaderDescriptorSetBinding.get();
	}



	// Debugging:
	void Shader::PrintShaderInfo() const
	{
		LOG_TRACE("ShaderInfo: {}\n{}", m_name, m_shaderReflection.ToString());
	}
}