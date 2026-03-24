#include "vulkanMaterial.h"
#include "commonPipelineState.h"
#include "descriptorSetMacros.h"
#include "logger.h"
#include "vulkanDefaultPushConstant.h"
#include "vulkanDescriptorSetBinding.h"
#include "vulkanForwardPipeline.h"
#include "vulkanMacros.h"
#include "vulkanMesh.h"
#include "vulkanPipeline.h"
#include "vulkanPresentPipeline.h"
#include "vulkanShadowPipeline.h"
#include "vulkanVertexBuffer.h"
#include "vulkanVertexLayout.h"
#include "vmaBuffer.h"
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Private methods:
	// Constructors:
	Material::Material(const std::string& name) : Shader(name)
	{

	}



	// Public methods:
	// Factories/Destructor:
	Material Material::CreateForward(const std::string& name, uint32_t renderQueue, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv)
	{
		Material material = Material(name);
		material.m_renderQueue = renderQueue;
		material.m_type = emberCommon::MaterialType::forward;

		// Load vertex shader:
		std::vector<char> vertexCode = emberSpirvReflect::ShaderReflection::ReadShaderCode(vertexSpv);
		material.m_shaderReflection.AddShaderStage(VK_SHADER_STAGE_VERTEX_BIT, vertexCode);

		// Load fragment shader:
		std::vector<char> fragmentCode = emberSpirvReflect::ShaderReflection::ReadShaderCode(fragmentSpv);
		material.m_shaderReflection.AddShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, fragmentCode);

		// Prepare pipeline data:
		material.CreateDescriptorSetLayout();
		const std::vector<emberSpirvReflect::VertexAttributeInfo>& vertexAttributeInfos = material.m_shaderReflection.GetVertexStageInfo()->vertexAttributes;
		std::vector<VkVertexInputBindingDescription> vertexBindingsInterleaved = GetVertexBindingDescriptions<InterleavedVertexLayout>(vertexAttributeInfos);
		std::vector<VkVertexInputBindingDescription> vertexBindingsSeparate = GetVertexBindingDescriptions<SeparateVertexLayout>(vertexAttributeInfos);
		std::vector<VkVertexInputAttributeDescription> vertexAttributesInterleaved = GetVertexAttributeDescriptions<InterleavedVertexLayout>(vertexAttributeInfos);
		std::vector<VkVertexInputAttributeDescription> vertexAttributesSeparate = GetVertexAttributeDescriptions<SeparateVertexLayout>(vertexAttributeInfos);
		std::array<std::vector<VkVertexInputBindingDescription>*, 2> vertexBindingVectors = { &vertexBindingsInterleaved , &vertexBindingsSeparate };
		std::array<std::vector<VkVertexInputAttributeDescription>*, 2> vertexAttributeVectors = { &vertexAttributesInterleaved , &vertexAttributesSeparate };

		// Push constants layout:
		VkPushConstantRange pushConstantRange = {};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(DefaultPushConstant);

		// Pipeline layout:
		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
		pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(material.m_vkDescriptorSetLayouts.size());
		pipelineLayoutCreateInfo.pSetLayouts = material.m_vkDescriptorSetLayouts.data();
		pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
		pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;
		VKA(vkCreatePipelineLayout(Context::GetVkDevice(), &pipelineLayoutCreateInfo, nullptr, &material.m_vkPipelineLayout));
		NAME_VK_OBJECT(material.m_vkPipelineLayout, material.m_name + "ForwardPipelineLayout");

		// Create pipelines:
		material.m_pPipelines.reserve(static_cast<size_t>(static_cast<size_t>(emberCommon::RenderMode::count) * static_cast<size_t>(emberCommon::VertexMemoryLayout::count)));
		for (uint32_t j = 0; j < static_cast<uint32_t>(emberCommon::VertexMemoryLayout::count); j++)
			for (uint32_t i = 0; i < static_cast<uint32_t>(emberCommon::RenderMode::count); i++)
			{
				uint32_t index = i + j * emberCommon::RenderMode::count;
				material.m_pPipelines.emplace_back(
					material.m_name,
					material.m_vkPipelineLayout,
					static_cast<emberCommon::RenderMode>(i),
					vertexCode, 
					fragmentCode, 
					*vertexBindingVectors[j], 
					*vertexAttributeVectors[j]);
			}

		// Create shader descriptorSetBinding:
		material.m_pShaderDescriptorSetBinding = std::make_unique<DescriptorSetBinding>(static_cast<Shader*>&material, SHADER_SET_INDEX);
		return material;
	}
	Material Material::CreateShadow(const std::string& name, uint32_t shadowMapResolution)
	{
		Material material = Material(name);
		material.m_renderQueue = emberCommon::RenderQueue::shadow;
		material.m_type = emberCommon::MaterialType::shadow;

		// Load vertex shader:
		std::filesystem::path directoryPath = (std::filesystem::path(VULKAN_LIBRARY_ROOT_PATH) / "src" / "shaders").make_preferred();
		std::vector<char> vertexCode = emberSpirvReflect::ShaderReflection::ReadShaderCode(directoryPath / "shadow.vert.spv");
		material.m_shaderReflection.AddShaderStage(VK_SHADER_STAGE_VERTEX_BIT, vertexCode);

		// Prepare pipeline data:
		material.CreateDescriptorSetLayout();
		const std::vector<emberSpirvReflect::VertexAttributeInfo>& vertexAttributeInfos = material.m_shaderReflection.GetVertexStageInfo()->vertexAttributes;
		std::vector<VkVertexInputBindingDescription> vertexBindingsInterleaved = GetVertexBindingDescriptions<InterleavedVertexLayout>(vertexAttributeInfos);
		std::vector<VkVertexInputBindingDescription> vertexBindingsSeparate = GetVertexBindingDescriptions<SeparateVertexLayout>(vertexAttributeInfos);
		std::vector<VkVertexInputAttributeDescription> vertexAttributesInterleaved = GetVertexAttributeDescriptions<InterleavedVertexLayout>(vertexAttributeInfos);
		std::vector<VkVertexInputAttributeDescription> vertexAttributesSeparate = GetVertexAttributeDescriptions<SeparateVertexLayout>(vertexAttributeInfos);
		std::array<std::vector<VkVertexInputBindingDescription>*, 2> vertexBindingVectors = { &vertexBindingsInterleaved , &vertexBindingsSeparate };
		std::array<std::vector<VkVertexInputAttributeDescription>*, 2> vertexAttributeVectors = { &vertexAttributesInterleaved , &vertexAttributesSeparate };

		// Push constants layout:
		VkPushConstantRange pushConstantRange = {};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT; // shadow materials are vertex shader only.
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(DefaultPushConstant);

		// Pipeline layout:
		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
		pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(material.m_vkDescriptorSetLayouts.size());
		pipelineLayoutCreateInfo.pSetLayouts = material.m_vkDescriptorSetLayouts.data();
		pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
		pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;
		VKA(vkCreatePipelineLayout(Context::GetVkDevice(), &pipelineLayoutCreateInfo, nullptr, &material.m_vkPipelineLayout));
		NAME_VK_OBJECT(material.m_vkPipelineLayout, material.m_name + "ShadowPipelineLayout");

		// Create pipelines:
		material.m_pPipelines.reserve(static_cast<size_t>(emberCommon::VertexMemoryLayout::count));
		for (uint32_t i = 0; i < static_cast<uint32_t>(emberCommon::VertexMemoryLayout::count); i++)
		{
			material.m_pPipelines.emplace_back(
				material.m_name,
				material.m_vkPipelineLayout,
				shadowMapResolution,
				vertexCode,
				*vertexBindingVectors[i],
				*vertexAttributeVectors[i]);
		}

		// Create shader descriptorSetBinding:
		material.m_pShaderDescriptorSetBinding = std::make_unique<DescriptorSetBinding>(static_cast<Shader*>&material, SHADER_SET_INDEX);
		return material;
	}
	Material Material::CreatePresent(const std::string& name, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv)
	{
		Material material = Material(name);
		material.m_renderQueue = 0;
		material.m_type = emberCommon::MaterialType::present;

		// Load vertex shader:
		std::vector<char> vertexCode = emberSpirvReflect::ShaderReflection::ReadShaderCode(vertexSpv);
		material.m_shaderReflection.AddShaderStage(VK_SHADER_STAGE_VERTEX_BIT, vertexCode);

		// Load fragment shader:
		std::vector<char> fragmentCode = emberSpirvReflect::ShaderReflection::ReadShaderCode(fragmentSpv);
		material.m_shaderReflection.AddShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, fragmentCode);

		// Prepare pipeline data:
		material.CreateDescriptorSetLayout();
		const std::vector<emberSpirvReflect::VertexAttributeInfo>& vertexAttributeInfos = material.m_shaderReflection.GetVertexStageInfo()->vertexAttributes;
		std::vector<VkVertexInputBindingDescription> vertexBindingsInterleaved = GetVertexBindingDescriptions<InterleavedVertexLayout>(vertexAttributeInfos);
		std::vector<VkVertexInputBindingDescription> vertexBindingsSeparate = GetVertexBindingDescriptions<SeparateVertexLayout>(vertexAttributeInfos);
		std::vector<VkVertexInputAttributeDescription> vertexAttributesInterleaved = GetVertexAttributeDescriptions<InterleavedVertexLayout>(vertexAttributeInfos);
		std::vector<VkVertexInputAttributeDescription> vertexAttributesSeparate = GetVertexAttributeDescriptions<SeparateVertexLayout>(vertexAttributeInfos);
		std::array<std::vector<VkVertexInputBindingDescription>*, 2> vertexBindingVectors = { &vertexBindingsInterleaved , &vertexBindingsSeparate };
		std::array<std::vector<VkVertexInputAttributeDescription>*, 2> vertexAttributeVectors = { &vertexAttributesInterleaved , &vertexAttributesSeparate };

		// Push constants layout:
		VkPushConstantRange pushConstantRange = {};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(DefaultPushConstant);

		// Pipeline layout:
		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
		pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(material.m_vkDescriptorSetLayouts.size());
		pipelineLayoutCreateInfo.pSetLayouts = material.m_vkDescriptorSetLayouts.data();
		pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
		pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;
		VKA(vkCreatePipelineLayout(Context::GetVkDevice(), &pipelineLayoutCreateInfo, nullptr, &material.m_vkPipelineLayout));
		NAME_VK_OBJECT(material.m_vkPipelineLayout, material.m_name + "PresentPipelineLayout");

		// Create pipelines:
		material.m_pPipelines.reserve(static_cast<size_t>(emberCommon::VertexMemoryLayout::count));
		for (uint32_t i = 0; i < static_cast<uint32_t>(emberCommon::VertexMemoryLayout::count); i++)
		{
			material.m_pPipelines.emplace_back(
				material.m_name,
				material.m_vkPipelineLayout,
				vertexCode,
				fragmentCode,
				*vertexBindingVectors[i],
				*vertexAttributeVectors[i]);
		}

		// Create shader descriptorSetBinding:
		material.m_pShaderDescriptorSetBinding = std::make_unique<DescriptorSetBinding>(static_cast<Shader*>&material, SHADER_SET_INDEX);
		return material;
	}
	Material::~Material()
	{

	}



	// Movable:
	Material::Material(Material&& other) noexcept = default;
	Material& Material::operator=(Material&& other) noexcept = default;



	// Getters:
	const std::string& Material::GetName() const
	{
		return m_name;
	}
	emberCommon::MaterialType Material::GetType() const
	{
		return m_type;
	}
	uint32_t Material::GetRenderQueue() const
	{
		return m_renderQueue;
	}
	const VertexInputDescriptions* Material::GetVertexInputDescriptions() const
	{
		return m_pVertexInputDescriptions.get();
	}
	const Pipeline* Material::GetPipeline(const Mesh* pMesh) const
	{
		Pipeline* pPipeline = m_pPipelines[static_cast<size_t>(pMesh->GetVertexMemoryLayout())].get();
		assert(pPipeline && "Material::GetPipeline: Pipeline not supported for this vertex layout");
		return pPipeline;
	}



	// Debugging:
	void Material::Print() const
	{
		PrintShaderInfo();
	}
}