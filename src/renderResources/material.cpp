#include "material.h"
#include "shadingPipeline.h"
#include "shadowPipeline.h"
#include "skyboxPipeline.h"
#include "logger.h"
#include <vector>
#include <fstream>



// Constructors:
Material::Material(VulkanContext* context, Type type, const std::string& name, RenderQueue renderQueue, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv)
{
	this->type = type;
	this->context = context;
	this->name = name;
	this->renderQueue = renderQueue;

	// Shading material creation:
	if (type == Type::shading)
	{
		// Load vertex shader:
		std::vector<char> vertexCode = ReadShaderCode(vertexSpv);
		SpirvReflect vertexShaderReflect(vertexCode);
		vertexShaderReflect.GetDescriptorSetLayoutBindings(bindings, bindingNames, uniformBufferBlockMap);

		// Load fragment shader:
		std::vector<char> fragmentCode = ReadShaderCode(fragmentSpv);
		SpirvReflect fragmentShaderReflect(fragmentCode);
		fragmentShaderReflect.GetDescriptorSetLayoutBindings(bindings, bindingNames, uniformBufferBlockMap);

		// Create pipeline:
		pipeline = std::make_unique<ShadingPipeline>(context, vertexCode, fragmentCode, bindings);
	}

	// Shadow material creation:
	else if (type == Type::shadow)
	{
		// Load vertex shader:
		std::vector<char> vertexCode = ReadShaderCode(vertexSpv);
		SpirvReflect vertexShaderReflect(vertexCode);
		vertexShaderReflect.GetDescriptorSetLayoutBindings(bindings, bindingNames, uniformBufferBlockMap);

		// Create pipeline:
		pipeline = std::make_unique<ShadowPipeline>(context, vertexCode, bindings);
	}

	// Skybox material creation:
	else if (type == Type::skybox)
	{
		// Load vertex shader:
		std::vector<char> vertexCode = ReadShaderCode(vertexSpv);
		SpirvReflect vertexShaderReflect(vertexCode);
		vertexShaderReflect.GetDescriptorSetLayoutBindings(bindings, bindingNames, uniformBufferBlockMap);

		// Load fragment shader:
		std::vector<char> fragmentCode = ReadShaderCode(fragmentSpv);
		SpirvReflect fragmentShaderReflect(fragmentCode);
		fragmentShaderReflect.GetDescriptorSetLayoutBindings(bindings, bindingNames, uniformBufferBlockMap);

		// Create pipeline:
		pipeline = std::make_unique<SkyboxPipeline>(context, vertexCode, fragmentCode, bindings);
	}
}



// Destructor:
Material::~Material()
{

}



// Public methods:
VulkanContext* Material::GetContext()
{
	return context;
}
uint32_t Material::GetBindingCount() const
{
	return static_cast<uint32_t>(bindings.size());
}
uint32_t Material::GetBindingIndex(uint32_t i) const
{
	return bindings[i].binding;
}
VkDescriptorType Material::GetBindingType(uint32_t i) const
{
	return bindings[i].descriptorType;
}
std::string Material::GetBindingName(uint32_t i) const
{
	return bindingNames[i];
}
UniformBufferBlock* Material::GetUniformBufferBlock(const std::string& name) const
{
	return uniformBufferBlockMap.at(name);
}



// Debugging:
void Material::PrintBindings() const
{
	std::string output = "\nMaterial: " + name + "\n\n";
	for (uint32_t i = 0; i < bindings.size(); i++)
	{
		std::string stageFlags;
		if ((int)bindings[i].stageFlags == VK_SHADER_STAGE_VERTEX_BIT)
			stageFlags = "VK_SHADER_STAGE_VERTEX_BIT";
		else if ((int)bindings[i].stageFlags == VK_SHADER_STAGE_FRAGMENT_BIT)
			stageFlags = "VK_SHADER_STAGE_FRAGMENT_BIT";

		std::string descriptorType;
		if ((int)bindings[i].descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
			descriptorType = "VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER";
		else if ((int)bindings[i].descriptorType == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE)
			descriptorType = "VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE";
		else if ((int)bindings[i].descriptorType == VK_DESCRIPTOR_TYPE_SAMPLER)
			descriptorType = "VK_DESCRIPTOR_TYPE_SAMPLER";

		output += "BindingName: " + bindingNames[i] + "\n";
		output += "Binding: " + std::to_string(bindings[i].binding) + "\n";
		output += "DescriptorType: " + descriptorType + "\n";
		output += "DescriptorCount: " + std::to_string(bindings[i].descriptorCount) + "\n";
		output += "StageFlags: " + stageFlags + "\n\n";
	}
	LOG_TRACE(output);
}
void Material::PrintUniformBuffers() const
{
	std::string output = "\nMaterial: " + name + "\n\n";
	for (const auto& [_, value] : uniformBufferBlockMap)
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