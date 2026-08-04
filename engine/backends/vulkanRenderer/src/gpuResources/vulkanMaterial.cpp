#include "vulkanMaterial.h"
#include "descriptorSetMacros.h"
#include "logger.h"
#include "vmaBuffer.h"
#include "vulkanDefaultPushConstant.h"
#include "vulkanDescriptorSetBinding.h"
#include "vulkanForwardPipeline.h"
#include "vulkanGizmoPipeline.h"
#include "vulkanMacros.h"
#include "vulkanMesh.h"
#include "vulkanOutlinePipeline.h"
#include "vulkanPipeline.h"
#include "vulkanPresentPipeline.h"
#include "vulkanShadowPipeline.h"
#include "vulkanVertexBuffer.h"
#include "vulkanVertexLayout.h"
#include <array>
#include <stdexcept>



namespace vulkanRendererBackend
{
	
	// Public methods:
	// Factories/Destructor:
    Material Material::CreateOutline(const std::string& name, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv)
    {
		Material material = Material(name);
		material.m_materialType = emberCommon::MaterialType::outline;
		material.m_pOutlineRenderState = std::make_unique<emberCommon::OutlineRenderState>();

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
		NAME_VK_OBJECT(material.m_vkPipelineLayout, "PipelineLayout_Outline_" + material.m_name);

		// Create pipelines:
		material.m_pipelines.reserve(static_cast<size_t>(emberCommon::VertexMemoryLayout::count));
		for (uint32_t i = 0; i < static_cast<uint32_t>(emberCommon::VertexMemoryLayout::count); i++)
		{
			PipelineKey pipelineKey = PipelineKey::Create<RenderStage::outline>(static_cast<emberCommon::VertexMemoryLayout>(i));
			material.m_pipelines.emplace(
				pipelineKey,
				std::make_unique<OutlinePipeline>(
					material.m_name,
					material.m_vkPipelineLayout,
					vertexCode,
					fragmentCode,
					*vertexBindingVectors[i],
					*vertexAttributeVectors[i]));
		}

		// Create shader descriptorSetBinding:
		material.m_pShaderDescriptorSetBinding = std::make_unique<DescriptorSetBinding>(static_cast<Shader*>(&material), SHADER_SET_INDEX);
		return material;
    }
	Material Material::CreateForward(const std::string& name, emberCommon::ForwardRenderMode renderMode, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv)
	{
		Material material = Material(name);
		material.m_materialType = emberCommon::MaterialType::forward;
		material.m_pForwardRenderState = std::make_unique<emberCommon::ForwardRenderState>(emberCommon::ForwardRenderState::ForwardDefault(renderMode));

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
		NAME_VK_OBJECT(material.m_vkPipelineLayout, "PipelineLayout_Forward_" + material.m_name);

		// Create pipelines:
		material.m_pipelines.reserve(static_cast<size_t>(emberCommon::ForwardRenderMode::count) * static_cast<size_t>(emberCommon::VertexMemoryLayout::count));
		for (uint32_t j = 0; j < static_cast<uint32_t>(emberCommon::VertexMemoryLayout::count); j++)
			for (uint32_t i = 0; i < static_cast<uint32_t>(emberCommon::ForwardRenderMode::count); i++)
			{
				emberCommon::ForwardRenderMode pipelineRenderMode = static_cast<emberCommon::ForwardRenderMode>(i);
				emberCommon::VertexMemoryLayout vertexMemoryLayout = static_cast<emberCommon::VertexMemoryLayout>(j);
				PipelineKey forwardPipelineKey = PipelineKey::Create<RenderStage::forward>(pipelineRenderMode, vertexMemoryLayout);
				material.m_pipelines.emplace(
					forwardPipelineKey,
					std::make_unique<ForwardPipeline>(
						material.m_name,
						material.m_vkPipelineLayout,
						pipelineRenderMode,
						vertexCode,
						fragmentCode,
						*vertexBindingVectors[j],
						*vertexAttributeVectors[j]));
			}

		// Create shader descriptorSetBinding:
		material.m_pShaderDescriptorSetBinding = std::make_unique<DescriptorSetBinding>(static_cast<Shader*>(&material), SHADER_SET_INDEX);
		return material;
	}
	Material Material::CreateGizmo(const std::string& name, emberCommon::GizmoRenderMode renderMode, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv)
	{
		Material material = Material(name);
		material.m_materialType = emberCommon::MaterialType::gizmo;
		material.m_pGizmoRenderState = std::make_unique<emberCommon::GizmoRenderState>(emberCommon::GizmoRenderState::GizmoDefault(renderMode));

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
		NAME_VK_OBJECT(material.m_vkPipelineLayout, "PipelineLayout_Gizmo_" + material.m_name);

		// Create pipelines:
		material.m_pipelines.reserve(static_cast<size_t>(emberCommon::GizmoRenderMode::count) * static_cast<size_t>(emberCommon::VertexMemoryLayout::count));
		for (uint32_t j = 0; j < static_cast<uint32_t>(emberCommon::VertexMemoryLayout::count); j++)
			for (uint32_t i = 0; i < static_cast<uint32_t>(emberCommon::GizmoRenderMode::count); i++)
			{
				emberCommon::GizmoRenderMode pipelineRenderMode = static_cast<emberCommon::GizmoRenderMode>(i);
				emberCommon::VertexMemoryLayout vertexMemoryLayout = static_cast<emberCommon::VertexMemoryLayout>(j);
				PipelineKey gizmoPipelineKey = PipelineKey::Create<RenderStage::gizmo>(pipelineRenderMode, vertexMemoryLayout);
				material.m_pipelines.emplace(
					gizmoPipelineKey,
					std::make_unique<GizmoPipeline>(
						material.m_name,
						material.m_vkPipelineLayout,
						pipelineRenderMode,
						vertexCode,
						fragmentCode,
						*vertexBindingVectors[j],
						*vertexAttributeVectors[j]));
			}

		// Create shader descriptorSetBinding:
		material.m_pShaderDescriptorSetBinding = std::make_unique<DescriptorSetBinding>(static_cast<Shader*>(&material), SHADER_SET_INDEX);
		return material;
	}
	Material Material::CreateShadow(const std::string& name, uint32_t shadowMapResolution, const std::filesystem::path& vertexSpv)
	{
		Material material = Material(name);
		material.m_materialType = emberCommon::MaterialType::shadow;
		material.m_pShadowRenderState = std::make_unique<emberCommon::ShadowRenderState>();

		// Load vertex shader:
		std::vector<char> vertexCode = emberSpirvReflect::ShaderReflection::ReadShaderCode(vertexSpv);
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
		NAME_VK_OBJECT(material.m_vkPipelineLayout, "PipelineLayout_Shadow_" + material.m_name);

		// Create pipelines:
		material.m_pipelines.reserve(static_cast<size_t>(emberCommon::VertexMemoryLayout::count));
		for (uint32_t i = 0; i < static_cast<uint32_t>(emberCommon::VertexMemoryLayout::count); i++)
		{
			PipelineKey pipelineKey = PipelineKey::Create<RenderStage::shadow>(static_cast<emberCommon::VertexMemoryLayout>(i));
			material.m_pipelines.emplace(
				pipelineKey,
				std::make_unique<ShadowPipeline>(
					material.m_name,
					material.m_vkPipelineLayout,
					shadowMapResolution,
					vertexCode,
					*vertexBindingVectors[i],
					*vertexAttributeVectors[i]));
		}

		// Create shader descriptorSetBinding:
		material.m_pShaderDescriptorSetBinding = std::make_unique<DescriptorSetBinding>(static_cast<Shader*>(&material), SHADER_SET_INDEX);
		return material;
	}
	Material Material::CreatePresent(const std::string& name, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv)
	{
		Material material = Material(name);
		material.m_materialType = emberCommon::MaterialType::present;
		material.m_pPresentRenderState = std::make_unique<emberCommon::PresentRenderState>();

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
		NAME_VK_OBJECT(material.m_vkPipelineLayout, "PipelineLayout_Present_" + material.m_name);

		// Create pipelines:
		material.m_pipelines.reserve(static_cast<size_t>(emberCommon::VertexMemoryLayout::count));
		for (uint32_t i = 0; i < static_cast<uint32_t>(emberCommon::VertexMemoryLayout::count); i++)
		{
			PipelineKey pipelineKey = PipelineKey::Create<RenderStage::present>(static_cast<emberCommon::VertexMemoryLayout>(i));
			material.m_pipelines.emplace(
				pipelineKey,
				std::make_unique<PresentPipeline>(
					material.m_name,
					material.m_vkPipelineLayout,
					vertexCode,
					fragmentCode,
					*vertexBindingVectors[i],
					*vertexAttributeVectors[i]));
		}

		// Create shader descriptorSetBinding:
		material.m_pShaderDescriptorSetBinding = std::make_unique<DescriptorSetBinding>(static_cast<Shader*>(&material), SHADER_SET_INDEX);
		return material;
	}
	Material::~Material()
	{

	}



