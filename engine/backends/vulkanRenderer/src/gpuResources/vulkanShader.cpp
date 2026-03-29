#include "vulkanShader.h"
#include "descriptorSetMacros.h"
#include "logger.h"
#include "vulkanDescriptorSetBinding.h"
#include "vulkanFrameDescriptorSetLayout.h"
#include "vulkanGlobalDescriptorSetLayout.h"
#include "vulkanMacros.h"
#include "vulkanPipeline.h"
#include "vulkanSceneDescriptorSetLayout.h"
#include <fstream>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Protected methods:
	// Constructor:
	Shader::Shader(const std::string& name)
		: m_name(name), m_shaderReflection(DESCRIPTOR_SET_COUNT)
	{

	}



	// Public methods:
	// Destructor:
	Shader::~Shader()
	{
		for (int i = 0; i < m_vkDescriptorSetLayouts.size(); i++)
			vkDestroyDescriptorSetLayout(Context::GetVkDevice(), m_vkDescriptorSetLayouts[i], nullptr);
		vkDestroyPipelineLayout(Context::GetVkDevice(), m_vkPipelineLayout, nullptr);
	}



	// Movable:
	Shader::Shader(Shader&& other) noexcept
        : m_name(std::move(other.m_name))
        , m_shaderReflection(std::move(other.m_shaderReflection))
        , m_vkDescriptorSetLayouts(std::move(other.m_vkDescriptorSetLayouts))
        , m_vkPipelineLayout(other.m_vkPipelineLayout)
        , m_pPipelines(std::move(other.m_pPipelines))
	{
        
	}
	Shader& Shader::operator=(Shader&& other) noexcept
	{
		if (this != &other)
		{
			m_name = std::move(other.m_name);
			m_shaderReflection = std::move(other.m_shaderReflection);
			m_vkDescriptorSetLayouts = std::move(other.m_vkDescriptorSetLayouts);
			m_vkPipelineLayout = other.m_vkPipelineLayout;
			other.m_vkPipelineLayout = VK_NULL_HANDLE;
			m_pPipelines = std::move(other.m_pPipelines);
		}
		return *this;
	}



	// Create descriptor sets:
	void Shader::CreateDescriptorSetLayout()
	{
		m_shaderReflection.CreateDescriptorSetReflections();

		m_vkDescriptorSetLayouts.clear();
		m_vkDescriptorSetLayouts.resize(DESCRIPTOR_SET_COUNT);

		// Sets GLOBAL_SET(0), SCENE_SET(1), and FRAME_SET(2) represent constant data and get bound to every shader:
		m_vkDescriptorSetLayouts[0] = GlobalDescriptorSetLayout::GetVkDescriptorSetLayout();
		m_vkDescriptorSetLayouts[1] = SceneDescriptorSetLayout::GetVkDescriptorSetLayout();
		m_vkDescriptorSetLayouts[2] = FrameDescriptorSetLayout::GetVkDescriptorSetLayout();

		// Only SHADER_SET(3) and DRAW_SET(4) are dynamic and come from reflection:
		for (size_t i = 3; i < DESCRIPTOR_SET_COUNT; i++)
		{
			const std::vector<emberSpirvReflect::DescriptorReflection>& descriptors = m_shaderReflection.GetDescriptorSetReflection(i).GetDescriptorReflections();

			std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
			layoutBindings.reserve(descriptors.size());

			for (const emberSpirvReflect::DescriptorReflection& descriptor : descriptors)
			{
				VkDescriptorSetLayoutBinding binding = {};
				binding.binding = descriptor.GetBinding();
				binding.descriptorCount = descriptor.GetDescriptorCount();
				binding.descriptorType = static_cast<VkDescriptorType>(descriptor.GetDescriptorType());
				binding.stageFlags = static_cast<VkShaderStageFlags>(descriptor.GetShaderStage());
				binding.pImmutableSamplers = nullptr; // not allowed.
				layoutBindings.push_back(binding);
			}

			// Sort by binding index to match vulkan spec expectations:
			std::sort(layoutBindings.begin(), layoutBindings.end(),
				[](const VkDescriptorSetLayoutBinding& a, const VkDescriptorSetLayoutBinding& b)
			{ return a.binding < b.binding; });

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
	const emberSpirvReflect::ShaderReflection& Shader::GetShaderReflection() const
	{
		return m_shaderReflection;
	}
	const VkPipelineLayout& Shader::GetVkPipelineLayout() const
	{
		return m_vkPipelineLayout;
	}
	DescriptorSetBinding* Shader::GetDescriptorSetBinding() const
	{
		return m_pShaderDescriptorSetBinding.get();
	}
	const std::vector<VkDescriptorSetLayout>& Shader::GetVkDescriptorSetLayout() const
	{
		return m_vkDescriptorSetLayouts;
	}



	// Debugging:
	void Shader::PrintShaderInfo() const
	{
		LOG_TRACE("ShaderInfo: {}\n{}", m_name, m_shaderReflection.ToString());
	}
}