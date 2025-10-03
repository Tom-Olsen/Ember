#include "vulkanMaterial.h"
#include "logger.h"
#include "vulkanForwardOpaquePipeline.h"
#include "vulkanForwardTransparentPipeline.h"
#include "vulkanMesh.h"
#include "vulkanPresentPipeline.h"
#include "vulkanShadowPipeline.h"
#include "vulkanSkyboxPipeline.h"
#include "vulkanVertexBuffer.h"
#include "spirvReflect.h"
#include "vmaBuffer.h"
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructors/Destructor:
	Material::Material(emberCommon::MaterialType type, const std::string& name, uint32_t renderQueue, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv)
	{
		m_type = type;
		m_name = name;
		m_renderQueue = renderQueue;
		m_pDescriptorBoundResources = std::make_unique<DescriptorBoundResources>();

		// Opaque forward material creation:
		if (m_type == emberCommon::MaterialType::forwardOpaque)
		{
			// Load vertex shader:
			std::vector<char> vertexCode = ReadShaderCode(vertexSpv);
			SpirvReflect vertexShaderReflect(vertexCode);
			vertexShaderReflect.AddDescriptorBoundResources(m_pDescriptorBoundResources.get());
			m_pVertexInputDescriptions = std::unique_ptr<VertexInputDescriptions>(vertexShaderReflect.GetVertexInputDescriptions());
			m_meshBuffers.resize(m_pVertexInputDescriptions->size, VK_NULL_HANDLE);
			m_meshOffsets.resize(m_pVertexInputDescriptions->size, 0);

			// Load fragment shader:
			std::vector<char> fragmentCode = ReadShaderCode(fragmentSpv);
			SpirvReflect fragmentShaderReflect(fragmentCode);
			fragmentShaderReflect.AddDescriptorBoundResources(m_pDescriptorBoundResources.get());

			// Create pipeline:
			m_pPipeline = std::make_unique<ForwardOpaquePipeline>(vertexCode, fragmentCode, m_pDescriptorBoundResources->descriptorSetLayoutBindings, m_pVertexInputDescriptions.get());
		}

		// Transparent forward material creation:
		if (m_type == emberCommon::MaterialType::forwardTransparent)
		{
			// Load vertex shader:
			std::vector<char> vertexCode = ReadShaderCode(vertexSpv);
			SpirvReflect vertexShaderReflect(vertexCode);
			vertexShaderReflect.AddDescriptorBoundResources(m_pDescriptorBoundResources.get());
			m_pVertexInputDescriptions = std::unique_ptr<VertexInputDescriptions>(vertexShaderReflect.GetVertexInputDescriptions());
			m_meshBuffers.resize(m_pVertexInputDescriptions->size, VK_NULL_HANDLE);
			m_meshOffsets.resize(m_pVertexInputDescriptions->size, 0);

			// Load fragment shader:
			std::vector<char> fragmentCode = ReadShaderCode(fragmentSpv);
			SpirvReflect fragmentShaderReflect(fragmentCode);
			fragmentShaderReflect.AddDescriptorBoundResources(m_pDescriptorBoundResources.get());

			// Create pipeline:
			m_pPipeline = std::make_unique<ForwardTransparentPipeline>(vertexCode, fragmentCode, m_pDescriptorBoundResources->descriptorSetLayoutBindings, m_pVertexInputDescriptions.get());
		}

		// Skybox material creation:
		else if (m_type == emberCommon::MaterialType::skybox)
		{
			// Load vertex shader:
			std::vector<char> vertexCode = ReadShaderCode(vertexSpv);
			SpirvReflect vertexShaderReflect(vertexCode);
			vertexShaderReflect.AddDescriptorBoundResources(m_pDescriptorBoundResources.get());
			m_pVertexInputDescriptions = std::unique_ptr<VertexInputDescriptions>(vertexShaderReflect.GetVertexInputDescriptions());
			m_meshBuffers.resize(m_pVertexInputDescriptions->size, VK_NULL_HANDLE);
			m_meshOffsets.resize(m_pVertexInputDescriptions->size, 0);

			// Load fragment shader:
			std::vector<char> fragmentCode = ReadShaderCode(fragmentSpv);
			SpirvReflect fragmentShaderReflect(fragmentCode);
			fragmentShaderReflect.AddDescriptorBoundResources(m_pDescriptorBoundResources.get());

			// Create pipeline:
			m_pPipeline = std::make_unique<SkyboxPipeline>(vertexCode, fragmentCode, m_pDescriptorBoundResources->descriptorSetLayoutBindings, m_pVertexInputDescriptions.get());
		}

		// Present material creation:
		else if (m_type == emberCommon::MaterialType::present)
		{
			// Load vertex shader:
			std::vector<char> vertexCode = ReadShaderCode(vertexSpv);
			SpirvReflect vertexShaderReflect(vertexCode);
			vertexShaderReflect.AddDescriptorBoundResources(m_pDescriptorBoundResources.get());
			m_pVertexInputDescriptions = std::unique_ptr<VertexInputDescriptions>(vertexShaderReflect.GetVertexInputDescriptions());
			m_meshBuffers.resize(m_pVertexInputDescriptions->size, VK_NULL_HANDLE);
			m_meshOffsets.resize(m_pVertexInputDescriptions->size, 0);

			// Load fragment shader:
			std::vector<char> fragmentCode = ReadShaderCode(fragmentSpv);
			SpirvReflect fragmentShaderReflect(fragmentCode);
			fragmentShaderReflect.AddDescriptorBoundResources(m_pDescriptorBoundResources.get());

			// Create pipeline:
			m_pPipeline = std::make_unique<PresentPipeline>(vertexCode, fragmentCode, m_pDescriptorBoundResources->descriptorSetLayoutBindings, m_pVertexInputDescriptions.get());
		}
	}
	// Special constructor for shadowMaterial:
	Material::Material(uint32_t shadowMapResolution)
	{
		m_type = emberCommon::MaterialType::shadow;
		m_name = "shadowMaterial";
		m_renderQueue = emberCommon::RenderQueue::shadow;
		m_pDescriptorBoundResources = std::make_unique<DescriptorBoundResources>();

		// Load vertex shader:
		std::filesystem::path directoryPath = (std::filesystem::path(VULKAN_LIBRARY_ROOT_PATH) / "src" / "shaders").make_preferred();
		std::vector<char> vertexCode = ReadShaderCode(directoryPath / "shadow.vert.spv");
		SpirvReflect vertexShaderReflect(vertexCode);
		vertexShaderReflect.AddDescriptorBoundResources(m_pDescriptorBoundResources.get());
		m_pVertexInputDescriptions = std::unique_ptr<VertexInputDescriptions>(vertexShaderReflect.GetVertexInputDescriptions());
		m_meshBuffers.resize(m_pVertexInputDescriptions->size, VK_NULL_HANDLE);
		m_meshOffsets.resize(m_pVertexInputDescriptions->size, 0);

		// Create pipeline:
		m_pPipeline = std::make_unique<ShadowPipeline>(shadowMapResolution, vertexCode, m_pDescriptorBoundResources->descriptorSetLayoutBindings, m_pVertexInputDescriptions.get());
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
	const VertexInputDescriptions* const Material::GetVertexInputDescriptions() const
	{
		return m_pVertexInputDescriptions.get();
	}
	const VkBuffer* const Material::GetMeshBuffers(Mesh* pMesh)
	{
		// All entries are stored in the same buffer:
		for (uint32_t i = 0; i < m_meshBuffers.size(); i++)
			m_meshBuffers[i] = pMesh->GetVertexBuffer()->GetVmaBuffer()->GetVkBuffer();
		return m_meshBuffers.data();
	}
	const uint64_t* const Material::GetMeshOffsets(Mesh* pMesh)
	{
		for (uint32_t i = 0; i < m_pVertexInputDescriptions->size; i++)
		{
			const std::string& semantic = m_pVertexInputDescriptions->semantics[i];

			if (semantic == "POSITION")
				m_meshOffsets[i] = pMesh->GetPositionsOffset();
			else if (semantic == "NORMAL")
				m_meshOffsets[i] = pMesh->GetNormalsOffset();
			else if (semantic == "TANGENT")
				m_meshOffsets[i] = pMesh->GetTangentsOffset();
			else if (semantic == "COLOR")
				m_meshOffsets[i] = pMesh->GetColorsOffset();
			else if (semantic == "TEXCOORD0")
				m_meshOffsets[i] = pMesh->GetUVsOffset();
			else
				LOG_WARN("Material system does not support the VertexInputDescription semantic '{}' yet. Material::GetMeshOffsets(Mesh*) must be updated,", semantic);
		}
		return m_meshOffsets.data();
	}
}