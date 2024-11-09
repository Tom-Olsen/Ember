#include "material.h"
#include "renderPassManager.h"
#include "logger.h"
#include <vector>
#include <fstream>



// Constructors:
Material::Material(VulkanContext* context, Type type, const std::string& name, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv)
{
	this->context = context;
	this->name = name;
	this->frameIndex = 0;
	this->type = type;

	// Forward material creation:
	if (type == Type::forward)
	{
		// Load vertex shader:
		std::vector<char> vertexCode = ReadShaderCode(vertexSpv);
		SpirvReflect vertexShaderReflect(vertexCode);
		GetDescriptorSetLayoutBindings(vertexShaderReflect, VK_SHADER_STAGE_VERTEX_BIT);

		// Load fragment shader:
		std::vector<char> fragmentCode = ReadShaderCode(fragmentSpv);
		SpirvReflect fragmentShaderReflect(fragmentCode);
		GetDescriptorSetLayoutBindings(fragmentShaderReflect, VK_SHADER_STAGE_FRAGMENT_BIT);

		// Create pipeline:
		pipeline = std::make_unique<ForwardPipeline>(context, &RenderPassManager::GetRenderPass("forwardRenderPass")->renderPass, vertexCode, fragmentCode, bindings);
	}

	// Shadow material creation:
	if (type == Type::shadow)
	{
		// Load vertex shader:
		std::vector<char> vertexCode = ReadShaderCode(vertexSpv);
		SpirvReflect vertexShaderReflect(vertexCode);
		GetDescriptorSetLayoutBindings(vertexShaderReflect, VK_SHADER_STAGE_VERTEX_BIT);

		// Create pipeline:
		pipeline = std::make_unique<ShadowPipeline>(context, &RenderPassManager::GetRenderPass("shadowRenderPass")->renderPass, vertexCode, bindings);
	}
}



// Destructor:
Material::~Material()
{

}



// Public methods:
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
	LOG_INFO(output);
}
void Material::PrintUniformBuffers() const
{
	std::string output = "\nMaterial: " + name + "\n\n";
	for (const auto& [key, value] : uniformBufferBlockMap)
	{
		output += "UniformBufferBlock: " + key + "\n";
		output += value->ToString() + "\n\n";
	}
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
void Material::GetDescriptorSetLayoutBindings(const SpirvReflect& shaderReflect, VkShaderStageFlagBits shaderStage)
{
	if (VkShaderStageFlagBits((int)shaderReflect.module.shader_stage) != shaderStage)
	{
		LOG_CRITICAL("Shader stage does not agree with given shader.");
		std::abort();
	}

	// Shader descriptor set reflection:
	std::vector<SpvReflectDescriptorSet*> shaderDescriptorSets = shaderReflect.GetDescriptorSetsReflection();

	for (uint32_t setIndex = 0; setIndex < shaderDescriptorSets.size(); setIndex++)
	{
		SpvReflectDescriptorSet* setReflection = shaderDescriptorSets[setIndex];
		for (uint32_t bindingIndex = 0; bindingIndex < setReflection->binding_count; bindingIndex++)
		{
			SpvReflectDescriptorBinding* bindingReflection = setReflection->bindings[bindingIndex];
			VkDescriptorSetLayoutBinding layoutBinding = {};
			layoutBinding.binding = bindingReflection->binding;
			layoutBinding.descriptorType = VkDescriptorType((int)bindingReflection->descriptor_type);
			layoutBinding.descriptorCount = bindingReflection->count;
			layoutBinding.stageFlags = shaderStage;
			layoutBinding.pImmutableSamplers = nullptr;

			// Add binding and name to lists:
			bindings.push_back(layoutBinding);
			bindingNames.push_back(bindingReflection->name);

			// In case of uniform buffer create UniformBufferBlock:
			if (bindingReflection->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
			{
				SpvReflectBlockVariable& blockReflection = bindingReflection->block;
				UniformBufferBlock* uniformBufferBlock = shaderReflect.GetUniformBufferBlock(blockReflection, setIndex, bindingIndex);
				uniformBufferBlockMap.emplace(uniformBufferBlock->name, uniformBufferBlock);
			}
		}
	}
}