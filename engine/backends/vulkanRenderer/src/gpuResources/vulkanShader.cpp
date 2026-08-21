#include "vulkanShader.h"
#include "descriptorSetMacros.h"
#include "logger.h"
#include "vulkanContext.h"
#include "vulkanFrameDescriptorSetLayout.h"
#include "vulkanGlobalDescriptorSetLayout.h"
#include "vulkanMacros.h"
#include "vulkanPoolManager.h"
#include "vulkanSceneDescriptorSetLayout.h"
#include <fstream>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Protected methods:
	// Constructor:
	Shader::Shader(const std::string& debugName)
		: m_debugName(debugName)
		, m_shaderReflection(DESCRIPTOR_SET_COUNT)
	{

	}



	// Public methods:
	// Destructor:
	Shader::~Shader()
	{
		PoolManager::RemoveShader(this);

		// Sets GLOBAL_SET_INDEX(0)/SCENE_SET_INDEX(1)/FRAME_SET_INDEX(2) are shared static layouts.
		// Sets SHADER_SET_INDEX(3) and CALL_SET_INDEX(4) are reflected per shader and owned here.
		// DescriptorSetBinding objects own descriptor sets/data, but not the descriptor set layouts.
		for (size_t i = SHADER_SET_INDEX; i < m_vkDescriptorSetLayouts.size(); i++)
			vkDestroyDescriptorSetLayout(Context::GetVkDevice(), m_vkDescriptorSetLayouts[i], nullptr);
		vkDestroyPipelineLayout(Context::GetVkDevice(), m_vkPipelineLayout, nullptr);
	}



	// Movable:
	Shader::Shader(Shader&& other) noexcept
		: m_debugName(std::move(other.m_debugName))
		, m_shaderReflection(std::move(other.m_shaderReflection))
		, m_vkDescriptorSetLayouts(std::move(other.m_vkDescriptorSetLayouts))
		, m_vkPipelineLayout(other.m_vkPipelineLayout)
	{
		other.m_vkPipelineLayout = VK_NULL_HANDLE;
	}
	Shader& Shader::operator=(Shader&& other) noexcept
	{
		if (this != &other)
		{
			m_debugName = std::move(other.m_debugName);
			m_shaderReflection = std::move(other.m_shaderReflection);
			m_vkDescriptorSetLayouts = std::move(other.m_vkDescriptorSetLayouts);
			m_vkPipelineLayout = other.m_vkPipelineLayout;
			other.m_vkPipelineLayout = VK_NULL_HANDLE;
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

		// Only SHADER_SET(3) and CALL_SET(4) are dynamic and come from reflection:
		for (size_t i = SHADER_SET_INDEX; i < DESCRIPTOR_SET_COUNT; i++)
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
			NAME_VK_OBJECT(m_vkDescriptorSetLayouts[i], "DescriptorSetLayout" + std::to_string(i) + "_" + m_debugName);
		}
	}



	// Getters:
	const emberSpirvReflect::ShaderReflection& Shader::GetShaderReflection() const
	{
		return m_shaderReflection;
	}
	const VkPipelineLayout& Shader::GetVkPipelineLayout() const
	{
		return m_vkPipelineLayout;
	}
	const std::vector<VkDescriptorSetLayout>& Shader::GetVkDescriptorSetLayout() const
	{
		return m_vkDescriptorSetLayouts;
	}
	const std::string& Shader::GetDebugName() const
	{
		return m_debugName;
	}



	// Debugging:
	void Shader::PrintShaderInfo() const
	{
		LOG_TRACE("ShaderInfo: {}\n{}", GetDebugName(), m_shaderReflection.ToString());
	}
}