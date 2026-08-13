#include "vulkanMaterialShader.h"
#include "vulkanContext.h"
#include "vulkanDefaultPushConstant.h"
#include "vulkanForwardPipeline.h"
#include "vulkanGizmoPipeline.h"
#include "vulkanMacros.h"
#include "vulkanMesh.h"
#include "vulkanOutlinePipeline.h"
#include "vulkanPipeline.h"
#include "vulkanPresentPipeline.h"
#include "vulkanShadowPipeline.h"
#include "vulkanVertexLayout.h"
#include <array>
#include <stdexcept>



namespace vulkanRendererBackend
{
	// Public methods:
	// Factories/Destructor:
	MaterialShader MaterialShader::CreateOutline(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& debugName)
	{
		MaterialShader materialShader(emberCommon::MaterialType::outline, debugName);

		// Load vertex shader:
		std::vector<char> vertexCode = emberSpirvReflect::ShaderReflection::ReadShaderCode(vertexSpv);
		materialShader.m_shaderReflection.AddShaderStage(VK_SHADER_STAGE_VERTEX_BIT, vertexCode);

		// Load fragment shader:
		std::vector<char> fragmentCode = emberSpirvReflect::ShaderReflection::ReadShaderCode(fragmentSpv);
		materialShader.m_shaderReflection.AddShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, fragmentCode);

		// Prepare pipeline data:
		materialShader.CreateDescriptorSetLayout();
		const std::vector<emberSpirvReflect::VertexAttributeInfo>& vertexAttributeInfos = materialShader.m_shaderReflection.GetVertexStageInfo()->vertexAttributes;
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
		pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(materialShader.m_vkDescriptorSetLayouts.size());
		pipelineLayoutCreateInfo.pSetLayouts = materialShader.m_vkDescriptorSetLayouts.data();
		pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
		pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;
		VKA(vkCreatePipelineLayout(Context::GetVkDevice(), &pipelineLayoutCreateInfo, nullptr, &materialShader.m_vkPipelineLayout));
		NAME_VK_OBJECT(materialShader.m_vkPipelineLayout, "PipelineLayout_Outline_" + debugName);

		// Create pipelines:
		materialShader.m_pipelines.reserve(static_cast<size_t>(emberCommon::VertexMemoryLayout::count));
		for (uint32_t i = 0; i < static_cast<uint32_t>(emberCommon::VertexMemoryLayout::count); i++)
		{
			PipelineKey pipelineKey = PipelineKey::Create<RenderStage::outline>(static_cast<emberCommon::VertexMemoryLayout>(i));
			materialShader.m_pipelines.emplace(
				pipelineKey,
				std::make_unique<OutlinePipeline>(
					materialShader.m_vkPipelineLayout,
					vertexCode,
					fragmentCode,
					*vertexBindingVectors[i],
					*vertexAttributeVectors[i],
					debugName));
		}

