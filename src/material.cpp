#include "material.h"
#include <vector>
#include <fstream>
#include "macros.h"



// Constructor:
Material::Material(VulkanContext* context, VkRenderPass* renderPass, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv, std::string name)
{
	// TODO: -try removing these two
	// Not sure why I need to init these here (doesnt work otherwise). Init happens in main.cpp.
	TextureManager::Init(context);
	SamplerManager::Init(context);

	this->context = context;
	this->forwardRenderPass = forwardRenderPass;
	this->name = name;
	this->frameIndex = 0;
	materialProperties = std::make_unique<MaterialProperties>(context);

	// Read .spv shader files:
	std::vector<char> vertexCode = ReadShaderCode(vertexSpv);
	std::vector<char> fragmentCode = ReadShaderCode(fragmentSpv);

	// Create shader reflections:
	SpirvReflect vertexShaderReflect(vertexCode);
	SpirvReflect fragmentShaderReflect(fragmentCode);

	// Create pipeline (unique for each material):
	GetDescriptorSetLayoutBindings(vertexShaderReflect, VK_SHADER_STAGE_VERTEX_BIT);
	GetDescriptorSetLayoutBindings(fragmentShaderReflect, VK_SHADER_STAGE_FRAGMENT_BIT);
	pipeline = std::make_unique<VulkanPipeline>(context, renderPass, vertexCode, fragmentCode, bindings);
	materialProperties->SetPipeline(pipeline.get());
	materialProperties->InitDescriptorSets();
}



// Destructor:
Material::~Material()
{

}



// Public:
MaterialProperties* Material::GetMaterialPropertiesCopy()
{
	return materialProperties->GetCopy();
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

	for (uint32_t i = 0; i < shaderDescriptorSets.size(); i++)
	{
		SpvReflectDescriptorSet* set = shaderDescriptorSets[i];
		for (uint32_t j = 0; j < set->binding_count; j++)
		{
			SpvReflectDescriptorBinding* binding = set->bindings[j];
			VkDescriptorSetLayoutBinding layoutBinding = {};
			layoutBinding.binding = binding->binding;
			layoutBinding.descriptorType = VkDescriptorType((int)binding->descriptor_type);
			layoutBinding.descriptorCount = binding->count;
			layoutBinding.stageFlags = shaderStage;
			layoutBinding.pImmutableSamplers = nullptr;
			bindings.push_back(layoutBinding);

			// Create resource based on descriptor type:
			if (layoutBinding.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
				materialProperties->InitUniformResourceBinding(binding->name, binding->binding, RenderMatrizes());
			if (layoutBinding.descriptorType == VK_DESCRIPTOR_TYPE_SAMPLER)
				materialProperties->InitSamplerResourceBinding(binding->name, binding->binding, SamplerManager::GetSampler("default"));
			if (layoutBinding.descriptorType == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE)
				materialProperties->InitTexture2dResourceBinding(binding->name, binding->binding, TextureManager::GetTexture2d("white"));
		}
	}
}