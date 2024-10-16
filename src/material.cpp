#include "material.h"
#include <vector>
#include <fstream>
#include "macros.h"



// Constructor:
Material::Material(uint32_t framesInFlight, VulkanContext* context, VulkanDescriptorPool* descriptorPool, VulkanRenderpass* renderpass, const std::string& vertexSpv, const std::string& fragmentSpv)
{
	this->context = context;
	this->descriptorPool = descriptorPool;
	this->framesInFlight = framesInFlight;
	this->frameIndex = 0;

	// Render resources:
	materialProperties.resize(framesInFlight);

	// Default resources:
	defaultUniformObject.model = glm::rotate(Float4x4(1.0f), glm::radians(90.0f), Float3(0.0f, 0.0f, 1.0f));
	defaultUniformObject.view = glm::lookAt(Float3(2.0f, 2.0f, 2.0f), Float3(0.0f, 0.0f, 0.0f), Float3(0.0f, 0.0f, 1.0f));
	defaultUniformObject.proj = glm::perspective(glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 10.0f);
	defaultUniformObject.proj[1][1] *= -1; // flip y-axis as it is inverted by default
	defaultSampler = std::make_unique<VulkanSampler>(context);
	defaultTexture2d = std::make_unique<Texture2d>(context, "../textures/white.png");

	// Read .spv shader files:
	std::vector<char> vertexCode = ReadShaderCode(vertexSpv);
	std::vector<char> fragmentCode = ReadShaderCode(fragmentSpv);

	// Create shader reflections:
	SpirvReflect vertexShaderReflect(vertexCode);
	SpirvReflect fragmentShaderReflect(fragmentCode);

	// Create pipeline:
	GetDescriptorSetLayoutBindings(vertexShaderReflect, VK_SHADER_STAGE_VERTEX_BIT);
	GetDescriptorSetLayoutBindings(fragmentShaderReflect, VK_SHADER_STAGE_FRAGMENT_BIT);
	pipeline = std::make_unique<VulkanPipeline>(context, renderpass, vertexCode, fragmentCode, bindings);
	
	// Set some default values:
	emptyMaterialProperties = materialProperties[0];
	descriptorWrites.reserve(std::max({ emptyMaterialProperties.uniformBufferMap.size(), emptyMaterialProperties.samplerMap.size(), emptyMaterialProperties.texture2dMap.size() }));

	// Create & fill descriptor sets:
	CreateDescriptorSets();
	for (uint32_t i = 0; i < framesInFlight; i++)
	{
		FillUniformBufferDescriptorSets(i);
		FillSamplerDescriptorSets(i);
		FillTexture2dDescriptorSets(i);
	}
}



// Destructor:
Material::~Material()
{

}



// Public:
MaterialProperties Material::GetEmptyMaterialProperties()
{
	return emptyMaterialProperties;
}
void Material::SetMaterialProperties(const MaterialProperties& properties, uint32_t frameIndex)
{
	this->frameIndex = frameIndex;
	materialProperties[frameIndex] = properties;

	FillUniformBufferDescriptorSets(frameIndex);
	FillSamplerDescriptorSets(frameIndex);
	FillTexture2dDescriptorSets(frameIndex);
}



// Private methods:
std::vector<char> Material::ReadShaderCode(const std::string& spvFile)
{
    // Open shader file:
    std::ifstream file(spvFile, std::ios::binary);
    if (!file.is_open())
        LOG_CRITICAL("Error opening shader file: {}", spvFile);

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
			for (uint32_t j = 0; j < framesInFlight; j++)
			{
				if (layoutBinding.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
				{
					VulkanUniformBuffer uniformBuffer(context, sizeof(UniformObject));
					uniformBuffer.UpdateBuffer(defaultUniformObject);
					materialProperties[j].InitUniformObjectResourceBinding(binding->name, ResourceBinding<VulkanUniformBuffer>(binding->binding, uniformBuffer));
				}
				if (layoutBinding.descriptorType == VK_DESCRIPTOR_TYPE_SAMPLER)
					materialProperties[j].InitSamplerResourceBinding(binding->name, ResourceBinding<VulkanSampler*>(binding->binding, defaultSampler.get()));
				if (layoutBinding.descriptorType == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE)
					materialProperties[j].InitTexture2dResourceBinding(binding->name, ResourceBinding<Texture2d*>(binding->binding, defaultTexture2d.get()));
			}
		}
	}
}

void Material::CreateDescriptorSets()
{
	std::vector<VkDescriptorSetLayout> layouts(framesInFlight, pipeline->descriptorSetLayout); // same layout for all frames

	VkDescriptorSetAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
	allocInfo.descriptorPool = descriptorPool->descriptorPool;
	allocInfo.descriptorSetCount = framesInFlight;
	allocInfo.pSetLayouts = layouts.data();

	descriptorSets.resize(framesInFlight);
	VKA(vkAllocateDescriptorSets(context->logicalDevice->device, &allocInfo, descriptorSets.data()));
}
void Material::FillUniformBufferDescriptorSets(uint32_t frameIndex)
{
	descriptorWrites.clear();
	for (const auto& pair : materialProperties[frameIndex].uniformBufferMap)
	{
		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = pair.second.resource.buffer->buffer;
		bufferInfo.offset = 0;
		bufferInfo.range = pair.second.resource.size;

		VkWriteDescriptorSet descriptorWrite = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
		descriptorWrite.dstSet = descriptorSets[frameIndex];
		descriptorWrite.dstBinding = pair.second.binding;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;
		descriptorWrite.pImageInfo = nullptr;
		descriptorWrite.pTexelBufferView = nullptr;
		descriptorWrites.push_back(descriptorWrite);
	}
	vkUpdateDescriptorSets(context->LogicalDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
}
void Material::FillSamplerDescriptorSets(uint32_t frameIndex)
{
	descriptorWrites.clear();
	for (const auto& pair : materialProperties[frameIndex].samplerMap)
	{
		VkDescriptorImageInfo samplerInfo = {};
		samplerInfo.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		samplerInfo.sampler = pair.second.resource->sampler;

		VkWriteDescriptorSet descriptorWrite = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
		descriptorWrite.dstSet = descriptorSets[frameIndex];
		descriptorWrite.dstBinding = pair.second.binding;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = nullptr;
		descriptorWrite.pImageInfo = &samplerInfo;
		descriptorWrite.pTexelBufferView = nullptr;
		descriptorWrites.push_back(descriptorWrite);
	}
	vkUpdateDescriptorSets(context->LogicalDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
}
void Material::FillTexture2dDescriptorSets(uint32_t frameIndex)
{
	descriptorWrites.clear();
	for (const auto& pair : materialProperties[frameIndex].texture2dMap)
	{
		VkDescriptorImageInfo imageInfo = {};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = pair.second.resource->image->imageView;

		VkWriteDescriptorSet descriptorWrite = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
		descriptorWrite.dstSet = descriptorSets[frameIndex];
		descriptorWrite.dstBinding = pair.second.binding;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = nullptr;
		descriptorWrite.pImageInfo = &imageInfo;
		descriptorWrite.pTexelBufferView = nullptr;
		descriptorWrites.push_back(descriptorWrite);
	}
	vkUpdateDescriptorSets(context->LogicalDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
}