		return materialShader;
	}
	MaterialShader MaterialShader::CreateForward(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& debugName)
	{
		MaterialShader materialShader(emberCommon::MaterialType::forward, debugName);

		// Load vertex shader:
		std::vector<char> vertexCode = emberSpirvReflect::ShaderReflection::ReadShaderCode(vertexSpv);
		materialShader.m_shaderReflection.AddShaderStage(VK_SHADER_STAGE_VERTEX_BIT, vertexCode);

		// Load fragment shader:
		std::vector<char> fragmentCode = emberSpirvReflect::ShaderReflection::ReadShaderCode(fragmentSpv);
		materialShader.m_shaderReflection.AddShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, fragmentCode);

		// Prepare pipeline data:
		materialShader.CreateDescriptorSetLayout();
		const std::vector<emberSpirvReflect::VertexAttributeInfo>& vertexAttributeInfos = materialShader.m_shaderReflection.GetVertexStageInfo()->vertexAttributes;
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
		pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(materialShader.m_vkDescriptorSetLayouts.size());
		pipelineLayoutCreateInfo.pSetLayouts = materialShader.m_vkDescriptorSetLayouts.data();
		pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
		pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;
		VKA(vkCreatePipelineLayout(Context::GetVkDevice(), &pipelineLayoutCreateInfo, nullptr, &materialShader.m_vkPipelineLayout));
		NAME_VK_OBJECT(materialShader.m_vkPipelineLayout, "PipelineLayout_Forward_" + debugName);

		// Create pipelines:
		materialShader.m_pipelines.reserve(static_cast<size_t>(emberCommon::ForwardRenderMode::count) * static_cast<size_t>(emberCommon::VertexMemoryLayout::count));
		for (uint32_t j = 0; j < static_cast<uint32_t>(emberCommon::VertexMemoryLayout::count); j++)
			for (uint32_t i = 0; i < static_cast<uint32_t>(emberCommon::ForwardRenderMode::count); i++)
			{
				emberCommon::ForwardRenderMode pipelineRenderMode = static_cast<emberCommon::ForwardRenderMode>(i);
				emberCommon::VertexMemoryLayout vertexMemoryLayout = static_cast<emberCommon::VertexMemoryLayout>(j);
				PipelineKey forwardPipelineKey = PipelineKey::Create<RenderStage::forward>(pipelineRenderMode, vertexMemoryLayout);
				materialShader.m_pipelines.emplace(
					forwardPipelineKey,
					std::make_unique<ForwardPipeline>(
						materialShader.m_vkPipelineLayout,
						pipelineRenderMode,
						vertexCode,
						fragmentCode,
						*vertexBindingVectors[j],
						*vertexAttributeVectors[j],
						debugName));
			}

		return materialShader;
	}
	MaterialShader MaterialShader::CreateGizmo(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& debugName)
	{
		MaterialShader materialShader(emberCommon::MaterialType::gizmo, debugName);

		// Load vertex shader:
		std::vector<char> vertexCode = emberSpirvReflect::ShaderReflection::ReadShaderCode(vertexSpv);
		materialShader.m_shaderReflection.AddShaderStage(VK_SHADER_STAGE_VERTEX_BIT, vertexCode);

		// Load fragment shader:
		std::vector<char> fragmentCode = emberSpirvReflect::ShaderReflection::ReadShaderCode(fragmentSpv);
		materialShader.m_shaderReflection.AddShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, fragmentCode);

		// Prepare pipeline data:
		materialShader.CreateDescriptorSetLayout();
		const std::vector<emberSpirvReflect::VertexAttributeInfo>& vertexAttributeInfos = materialShader.m_shaderReflection.GetVertexStageInfo()->vertexAttributes;
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
		pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(materialShader.m_vkDescriptorSetLayouts.size());
		pipelineLayoutCreateInfo.pSetLayouts = materialShader.m_vkDescriptorSetLayouts.data();
		pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
		pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;
		VKA(vkCreatePipelineLayout(Context::GetVkDevice(), &pipelineLayoutCreateInfo, nullptr, &materialShader.m_vkPipelineLayout));
		NAME_VK_OBJECT(materialShader.m_vkPipelineLayout, "PipelineLayout_Gizmo_" + debugName);

		// Create pipelines:
		materialShader.m_pipelines.reserve(static_cast<size_t>(emberCommon::GizmoRenderMode::count) * static_cast<size_t>(emberCommon::VertexMemoryLayout::count));
		for (uint32_t j = 0; j < static_cast<uint32_t>(emberCommon::VertexMemoryLayout::count); j++)
			for (uint32_t i = 0; i < static_cast<uint32_t>(emberCommon::GizmoRenderMode::count); i++)
			{
				emberCommon::GizmoRenderMode pipelineRenderMode = static_cast<emberCommon::GizmoRenderMode>(i);
				emberCommon::VertexMemoryLayout vertexMemoryLayout = static_cast<emberCommon::VertexMemoryLayout>(j);
				PipelineKey gizmoPipelineKey = PipelineKey::Create<RenderStage::gizmo>(pipelineRenderMode, vertexMemoryLayout);
				materialShader.m_pipelines.emplace(
					gizmoPipelineKey,
					std::make_unique<GizmoPipeline>(
						materialShader.m_vkPipelineLayout,
						pipelineRenderMode,
						vertexCode,
						fragmentCode,
						*vertexBindingVectors[j],
						*vertexAttributeVectors[j],
						debugName));
			}

		return materialShader;
	}
	MaterialShader MaterialShader::CreateShadow(uint32_t shadowMapResolution, const std::filesystem::path& vertexSpv, const std::string& debugName)
	{
		MaterialShader materialShader(emberCommon::MaterialType::shadow, debugName);

		// Load vertex shader:
		std::vector<char> vertexCode = emberSpirvReflect::ShaderReflection::ReadShaderCode(vertexSpv);
		materialShader.m_shaderReflection.AddShaderStage(VK_SHADER_STAGE_VERTEX_BIT, vertexCode);

		// Prepare pipeline data:
		materialShader.CreateDescriptorSetLayout();
		const std::vector<emberSpirvReflect::VertexAttributeInfo>& vertexAttributeInfos = materialShader.m_shaderReflection.GetVertexStageInfo()->vertexAttributes;
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
		pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(materialShader.m_vkDescriptorSetLayouts.size());
		pipelineLayoutCreateInfo.pSetLayouts = materialShader.m_vkDescriptorSetLayouts.data();
		pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
		pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;
		VKA(vkCreatePipelineLayout(Context::GetVkDevice(), &pipelineLayoutCreateInfo, nullptr, &materialShader.m_vkPipelineLayout));
		NAME_VK_OBJECT(materialShader.m_vkPipelineLayout, "PipelineLayout_Shadow_" + debugName);

		// Create pipelines:
		materialShader.m_pipelines.reserve(static_cast<size_t>(emberCommon::VertexMemoryLayout::count));
		for (uint32_t i = 0; i < static_cast<uint32_t>(emberCommon::VertexMemoryLayout::count); i++)
		{
			PipelineKey pipelineKey = PipelineKey::Create<RenderStage::shadow>(static_cast<emberCommon::VertexMemoryLayout>(i));
			materialShader.m_pipelines.emplace(
				pipelineKey,
				std::make_unique<ShadowPipeline>(
					materialShader.m_vkPipelineLayout,
					shadowMapResolution,
					vertexCode,
					*vertexBindingVectors[i],
					*vertexAttributeVectors[i],
					debugName));
		}

		return materialShader;
	}
	MaterialShader MaterialShader::CreatePresent(const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, const std::string& debugName)
	{
		MaterialShader materialShader(emberCommon::MaterialType::present, debugName);

		// Load vertex shader:
		std::vector<char> vertexCode = emberSpirvReflect::ShaderReflection::ReadShaderCode(vertexSpv);
		materialShader.m_shaderReflection.AddShaderStage(VK_SHADER_STAGE_VERTEX_BIT, vertexCode);

		// Load fragment shader:
		std::vector<char> fragmentCode = emberSpirvReflect::ShaderReflection::ReadShaderCode(fragmentSpv);
		materialShader.m_shaderReflection.AddShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, fragmentCode);

		// Prepare pipeline data:
		materialShader.CreateDescriptorSetLayout();
		const std::vector<emberSpirvReflect::VertexAttributeInfo>& vertexAttributeInfos = materialShader.m_shaderReflection.GetVertexStageInfo()->vertexAttributes;
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
		pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(materialShader.m_vkDescriptorSetLayouts.size());
		pipelineLayoutCreateInfo.pSetLayouts = materialShader.m_vkDescriptorSetLayouts.data();
		pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
		pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;
		VKA(vkCreatePipelineLayout(Context::GetVkDevice(), &pipelineLayoutCreateInfo, nullptr, &materialShader.m_vkPipelineLayout));
		NAME_VK_OBJECT(materialShader.m_vkPipelineLayout, "PipelineLayout_Present_" + debugName);

		// Create pipelines:
		materialShader.m_pipelines.reserve(static_cast<size_t>(emberCommon::VertexMemoryLayout::count));
		for (uint32_t i = 0; i < static_cast<uint32_t>(emberCommon::VertexMemoryLayout::count); i++)
		{
			PipelineKey pipelineKey = PipelineKey::Create<RenderStage::present>(static_cast<emberCommon::VertexMemoryLayout>(i));
			materialShader.m_pipelines.emplace(
				pipelineKey,
				std::make_unique<PresentPipeline>(
					materialShader.m_vkPipelineLayout,
					vertexCode,
					fragmentCode,
					*vertexBindingVectors[i],
					*vertexAttributeVectors[i],
					debugName));
		}

		return materialShader;
	}
	MaterialShader::~MaterialShader()
	{

	}



	// Movable:
	MaterialShader::MaterialShader(MaterialShader&& other) noexcept = default;
	MaterialShader& MaterialShader::operator=(MaterialShader&& other) noexcept = default;



	// Getters:
	emberCommon::MaterialType MaterialShader::GetMaterialType() const
	{
		return m_materialType;
	}
	bool MaterialShader::HasPipeline(PipelineType pipelineType) const
	{
		for (const auto& [pipelineKey, _] : m_pipelines)
			if (pipelineKey.pipelineType == pipelineType)
				return true;
		return false;
	}



	// Private methods:
	// Constructor:
	MaterialShader::MaterialShader(emberCommon::MaterialType materialType, const std::string& debugName)
		: Shader(debugName)
		, m_materialType(materialType)
	{

	}



	// Pipeline lookup:
	const Pipeline* MaterialShader::GetPipelineByStage(PipelineType pipelineType, const Mesh* pMesh, uint32_t renderModeIndex) const
	{
		if (!HasPipeline(pipelineType))
			throw std::runtime_error("MaterialShader::GetPipelineByStage(...) failed. Requested pipeline type is not supported by this material shader.");

		PipelineKey pipelineKey(pipelineType, renderModeIndex, pMesh->GetVertexMemoryLayout());
		auto it = m_pipelines.find(pipelineKey);
		if (it == m_pipelines.end())
			throw std::runtime_error("MaterialShader::GetPipelineByStage(...) failed. Render mode is not supported by this material shader.");
		return it->second.get();
	}
}