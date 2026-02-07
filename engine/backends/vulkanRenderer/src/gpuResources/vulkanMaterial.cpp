#include "vulkanMaterial.h"
#include "logger.h"
#include "vulkanForwardOpaquePipeline.h"
#include "vulkanForwardTransparentPipeline.h"
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

		// Create descriptor sets:
		m_shaderReflection.CreateDescriptorSets();

		// Create pipeline:
		m_pPipelines.resize(static_cast<size_t>(emberCommon::VertexMemoryLayout::count));
		switch (m_type)
		{
			case emberCommon::MaterialType::forwardOpaque:
				m_pPipelines[static_cast<size_t>(emberCommon::VertexMemoryLayout::interleaved)] = std::make_unique<ForwardOpaquePipeline<InterleavedVertexLayout>>(m_name, vertexCode, fragmentCode, m_shaderReflection);
				m_pPipelines[static_cast<size_t>(emberCommon::VertexMemoryLayout::separate)] = std::make_unique<ForwardOpaquePipeline<SeparateVertexLayout>>(m_name, vertexCode, fragmentCode, m_shaderReflection);
				break;
			case emberCommon::MaterialType::forwardTransparent:
				m_pPipelines[static_cast<size_t>(emberCommon::VertexMemoryLayout::interleaved)] = std::make_unique<ForwardTransparentPipeline<InterleavedVertexLayout>>(m_name, vertexCode, fragmentCode, m_shaderReflection);
				m_pPipelines[static_cast<size_t>(emberCommon::VertexMemoryLayout::separate)] = std::make_unique<ForwardTransparentPipeline<SeparateVertexLayout>>(m_name, vertexCode, fragmentCode, m_shaderReflection);
				break;
			case emberCommon::MaterialType::skybox:
				m_pPipelines[static_cast<size_t>(emberCommon::VertexMemoryLayout::interleaved)] = std::make_unique<SkyboxPipeline<InterleavedVertexLayout>>(m_name, vertexCode, fragmentCode, m_shaderReflection);
				m_pPipelines[static_cast<size_t>(emberCommon::VertexMemoryLayout::separate)] = std::make_unique<SkyboxPipeline<SeparateVertexLayout>>(m_name, vertexCode, fragmentCode, m_shaderReflection);
				break;
			case emberCommon::MaterialType::present:
				m_pPipelines[static_cast<size_t>(emberCommon::VertexMemoryLayout::interleaved)] = std::make_unique<PresentPipeline<InterleavedVertexLayout>>(m_name, vertexCode, fragmentCode, m_shaderReflection);
				m_pPipelines[static_cast<size_t>(emberCommon::VertexMemoryLayout::separate)] = std::make_unique<PresentPipeline<SeparateVertexLayout>>(m_name, vertexCode, fragmentCode, m_shaderReflection);
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

		// Create descriptor sets:
		m_shaderReflection.CreateDescriptorSets();

		// Create pipeline:
		m_pPipeline = std::make_unique<ShadowPipeline<SeparateVertexLayout>>(m_name, shadowMapResolution, vertexCode, m_shaderReflection);
	}
	Material::~Material()
	{

	}



	// Movable:
	Material::Material(Material&& other) = default;
	Material& Material::operator=(Material&& other) = default;



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