	// Movable:
	Material::Material(Material&& other) noexcept = default;
	Material& Material::operator=(Material&& other) noexcept = default;


    
	// Setters:
	void Material::SetShadowMaterial(emberBackendInterface::IMaterial* pShadowMaterial)
	{
		if (!HasPipeline(PipelineType::forward))
		{
			LOG_WARN("Material::SetShadowMaterial(...) ignored. Only forward materials can override their shadow material.");
			return;
		}
		if (pShadowMaterial == nullptr)
		{
			m_pShadowMaterial = nullptr;
			return;
		}

		Material* pVulkanShadowMaterial = static_cast<Material*>(pShadowMaterial);
		if (!pVulkanShadowMaterial->HasPipeline(PipelineType::shadow))
		{
			LOG_WARN("Material::SetShadowMaterial(...) ignored. '{}' is not a shadow material.", pVulkanShadowMaterial->GetName());
			return;
		}
		m_pShadowMaterial = pVulkanShadowMaterial;
	}
	void Material::SetRenderQueue(int32_t renderQueue)
	{
		switch (m_materialType)
		{
			case emberCommon::MaterialType::forward:
				m_pForwardRenderState->renderQueue = renderQueue;
				return;
			case emberCommon::MaterialType::gizmo:
				m_pGizmoRenderState->renderQueue = renderQueue;
				return;
			default:
				throw std::runtime_error("Material::SetRenderQueue(...) failed. Render queue is not dynamic for this material type.");
		}
	}
	void Material::SetCullMode(emberCommon::CullMode cullMode)
	{
		switch (m_materialType)
		{
			case emberCommon::MaterialType::forward:
				m_pForwardRenderState->cullMode = cullMode;
				return;
			case emberCommon::MaterialType::gizmo:
				m_pGizmoRenderState->cullMode = cullMode;
				return;
			default:
				throw std::runtime_error("Material::SetCullMode(...) failed. Cull mode is not dynamic for this material type.");
		}
	}
	void Material::SetForwardRenderMode(emberCommon::ForwardRenderMode renderMode)
	{
		if (m_materialType != emberCommon::MaterialType::forward)
			throw std::runtime_error("Material::SetForwardRenderMode(...) failed. Material is not a forward material.");
		m_pForwardRenderState = std::make_unique<emberCommon::ForwardRenderState>(emberCommon::ForwardRenderState::ForwardDefault(renderMode));
	}
	void Material::SetGizmoRenderMode(emberCommon::GizmoRenderMode renderMode)
	{
		if (m_materialType != emberCommon::MaterialType::gizmo)
			throw std::runtime_error("Material::SetGizmoRenderMode(...) failed. Material is not a gizmo material.");
		m_pGizmoRenderState = std::make_unique<emberCommon::GizmoRenderState>(emberCommon::GizmoRenderState::GizmoDefault(renderMode));
	}



