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



// Constructors/Destructor:
Material::Material(VulkanContext* pContext, Type type, const std::string& name, RenderQueue renderQueue, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv)
{
	m_type = type;
	m_pContext = pContext;
	m_name = name;
	m_renderQueue = renderQueue;

	// Shading material creation:
	if (m_type == Type::shading)
	{
		// Load vertex shader:
		std::vector<char> vertexCode = ReadShaderCode(vertexSpv);
		SpirvReflect vertexShaderReflect(vertexCode);
		vertexShaderReflect.GetDescriptorSetLayoutBindings(m_descriptorSetBindings, m_descriptorSetBindingNames, m_uniformBufferBlockMap);
		vertexShaderReflect.GetInputBindingAndAttributeDescriptions(m_inputBindingDescriptions, m_inputAttributeDescriptions, m_inputSemantics);
		m_meshBuffers.resize(m_inputBindingDescriptions.size(), VK_NULL_HANDLE);
		m_meshOffsets.resize(m_inputBindingDescriptions.size(), 0);

		// Load fragment shader:
		std::vector<char> fragmentCode = ReadShaderCode(fragmentSpv);
		SpirvReflect fragmentShaderReflect(fragmentCode);
		fragmentShaderReflect.GetDescriptorSetLayoutBindings(m_descriptorSetBindings, m_descriptorSetBindingNames, m_uniformBufferBlockMap);

		// Create pipeline:
		m_pPipeline = std::make_unique<ShadingPipeline>(m_pContext, vertexCode, fragmentCode, m_descriptorSetBindings, m_inputBindingDescriptions, m_inputAttributeDescriptions);
	}

	// Shadow material creation:
	else if (m_type == Type::shadow)
	{
		// Load vertex shader:
		std::vector<char> vertexCode = ReadShaderCode(vertexSpv);
		SpirvReflect vertexShaderReflect(vertexCode);
		vertexShaderReflect.GetDescriptorSetLayoutBindings(m_descriptorSetBindings, m_descriptorSetBindingNames, m_uniformBufferBlockMap);
		vertexShaderReflect.GetInputBindingAndAttributeDescriptions(m_inputBindingDescriptions, m_inputAttributeDescriptions, m_inputSemantics);
		m_meshBuffers.resize(m_inputBindingDescriptions.size(), VK_NULL_HANDLE);
		m_meshOffsets.resize(m_inputBindingDescriptions.size(), 0);

		// Create pipeline:
		m_pPipeline = std::make_unique<ShadowPipeline>(m_pContext, vertexCode, m_descriptorSetBindings, m_inputBindingDescriptions, m_inputAttributeDescriptions);
	}

	// Skybox material creation:
	else if (m_type == Type::skybox)
	{
		// Load vertex shader:
		std::vector<char> vertexCode = ReadShaderCode(vertexSpv);
		SpirvReflect vertexShaderReflect(vertexCode);
		vertexShaderReflect.GetDescriptorSetLayoutBindings(m_descriptorSetBindings, m_descriptorSetBindingNames, m_uniformBufferBlockMap);
		vertexShaderReflect.GetInputBindingAndAttributeDescriptions(m_inputBindingDescriptions, m_inputAttributeDescriptions, m_inputSemantics);
		m_meshBuffers.resize(m_inputBindingDescriptions.size(), VK_NULL_HANDLE);
		m_meshOffsets.resize(m_inputBindingDescriptions.size(), 0);

		// Load fragment shader:
		std::vector<char> fragmentCode = ReadShaderCode(fragmentSpv);
		SpirvReflect fragmentShaderReflect(fragmentCode);
		fragmentShaderReflect.GetDescriptorSetLayoutBindings(m_descriptorSetBindings, m_descriptorSetBindingNames, m_uniformBufferBlockMap);

		// Create pipeline:
		m_pPipeline = std::make_unique<SkyboxPipeline>(m_pContext, vertexCode, fragmentCode, m_descriptorSetBindings, m_inputBindingDescriptions, m_inputAttributeDescriptions);
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
const std::vector<VkDescriptorSetLayoutBinding>& Material::GetDescriptorSetLayoutBindings() const
{
	return m_descriptorSetBindings;
}
const std::vector<VkVertexInputBindingDescription>& Material::GetVertexInputBindingDescriptions() const
{
	return m_inputBindingDescriptions;
}
const std::vector<VkVertexInputAttributeDescription>& Material::GetVertexInputAttributeDescriptions() const
{
	return m_inputAttributeDescriptions;
}
const std::vector<std::string>& Material::GetDescriptorSetBindingNames() const
{
	return m_descriptorSetBindingNames;
}
const std::unordered_map<std::string, UniformBufferBlock*>& Material::GetUniformBufferBlockMap() const
{
	return m_uniformBufferBlockMap;
}
const uint32_t Material::GetVertexInputBindingDescriptionCount() const
{
	return static_cast<uint32_t>(m_inputBindingDescriptions.size());
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
	for (uint32_t i = 0; i < m_inputSemantics.size(); i++)
	{
		if (m_inputSemantics[i] == "POSITION")
			m_meshOffsets[i] = pMesh->GetPositionsOffset();
		else if (m_inputSemantics[i] == "NORMAL")
			m_meshOffsets[i] = pMesh->GetNormalsOffset();
		else if (m_inputSemantics[i] == "TANGENT")
			m_meshOffsets[i] = pMesh->GetTangentsOffset();
		else if (m_inputSemantics[i] == "COLOR")
			m_meshOffsets[i] = pMesh->GetColorsOffset();
		else if (m_inputSemantics[i] == "TEXCOORD0")
			m_meshOffsets[i] = pMesh->GetUVsOffset();
	}
	return m_meshOffsets.data();
}
VulkanContext* const Material::GetContext() const
{
	return m_pContext;
}
uint32_t Material::GetDescriptorSetLayoutBindingCount() const
{
	return static_cast<uint32_t>(m_descriptorSetBindings.size());
}
uint32_t Material::GetDescriptorBindingIndex(uint32_t i) const
{
	return m_descriptorSetBindings[i].binding;
}
VkDescriptorType Material::GetVkDescriptorType(uint32_t i) const
{
	return m_descriptorSetBindings[i].descriptorType;
}
const std::string& Material::GetDescriptorBindingName(uint32_t i) const
{
	return m_descriptorSetBindingNames[i];
}
UniformBufferBlock* Material::GetUniformBufferBlock(const std::string& name) const
{
	return m_uniformBufferBlockMap.at(name);
}



// Debugging:
void Material::PrintBindings() const
{
	std::string output = "\nMaterial: " + m_name + "\n\n";
	for (uint32_t i = 0; i < m_descriptorSetBindings.size(); i++)
	{
		std::string stageFlags;
		if ((int)m_descriptorSetBindings[i].stageFlags == VK_SHADER_STAGE_VERTEX_BIT)
			stageFlags = "VK_SHADER_STAGE_VERTEX_BIT";
		else if ((int)m_descriptorSetBindings[i].stageFlags == VK_SHADER_STAGE_FRAGMENT_BIT)
			stageFlags = "VK_SHADER_STAGE_FRAGMENT_BIT";

		std::string descriptorType;
		if ((int)m_descriptorSetBindings[i].descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
			descriptorType = "VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER";
		else if ((int)m_descriptorSetBindings[i].descriptorType == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE)
			descriptorType = "VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE";
		else if ((int)m_descriptorSetBindings[i].descriptorType == VK_DESCRIPTOR_TYPE_SAMPLER)
			descriptorType = "VK_DESCRIPTOR_TYPE_SAMPLER";

		output += "BindingName: " + m_descriptorSetBindingNames[i] + "\n";
		output += "Binding: " + std::to_string(m_descriptorSetBindings[i].binding) + "\n";
		output += "DescriptorType: " + descriptorType + "\n";
		output += "DescriptorCount: " + std::to_string(m_descriptorSetBindings[i].descriptorCount) + "\n";
		output += "StageFlags: " + stageFlags + "\n\n";
	}
	LOG_TRACE(output);
}
void Material::PrintUniformBuffers() const
{
	std::string output = "\nMaterial: " + m_name + "\n\n";
	for (const auto& [_, value] : m_uniformBufferBlockMap)
		output += value->ToString() + "\n";
	LOG_INFO(output);
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