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
	std::shared_ptr<MaterialShader> MaterialShader::CreateOutline(const std::string& name, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv)
	{
		std::shared_ptr<MaterialShader> pMaterialShader(new MaterialShader(name));

		// Load vertex shader:
		std::vector<char> vertexCode = emberSpirvReflect::ShaderReflection::ReadShaderCode(vertexSpv);
		pMaterialShader->m_shaderReflection.AddShaderStage(VK_SHADER_STAGE_VERTEX_BIT, vertexCode);

		// Load fragment shader:
		std::vector<char> fragmentCode = emberSpirvReflect::ShaderReflection::ReadShaderCode(fragmentSpv);
		pMaterialShader->m_shaderReflection.AddShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, fragmentCode);

		// Prepare pipeline data:
		pMaterialShader->CreateDescriptorSetLayout();
		const std::vector<emberSpirvReflect::VertexAttributeInfo>& vertexAttributeInfos = pMaterialShader->m_shaderReflection.GetVertexStageInfo()->vertexAttributes;
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
		pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(pMaterialShader->m_vkDescriptorSetLayouts.size());
		pipelineLayoutCreateInfo.pSetLayouts = pMaterialShader->m_vkDescriptorSetLayouts.data();
		pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
		pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;
		VKA(vkCreatePipelineLayout(Context::GetVkDevice(), &pipelineLayoutCreateInfo, nullptr, &pMaterialShader->m_vkPipelineLayout));
		NAME_VK_OBJECT(pMaterialShader->m_vkPipelineLayout, "PipelineLayout_Outline_" + pMaterialShader->m_name);

		// Create pipelines:
		pMaterialShader->m_pipelines.reserve(static_cast<size_t>(emberCommon::VertexMemoryLayout::count));
		for (uint32_t i = 0; i < static_cast<uint32_t>(emberCommon::VertexMemoryLayout::count); i++)
		{
			PipelineKey pipelineKey = PipelineKey::Create<RenderStage::outline>(static_cast<emberCommon::VertexMemoryLayout>(i));
			pMaterialShader->m_pipelines.emplace(
				pipelineKey,
				std::make_unique<OutlinePipeline>(
					pMaterialShader->m_name,
					pMaterialShader->m_vkPipelineLayout,
					vertexCode,
					fragmentCode,
					*vertexBindingVectors[i],
					*vertexAttributeVectors[i]));
		}

		return pMaterialShader;
	}
	std::shared_ptr<MaterialShader> MaterialShader::CreateForward(const std::string& name, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv)
	{
		std::shared_ptr<MaterialShader> pMaterialShader(new MaterialShader(name));

		// Load vertex shader:
		std::vector<char> vertexCode = emberSpirvReflect::ShaderReflection::ReadShaderCode(vertexSpv);
		pMaterialShader->m_shaderReflection.AddShaderStage(VK_SHADER_STAGE_VERTEX_BIT, vertexCode);

		// Load fragment shader:
		std::vector<char> fragmentCode = emberSpirvReflect::ShaderReflection::ReadShaderCode(fragmentSpv);
		pMaterialShader->m_shaderReflection.AddShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, fragmentCode);

		// Prepare pipeline data:
		pMaterialShader->CreateDescriptorSetLayout();
		const std::vector<emberSpirvReflect::VertexAttributeInfo>& vertexAttributeInfos = pMaterialShader->m_shaderReflection.GetVertexStageInfo()->vertexAttributes;
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
		pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(pMaterialShader->m_vkDescriptorSetLayouts.size());
		pipelineLayoutCreateInfo.pSetLayouts = pMaterialShader->m_vkDescriptorSetLayouts.data();
		pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
		pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;
		VKA(vkCreatePipelineLayout(Context::GetVkDevice(), &pipelineLayoutCreateInfo, nullptr, &pMaterialShader->m_vkPipelineLayout));
		NAME_VK_OBJECT(pMaterialShader->m_vkPipelineLayout, "PipelineLayout_Forward_" + pMaterialShader->m_name);

		// Create pipelines:
		pMaterialShader->m_pipelines.reserve(static_cast<size_t>(emberCommon::ForwardRenderMode::count) * static_cast<size_t>(emberCommon::VertexMemoryLayout::count));
		for (uint32_t j = 0; j < static_cast<uint32_t>(emberCommon::VertexMemoryLayout::count); j++)
			for (uint32_t i = 0; i < static_cast<uint32_t>(emberCommon::ForwardRenderMode::count); i++)
			{
				emberCommon::ForwardRenderMode pipelineRenderMode = static_cast<emberCommon::ForwardRenderMode>(i);
				emberCommon::VertexMemoryLayout vertexMemoryLayout = static_cast<emberCommon::VertexMemoryLayout>(j);
				PipelineKey forwardPipelineKey = PipelineKey::Create<RenderStage::forward>(pipelineRenderMode, vertexMemoryLayout);
				pMaterialShader->m_pipelines.emplace(
					forwardPipelineKey,
					std::make_unique<ForwardPipeline>(
						pMaterialShader->m_name,
						pMaterialShader->m_vkPipelineLayout,
						pipelineRenderMode,
						vertexCode,
						fragmentCode,
						*vertexBindingVectors[j],
						*vertexAttributeVectors[j]));
			}

		return pMaterialShader;
	}
	std::shared_ptr<MaterialShader> MaterialShader::CreateGizmo(const std::string& name, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv)
	{
		std::shared_ptr<MaterialShader> pMaterialShader(new MaterialShader(name));

		// Load vertex shader:
		std::vector<char> vertexCode = emberSpirvReflect::ShaderReflection::ReadShaderCode(vertexSpv);
		pMaterialShader->m_shaderReflection.AddShaderStage(VK_SHADER_STAGE_VERTEX_BIT, vertexCode);

		// Load fragment shader:
		std::vector<char> fragmentCode = emberSpirvReflect::ShaderReflection::ReadShaderCode(fragmentSpv);
		pMaterialShader->m_shaderReflection.AddShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, fragmentCode);

		// Prepare pipeline data:
		pMaterialShader->CreateDescriptorSetLayout();
		const std::vector<emberSpirvReflect::VertexAttributeInfo>& vertexAttributeInfos = pMaterialShader->m_shaderReflection.GetVertexStageInfo()->vertexAttributes;
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
		pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(pMaterialShader->m_vkDescriptorSetLayouts.size());
		pipelineLayoutCreateInfo.pSetLayouts = pMaterialShader->m_vkDescriptorSetLayouts.data();
		pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
		pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;
		VKA(vkCreatePipelineLayout(Context::GetVkDevice(), &pipelineLayoutCreateInfo, nullptr, &pMaterialShader->m_vkPipelineLayout));
		NAME_VK_OBJECT(pMaterialShader->m_vkPipelineLayout, "PipelineLayout_Gizmo_" + pMaterialShader->m_name);

		// Create pipelines:
		pMaterialShader->m_pipelines.reserve(static_cast<size_t>(emberCommon::GizmoRenderMode::count) * static_cast<size_t>(emberCommon::VertexMemoryLayout::count));
		for (uint32_t j = 0; j < static_cast<uint32_t>(emberCommon::VertexMemoryLayout::count); j++)
			for (uint32_t i = 0; i < static_cast<uint32_t>(emberCommon::GizmoRenderMode::count); i++)
			{
				emberCommon::GizmoRenderMode pipelineRenderMode = static_cast<emberCommon::GizmoRenderMode>(i);
				emberCommon::VertexMemoryLayout vertexMemoryLayout = static_cast<emberCommon::VertexMemoryLayout>(j);
				PipelineKey gizmoPipelineKey = PipelineKey::Create<RenderStage::gizmo>(pipelineRenderMode, vertexMemoryLayout);
				pMaterialShader->m_pipelines.emplace(
					gizmoPipelineKey,
					std::make_unique<GizmoPipeline>(
						pMaterialShader->m_name,
						pMaterialShader->m_vkPipelineLayout,
						pipelineRenderMode,
						vertexCode,
						fragmentCode,
						*vertexBindingVectors[j],
						*vertexAttributeVectors[j]));
			}

		return pMaterialShader;
	}
	std::shared_ptr<MaterialShader> MaterialShader::CreateShadow(const std::string& name, uint32_t shadowMapResolution, const std::filesystem::path& vertexSpv)
	{
		std::shared_ptr<MaterialShader> pMaterialShader(new MaterialShader(name));

		// Load vertex shader:
		std::vector<char> vertexCode = emberSpirvReflect::ShaderReflection::ReadShaderCode(vertexSpv);
		pMaterialShader->m_shaderReflection.AddShaderStage(VK_SHADER_STAGE_VERTEX_BIT, vertexCode);

		// Prepare pipeline data:
		pMaterialShader->CreateDescriptorSetLayout();
		const std::vector<emberSpirvReflect::VertexAttributeInfo>& vertexAttributeInfos = pMaterialShader->m_shaderReflection.GetVertexStageInfo()->vertexAttributes;
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
		pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(pMaterialShader->m_vkDescriptorSetLayouts.size());
		pipelineLayoutCreateInfo.pSetLayouts = pMaterialShader->m_vkDescriptorSetLayouts.data();
		pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
		pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;
		VKA(vkCreatePipelineLayout(Context::GetVkDevice(), &pipelineLayoutCreateInfo, nullptr, &pMaterialShader->m_vkPipelineLayout));
		NAME_VK_OBJECT(pMaterialShader->m_vkPipelineLayout, "PipelineLayout_Shadow_" + pMaterialShader->m_name);

		// Create pipelines:
		pMaterialShader->m_pipelines.reserve(static_cast<size_t>(emberCommon::VertexMemoryLayout::count));
		for (uint32_t i = 0; i < static_cast<uint32_t>(emberCommon::VertexMemoryLayout::count); i++)
		{
			PipelineKey pipelineKey = PipelineKey::Create<RenderStage::shadow>(static_cast<emberCommon::VertexMemoryLayout>(i));
			pMaterialShader->m_pipelines.emplace(
				pipelineKey,
				std::make_unique<ShadowPipeline>(
					pMaterialShader->m_name,
					pMaterialShader->m_vkPipelineLayout,
					shadowMapResolution,
					vertexCode,
					*vertexBindingVectors[i],
					*vertexAttributeVectors[i]));
		}

		return pMaterialShader;
	}
	std::shared_ptr<MaterialShader> MaterialShader::CreatePresent(const std::string& name, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv)
	{
		std::shared_ptr<MaterialShader> pMaterialShader(new MaterialShader(name));

		// Load vertex shader:
		std::vector<char> vertexCode = emberSpirvReflect::ShaderReflection::ReadShaderCode(vertexSpv);
		pMaterialShader->m_shaderReflection.AddShaderStage(VK_SHADER_STAGE_VERTEX_BIT, vertexCode);

		// Load fragment shader:
		std::vector<char> fragmentCode = emberSpirvReflect::ShaderReflection::ReadShaderCode(fragmentSpv);
		pMaterialShader->m_shaderReflection.AddShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, fragmentCode);

		// Prepare pipeline data:
		pMaterialShader->CreateDescriptorSetLayout();
		const std::vector<emberSpirvReflect::VertexAttributeInfo>& vertexAttributeInfos = pMaterialShader->m_shaderReflection.GetVertexStageInfo()->vertexAttributes;
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
		pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(pMaterialShader->m_vkDescriptorSetLayouts.size());
		pipelineLayoutCreateInfo.pSetLayouts = pMaterialShader->m_vkDescriptorSetLayouts.data();
		pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
		pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;
		VKA(vkCreatePipelineLayout(Context::GetVkDevice(), &pipelineLayoutCreateInfo, nullptr, &pMaterialShader->m_vkPipelineLayout));
		NAME_VK_OBJECT(pMaterialShader->m_vkPipelineLayout, "PipelineLayout_Present_" + pMaterialShader->m_name);

		// Create pipelines:
		pMaterialShader->m_pipelines.reserve(static_cast<size_t>(emberCommon::VertexMemoryLayout::count));
		for (uint32_t i = 0; i < static_cast<uint32_t>(emberCommon::VertexMemoryLayout::count); i++)
		{
			PipelineKey pipelineKey = PipelineKey::Create<RenderStage::present>(static_cast<emberCommon::VertexMemoryLayout>(i));
			pMaterialShader->m_pipelines.emplace(
				pipelineKey,
				std::make_unique<PresentPipeline>(
					pMaterialShader->m_name,
					pMaterialShader->m_vkPipelineLayout,
					vertexCode,
					fragmentCode,
					*vertexBindingVectors[i],
					*vertexAttributeVectors[i]));
		}

		return pMaterialShader;
	}
	MaterialShader::~MaterialShader()
	{

	}



	// Movable:
	MaterialShader::MaterialShader(MaterialShader&& other) noexcept = default;
	MaterialShader& MaterialShader::operator=(MaterialShader&& other) noexcept = default;



	// Getters:
	bool MaterialShader::HasPipeline(PipelineType pipelineType) const
	{
		for (const auto& [pipelineKey, _] : m_pipelines)
			if (pipelineKey.pipelineType == pipelineType)
				return true;
		return false;
	}



	// Private methods:
	// Constructor:
	MaterialShader::MaterialShader(const std::string& name)
		: Shader(name)
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