	// Getters:
	const std::string& Material::GetName() const
	{
		return m_name;
	}
	emberCommon::MaterialType Material::GetMaterialType() const
	{
		return m_materialType;
	}
	Material* Material::GetShadowMaterial() const
	{
		return m_pShadowMaterial;
	}
	emberBackendInterface::IDescriptorSetBinding* Material::GetShaderDescriptorSetBinding() const
	{
		return GetDescriptorSetBinding();
	}
	int32_t Material::GetRenderQueue() const
	{
		switch (m_materialType)
		{
			case emberCommon::MaterialType::forward:
				return m_pForwardRenderState->renderQueue;
			case emberCommon::MaterialType::gizmo:
				return m_pGizmoRenderState->renderQueue;
			default:
				return 0;
		}
	}
	emberCommon::CullMode Material::GetCullMode() const
	{
		switch (m_materialType)
		{
			case emberCommon::MaterialType::forward:
				return m_pForwardRenderState->cullMode;
			case emberCommon::MaterialType::gizmo:
				return m_pGizmoRenderState->cullMode;
			case emberCommon::MaterialType::outline:
				return m_pOutlineRenderState->cullMode;
			case emberCommon::MaterialType::shadow:
				return m_pShadowRenderState->cullMode;
			case emberCommon::MaterialType::present:
				return m_pPresentRenderState->cullMode;
			default:
				throw std::runtime_error("Material::GetCullMode(...) failed. Unsupported material type.");
		}
	}
	bool Material::IsTransparent() const
	{
		switch (m_materialType)
		{
			case emberCommon::MaterialType::forward:
				return m_pForwardRenderState->renderMode == emberCommon::ForwardRenderMode::transparent;
			case emberCommon::MaterialType::gizmo:
				return m_pGizmoRenderState->renderMode == emberCommon::GizmoRenderMode::transparent;
			default:
				return false;
		}
	}
	uint32_t Material::GetPipelineVariantIndex() const
	{
		switch (m_materialType)
		{
			case emberCommon::MaterialType::forward:
				return static_cast<uint32_t>(m_pForwardRenderState->renderMode);
			case emberCommon::MaterialType::gizmo:
				return static_cast<uint32_t>(m_pGizmoRenderState->renderMode);
			default:
				return 0;
		}
	}
	emberCommon::ForwardRenderMode Material::GetForwardRenderMode() const
	{
		if (m_materialType != emberCommon::MaterialType::forward)
			throw std::runtime_error("Material::GetForwardRenderMode(...) failed. Material is not a forward material.");
		return m_pForwardRenderState->renderMode;
	}
	emberCommon::GizmoRenderMode Material::GetGizmoRenderMode() const
	{
		if (m_materialType != emberCommon::MaterialType::gizmo)
			throw std::runtime_error("Material::GetGizmoRenderMode(...) failed. Material is not a gizmo material.");
		return m_pGizmoRenderState->renderMode;
	}
	const emberCommon::ForwardRenderState& Material::GetForwardRenderState() const
	{
		if (m_materialType != emberCommon::MaterialType::forward)
			throw std::runtime_error("Material::GetForwardRenderState(...) failed. Material is not a forward material.");
		return *m_pForwardRenderState;
	}
	const emberCommon::GizmoRenderState& Material::GetGizmoRenderState() const
	{
		if (m_materialType != emberCommon::MaterialType::gizmo)
			throw std::runtime_error("Material::GetGizmoRenderState(...) failed. Material is not a gizmo material.");
		return *m_pGizmoRenderState;
	}



	// Debugging:
	void Material::Print() const
	{
		PrintShaderInfo();
	}



    // Private methods:
	// Constructor:
	Material::Material(const std::string& name) : Shader(name)
	{
		m_materialType = emberCommon::MaterialType::count;
		m_pShadowMaterial = nullptr;
	}



	// Pipeline lookup:
	bool Material::HasPipeline(PipelineType pipelineType) const
	{
		for (const auto& [pipelineKey, _] : m_pipelines)
			if (pipelineKey.pipelineType == pipelineType)
				return true;
		return false;
	}
	const Pipeline* Material::GetPipelineByStage(PipelineType pipelineType, const Mesh* pMesh, uint32_t pipelineVariantIndex) const
	{
		if (!HasPipeline(pipelineType))
			throw std::runtime_error("Material::GetPipelineByStage(...) failed. Requested pipeline type is not supported by this material.");

		PipelineKey pipelineKey(pipelineType, pipelineVariantIndex, pMesh->GetVertexMemoryLayout());
		auto it = m_pipelines.find(pipelineKey);
		if (it == m_pipelines.end())
			throw std::runtime_error("Material::GetPipelineByStage(...) failed. Pipeline variant is not supported by this material.");
		return it->second.get();
	}
}