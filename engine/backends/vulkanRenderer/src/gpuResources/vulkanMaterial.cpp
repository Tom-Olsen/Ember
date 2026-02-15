#include "vulkanMaterial.h"
#include "logger.h"
#include "vulkanDefaultPushConstant.h"
#include "vulkanForwardOpaquePipeline.h"
#include "vulkanForwardTransparentPipeline.h"
#include "vulkanMacros.h"
#include "vulkanMesh.h"
#include "vulkanPipeline.h"
#include "vulkanPresentPipeline.h"
#include "vulkanShadowPipeline.h"
#include "vulkanSkyboxPipeline.h"
#include "vulkanVertexBuffer.h"
#include "vulkanVertexLayout.h"
#include "vmaBuffer.h"
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructors/Destructor:
	Material::Material(emberCommon::MaterialType type, const std::string& name, uint32_t renderQueue, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv)
		: m_shaderReflection(Pipeline::s_descriptorSetCount), m_type(type), m_name(name), m_renderQueue(renderQueue)
	{
		// Load vertex shader:
		std::vector<char> vertexCode = emberSpirvReflect::ShaderReflection::ReadShaderCode(vertexSpv);
		m_shaderReflection.AddShaderStage(VK_SHADER_STAGE_VERTEX_BIT, vertexCode);

		// Load fragment shader:
		std::vector<char> fragmentCode = emberSpirvReflect::ShaderReflection::ReadShaderCode(fragmentSpv);
		m_shaderReflection.AddShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT, fragmentCode);

		// Prepare pipeline data:
		// Vertex input layout:
		CreateDescriptorSetLayout();
		const std::vector<emberSpirvReflect::VertexStageInfo>& vertexStageinfos = *m_shaderReflection.GetVertexShaderReflection()->GetVertexInfos();
		std::vector<VkVertexInputBindingDescription> vertexBindingsInterleaved = GetVertexBindingDescriptions<InterleavedVertexLayout>(vertexStageinfos);
		std::vector<VkVertexInputBindingDescription> vertexBindingsSeparate = GetVertexBindingDescriptions<SeparateVertexLayout>(vertexStageinfos);
		std::vector<VkVertexInputAttributeDescription> vertexAttributesInterleaved = GetVertexAttributeDescriptions<InterleavedVertexLayout>(vertexStageinfos);
		std::vector<VkVertexInputAttributeDescription> vertexAttributesSeparate = GetVertexAttributeDescriptions<SeparateVertexLayout>(vertexStageinfos);

		// Push constants layout:
		VkPushConstantRange pushConstantRange = {};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(DefaultPushConstant);

		// Pipeline layout:
		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
		pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(m_vkDescriptorSetLayouts.size());
		pipelineLayoutCreateInfo.pSetLayouts = m_vkDescriptorSetLayouts.data();
		pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
		pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;

		VKA(vkCreatePipelineLayout(Context::GetVkDevice(), &pipelineLayoutCreateInfo, nullptr, &m_vkPipelineLayout));
		NAME_VK_OBJECT(m_vkPipelineLayout, m_name + "ForwardOpaquePipelineLayout");

		// Create pipeline:
		m_pPipelines.resize(static_cast<size_t>(emberCommon::VertexMemoryLayout::count));
		switch (m_type)
		{
			case emberCommon::MaterialType::forwardOpaque:
				m_pPipelines[static_cast<size_t>(emberCommon::VertexMemoryLayout::interleaved)] = std::make_unique<ForwardOpaquePipeline<InterleavedVertexLayout>>(m_name, vertexCode, fragmentCode, m_vkDescriptorSetLayouts, vertexBindingsInterleaved, vertexAttributesInterleaved);
				m_pPipelines[static_cast<size_t>(emberCommon::VertexMemoryLayout::separate)] = std::make_unique<ForwardOpaquePipeline<SeparateVertexLayout>>(m_name, vertexCode, fragmentCode, m_vkDescriptorSetLayouts, vertexBindingsSeparate, vertexAttributesSeparate);
				break;
			case emberCommon::MaterialType::forwardTransparent:
				m_pPipelines[static_cast<size_t>(emberCommon::VertexMemoryLayout::interleaved)] = std::make_unique<ForwardTransparentPipeline<InterleavedVertexLayout>>(m_name, vertexCode, fragmentCode, m_vkDescriptorSetLayouts, vertexBindingsInterleaved, vertexAttributesInterleaved);
				m_pPipelines[static_cast<size_t>(emberCommon::VertexMemoryLayout::separate)] = std::make_unique<ForwardTransparentPipeline<SeparateVertexLayout>>(m_name, vertexCode, fragmentCode, m_vkDescriptorSetLayouts, vertexBindingsSeparate, vertexAttributesSeparate);
				break;
			case emberCommon::MaterialType::skybox:
				m_pPipelines[static_cast<size_t>(emberCommon::VertexMemoryLayout::interleaved)] = std::make_unique<SkyboxPipeline<InterleavedVertexLayout>>(m_name, vertexCode, fragmentCode, m_vkDescriptorSetLayouts, vertexBindingsInterleaved, vertexAttributesInterleaved);
				m_pPipelines[static_cast<size_t>(emberCommon::VertexMemoryLayout::separate)] = std::make_unique<SkyboxPipeline<SeparateVertexLayout>>(m_name, vertexCode, fragmentCode, m_vkDescriptorSetLayouts, vertexBindingsSeparate, vertexAttributesSeparate);
				break;
			case emberCommon::MaterialType::present:
				m_pPipelines[static_cast<size_t>(emberCommon::VertexMemoryLayout::interleaved)] = std::make_unique<PresentPipeline<InterleavedVertexLayout>>(m_name, vertexCode, fragmentCode, m_vkDescriptorSetLayouts, vertexBindingsInterleaved, vertexAttributesInterleaved);
				m_pPipelines[static_cast<size_t>(emberCommon::VertexMemoryLayout::separate)] = std::make_unique<PresentPipeline<SeparateVertexLayout>>(m_name, vertexCode, fragmentCode, m_vkDescriptorSetLayouts, vertexBindingsSeparate, vertexAttributesSeparate);
				break;
		}
	}
	// Special constructor for shadowMaterial:
	Material::Material(uint32_t shadowMapResolution)
		: m_shaderReflection(Pipeline::s_descriptorSetCount), m_type(emberCommon::MaterialType::shadow), m_name("shadowMaterial"), m_renderQueue(emberCommon::RenderQueue::shadow)
	{
		// Load vertex shader:
		std::filesystem::path directoryPath = (std::filesystem::path(VULKAN_LIBRARY_ROOT_PATH) / "src" / "shaders").make_preferred();
		std::vector<char> vertexCode = emberSpirvReflect::ShaderReflection::ReadShaderCode(directoryPath / "shadow.vert.spv");
		m_shaderReflection.AddShaderStage(VK_SHADER_STAGE_VERTEX_BIT, vertexCode);

		// Prepare pipeline data:
		CreateDescriptorSetLayout();
		const std::vector<emberSpirvReflect::VertexStageInfo>& vertexStageinfos = *m_shaderReflection.GetVertexShaderReflection()->GetVertexInfos();
		std::vector<VkVertexInputBindingDescription> vertexBindingsInterleaved = GetVertexBindingDescriptions<InterleavedVertexLayout>(vertexStageinfos);
		std::vector<VkVertexInputBindingDescription> vertexBindingsSeparate = GetVertexBindingDescriptions<SeparateVertexLayout>(vertexStageinfos);
		std::vector<VkVertexInputAttributeDescription> vertexAttributesInterleaved = GetVertexAttributeDescriptions<InterleavedVertexLayout>(vertexStageinfos);
		std::vector<VkVertexInputAttributeDescription> vertexAttributesSeparate = GetVertexAttributeDescriptions<SeparateVertexLayout>(vertexStageinfos);

		// Create pipeline:
		m_pPipelines[static_cast<size_t>(emberCommon::VertexMemoryLayout::interleaved)] = std::make_unique<ShadowPipeline<InterleavedVertexLayout>>(m_name, shadowMapResolution, vertexCode, m_vkDescriptorSetLayouts, vertexBindingsInterleaved, vertexAttributesInterleaved);
		m_pPipelines[static_cast<size_t>(emberCommon::VertexMemoryLayout::separate)] = std::make_unique<ShadowPipeline<SeparateVertexLayout>>(m_name, shadowMapResolution, vertexCode, m_vkDescriptorSetLayouts, vertexBindingsSeparate, vertexAttributesSeparate);
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