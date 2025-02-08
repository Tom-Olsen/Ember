#include "material.h"
#include "logger.h"
#include "mesh.h"
#include "shadingPipeline.h"
#include "shadowPipeline.h"
#include "skyboxPipeline.h"
#include "spirvReflect.h"
#include "vmaBuffer.h"
#include "vulkanContext.h"
#include <fstream>



namespace emberEngine
{
	// Constructors/Destructor:
	Material::Material(VulkanContext* pContext, Type type, const std::string& name, RenderQueue renderQueue, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv)
	{
		m_type = type;
		m_pContext = pContext;
		m_name = name;
		m_renderQueue = renderQueue;
		m_pDescriptorBoundResources = std::make_unique<DescriptorBoundResources>();

		// Shading material creation:
		if (m_type == Type::shading)
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
			m_pPipeline = std::make_unique<ShadingPipeline>(m_pContext, vertexCode, fragmentCode, m_pDescriptorBoundResources->descriptorSetLayoutBindings, m_pVertexInputDescriptions.get());
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
	const std::string& Material::GetName() const
	{
		return m_name;
	}
	Material::RenderQueue Material::GetRenderQueue() const
	{
		return m_renderQueue;
	}
	const Pipeline* const Material::GetPipeline() const
	{
		return m_pPipeline.get();
	}
	const DescriptorBoundResources* const Material::GetDescriptorBoundResources() const
	{
		return m_pDescriptorBoundResources.get();
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
		}
		return m_meshOffsets.data();
	}
	VulkanContext* const Material::GetContext() const
	{
		return m_pContext;
	}



	// Debugging:
	void Material::PrintBindings() const
	{
		std::string output = "\nMaterial: " + m_name + "\n";

		for (uint32_t i = 0; i < m_pDescriptorBoundResources->size; i++)
		{
			const VkDescriptorSetLayoutBinding& descriptorSetLayoutBinding = m_pDescriptorBoundResources->descriptorSetLayoutBindings[i];

			std::string stageFlags;
			if ((int)descriptorSetLayoutBinding.stageFlags == VK_SHADER_STAGE_VERTEX_BIT)
				stageFlags = "VK_SHADER_STAGE_VERTEX_BIT";
			else if ((int)descriptorSetLayoutBinding.stageFlags == VK_SHADER_STAGE_FRAGMENT_BIT)
				stageFlags = "VK_SHADER_STAGE_FRAGMENT_BIT";

			std::string descriptorType;
			if ((int)descriptorSetLayoutBinding.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
				descriptorType = "VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER";
			else if ((int)descriptorSetLayoutBinding.descriptorType == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE)
				descriptorType = "VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE";
			else if ((int)descriptorSetLayoutBinding.descriptorType == VK_DESCRIPTOR_TYPE_SAMPLER)
				descriptorType = "VK_DESCRIPTOR_TYPE_SAMPLER";

			output += "  BindingName: " + m_pDescriptorBoundResources->descriptorSetBindingNames[i] + "\n";
			output += "  Binding: " + std::to_string(descriptorSetLayoutBinding.binding) + "\n";
			output += "  DescriptorType: " + descriptorType + "\n";
			output += "  DescriptorCount: " + std::to_string(descriptorSetLayoutBinding.descriptorCount) + "\n";
			output += "  StageFlags: " + stageFlags + "\n\n";
		}
		LOG_TRACE(output);
	}
	void Material::PrintUniformBuffers() const
	{
		std::string output = "\nMaterial: " + m_name + "\n";
		for (const auto& [_, value] : m_pDescriptorBoundResources->uniformBufferBlockMap)
			output += value->ToString(2) + "\n";
		LOG_TRACE(output);
	}



	// Private methods:
	std::vector<char> Material::ReadShaderCode(const std::filesystem::path& spvFile)
	{
		// Open shader file:
		std::ifstream file(spvFile, std::ios::binary);
		if (!file.is_open())
			LOG_CRITICAL("Error opening shader file: {}", spvFile.string());

		// Get file size:
		file.seekg(0, std::ios::end);
		size_t fileSize = static_cast<size_t>(file.tellg());
		file.seekg(0, std::ios::beg);

		// Copy code:
		std::vector<char> code(fileSize);
		file.read(code.data(), fileSize);
		file.close();

		return code;
	}
}