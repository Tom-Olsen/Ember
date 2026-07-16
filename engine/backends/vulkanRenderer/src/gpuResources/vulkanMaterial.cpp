#include "vulkanMaterial.h"
#include "descriptorSetMacros.h"
#include "logger.h"
#include "vulkanDefaultPushConstant.h"
#include "vulkanDescriptorSetBinding.h"
#include "vulkanForwardPipeline.h"
#include "vulkanGizmoPipeline.h"
#include "vulkanMacros.h"
#include "vulkanMesh.h"
#include "vulkanPipeline.h"
#include "vulkanPresentPipeline.h"
#include "vulkanShadowPipeline.h"
#include "vulkanVertexBuffer.h"
#include "vulkanVertexLayout.h"
#include "vmaBuffer.h"
#include <stdexcept>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	
	// Public methods:
	// Factories/Destructor:
	Material Material::CreateForward(const std::string& name, emberCommon::RenderMode renderMode, int32_t renderQueue, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv)
	{
		Material material = Material(name);
		material.m_renderMode = renderMode;
		material.m_renderQueue = renderQueue;

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
		material.m_pForwardPipelines.reserve(static_cast<size_t>(static_cast<size_t>(emberCommon::RenderMode::count) * static_cast<size_t>(emberCommon::VertexMemoryLayout::count)));
		material.m_pGizmoPipelines.reserve(static_cast<size_t>(static_cast<size_t>(emberCommon::RenderMode::count) * static_cast<size_t>(emberCommon::VertexMemoryLayout::count)));
		for (uint32_t j = 0; j < static_cast<uint32_t>(emberCommon::VertexMemoryLayout::count); j++)
			for (uint32_t i = 0; i < static_cast<uint32_t>(emberCommon::RenderMode::count); i++)
			{
				material.m_pForwardPipelines.emplace_back(
					std::make_unique<ForwardPipeline>(
						material.m_name,
						material.m_vkPipelineLayout,
						static_cast<emberCommon::RenderMode>(i),
						vertexCode,
						fragmentCode,
						*vertexBindingVectors[j],
						*vertexAttributeVectors[j]));
				material.m_pGizmoPipelines.emplace_back(
					std::make_unique<GizmoPipeline>(
						material.m_name,
						material.m_vkPipelineLayout,
						static_cast<emberCommon::RenderMode>(i),
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
		material.m_renderQueue = 0; // has no inpact on shadow materials.
		material.m_renderMode = emberCommon::RenderMode::opaque;

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
		material.m_pShadowPipelines.reserve(static_cast<size_t>(emberCommon::VertexMemoryLayout::count));
		for (uint32_t i = 0; i < static_cast<uint32_t>(emberCommon::VertexMemoryLayout::count); i++)
		{
			material.m_pShadowPipelines.emplace_back(
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
		material.m_renderQueue = 0; // has no inpact on present materials.
		material.m_renderMode = emberCommon::RenderMode::opaque;

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
		material.m_pPresentPipelines.reserve(static_cast<size_t>(emberCommon::VertexMemoryLayout::count));
		for (uint32_t i = 0; i < static_cast<uint32_t>(emberCommon::VertexMemoryLayout::count); i++)
		{
			material.m_pPresentPipelines.emplace_back(
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
	void Material::SetRenderQueue(int32_t renderQueue)
	{
		m_renderQueue = renderQueue;
	}
    void Material::SetRenderMode(emberCommon::RenderMode renderMode)
    {
    	// Only materials with renderMode-specific pipeline variants support renderMode changes: forward and gizmo.
    	if (HasPipeline(PipelineType::forward) || HasPipeline(PipelineType::gizmo))
    	    m_renderMode = renderMode;
        else
            LOG_WARN("Material::SetRenderMode(...): modifying renderMode of material '{}' is not possible.", GetName());
    }
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



	// Getters:
	const std::string& Material::GetName() const
	{
		return m_name;
	}
	int32_t Material::GetRenderQueue() const
	{
		return m_renderQueue;
	}
	emberCommon::RenderMode Material::GetRenderMode() const
	{
		return m_renderMode;
	}
	Material* Material::GetShadowMaterial() const
	{
		return m_pShadowMaterial;
	}
	emberBackendInterface::IDescriptorSetBinding* Material::GetShaderDescriptorSetBinding() const
	{
		return GetDescriptorSetBinding();
	}
	const Pipeline* Material::GetPipeline(const Mesh* pMesh, PipelineType pipelineType) const
	{
		if (!HasPipeline(pipelineType))
			throw std::runtime_error("Material::GetPipeline(...) failed. Requested pipeline type is not supported by this material.");
		const size_t pipelineIndex = GetPipelineIndex(pMesh, pipelineType);
		Pipeline* pPipeline = nullptr;
		switch (pipelineType)
		{
		    case PipelineType::gizmo:
		    	pPipeline = m_pGizmoPipelines[pipelineIndex].get();
		    	break;
		    case PipelineType::shadow:
		    	pPipeline = m_pShadowPipelines[pipelineIndex].get();
		    	break;
		    case PipelineType::forward:
		    	pPipeline = m_pForwardPipelines[pipelineIndex].get();
		    	break;
		    case PipelineType::present:
		    	pPipeline = m_pPresentPipelines[pipelineIndex].get();
		    	break;
		    default:
		    	throw std::runtime_error("Material::GetPipeline(...) failed. Unsupported pipeline type.");
		}
		assert(pPipeline && "Material::GetPipeline(...): Pipeline not supported for this vertex layout");
		return pPipeline;
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
		m_renderMode = emberCommon::RenderMode::opaque;
		m_renderQueue = emberCommon::RenderQueue::opaque;
		m_pShadowMaterial = nullptr;
	}

	// Pipeline indexing:
	bool Material::HasPipeline(PipelineType pipelineType) const
	{
		switch (pipelineType)
		{
		    case PipelineType::gizmo:
		    	return !m_pGizmoPipelines.empty();
		    case PipelineType::shadow:
		    	return !m_pShadowPipelines.empty();
		    case PipelineType::forward:
		    	return !m_pForwardPipelines.empty();
		    case PipelineType::present:
		    	return !m_pPresentPipelines.empty();
		    default:
		    	throw std::runtime_error("Material::HasPipeline(...) failed. Unsupported pipeline type.");
		}
	}
	size_t Material::GetPipelineIndex(const Mesh* pMesh, PipelineType pipelineType) const
	{
		const size_t vertexLayoutIndex = static_cast<size_t>(pMesh->GetVertexMemoryLayout());
		switch (pipelineType)
		{
		    case PipelineType::gizmo:
		    {
		    	const size_t renderModeCount = static_cast<size_t>(emberCommon::RenderMode::count);
		    	return static_cast<size_t>(m_renderMode) + vertexLayoutIndex * renderModeCount;
		    }
		    case PipelineType::shadow:
		    case PipelineType::forward:
		    case PipelineType::present:
		    	return vertexLayoutIndex;
		    default:
		    	throw std::runtime_error("Material::GetPipelineIndex(...) failed. Unsupported pipeline type.");
		}
	}
}