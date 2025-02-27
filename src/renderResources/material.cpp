#include "material.h"
#include "logger.h"
#include "mesh.h"
#include "forwardPipeline.h"
#include "shadowPipeline.h"
#include "skyboxPipeline.h"
#include "spirvReflect.h"
#include "vmaBuffer.h"
#include "vulkanContext.h"



namespace emberEngine
{
	// Constructors/Destructor:
	Material::Material(VulkanContext* pContext, Type type, const std::string& name, uint32_t renderQueue, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv)
	{
		m_type = type;
		m_pContext = pContext;
		m_name = name;
		m_renderQueue = renderQueue;
		m_pDescriptorBoundResources = std::make_unique<DescriptorBoundResources>();

		// Forward material creation:
		if (m_type == Type::forward)
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
			m_pPipeline = std::make_unique<ForwardPipeline>(m_pContext, vertexCode, fragmentCode, m_pDescriptorBoundResources->descriptorSetLayoutBindings, m_pVertexInputDescriptions.get());
		}

		// Shadow material creation:
		else if (m_type == Type::shadow)
		{
			// Load vertex shader:
			std::vector<char> vertexCode = ReadShaderCode(vertexSpv);
			SpirvReflect vertexShaderReflect(vertexCode);
			vertexShaderReflect.AddDescriptorBoundResources(m_pDescriptorBoundResources.get());
			m_pVertexInputDescriptions = std::unique_ptr<VertexInputDescriptions>(vertexShaderReflect.GetVertexInputDescriptions());
			m_meshBuffers.resize(m_pVertexInputDescriptions->size, VK_NULL_HANDLE);
			m_meshOffsets.resize(m_pVertexInputDescriptions->size, 0);

			// Create pipeline:
			m_pPipeline = std::make_unique<ShadowPipeline>(m_pContext, vertexCode, m_pDescriptorBoundResources->descriptorSetLayoutBindings, m_pVertexInputDescriptions.get());
		}

		// Skybox material creation:
		else if (m_type == Type::skybox)
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
			m_pPipeline = std::make_unique<SkyboxPipeline>(m_pContext, vertexCode, fragmentCode, m_pDescriptorBoundResources->descriptorSetLayoutBindings, m_pVertexInputDescriptions.get());
		}
	}
	Material::~Material()
	{

	}



	// Public methods:
	// Getters:
	Material::Type Material::GetType() const
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
			m_meshBuffers[i] = pMesh->GetVertexBuffer(m_pContext)->GetVkBuffer();
		return m_meshBuffers.data();
	}
	const VkDeviceSize* const Material::GetMeshOffsets(Mesh* pMesh)
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