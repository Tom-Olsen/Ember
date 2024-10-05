#include "material.h"
#include <vector>
#include <fstream>
#include "macros.h"



// Constructor:
Material::Material(uint32_t framesInFlight, VulkanLogicalDevice* logicalDevice, VulkanPhysicalDevice* physicalDevice, VulkanDescriptorPool* descriptorPool, VulkanRenderpass* renderpass, const std::string& vertexSpv, const std::string& fragmentSpv, const std::vector<VulkanUniformBuffer>& uniformBuffers, Texture2d* texture2d, VulkanSampler* sampler)
{
	this->logicalDevice = logicalDevice;
	this->physicalDevice = physicalDevice;
	this->descriptorPool = descriptorPool;

	// Read .spv shader files:
	vertexCode = ReadShaderCode(vertexSpv);
	fragmentCode = ReadShaderCode(fragmentSpv);

	// Create shader reflections:
	SpirvReflect vertexShaderReflect(vertexCode);
	SpirvReflect fragmentShaderReflect(fragmentCode);

	// Create pipeline:
	std::vector<VkDescriptorSetLayoutBinding> bindings = SpirvReflect::GetDescriptorSetLayoutBindings(vertexShaderReflect, fragmentShaderReflect);
	pipeline = std::make_unique<VulkanPipeline>(logicalDevice, physicalDevice, renderpass, vertexCode, fragmentCode, bindings);

	// Create & fill descriptor sets:
	CreateDescriptorSets(framesInFlight);
	FillDescriptorSets(framesInFlight, uniformBuffers, texture2d, sampler);
}



// Destructor:
Material::~Material()
{

}



// Private:
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

void Material::CreateDescriptorSets(uint32_t framesInFlight)
{
	std::vector<VkDescriptorSetLayout> layouts(framesInFlight, pipeline->descriptorSetLayout); // same layout for all frames

	VkDescriptorSetAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
	allocInfo.descriptorPool = descriptorPool->descriptorPool;
	allocInfo.descriptorSetCount = framesInFlight;
	allocInfo.pSetLayouts = layouts.data();

	descriptorSets.resize(framesInFlight);
	VKA(vkAllocateDescriptorSets(logicalDevice->device, &allocInfo, descriptorSets.data()));
}
void Material::FillDescriptorSets(uint32_t framesInFlight, const std::vector<VulkanUniformBuffer>& uniformBuffers, Texture2d* texture2d, VulkanSampler* sampler)
{
	if (uniformBuffers.size() != framesInFlight)
		throw std::runtime_error((std::string)"size of uniformBuffers (" + std::to_string(uniformBuffers.size()) + (std::string)") does not match descriptor count (" + std::to_string(framesInFlight) + (std::string)").");

	for (size_t i = 0; i < framesInFlight; i++)
	{
		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = uniformBuffers[i].buffer->buffer;
		bufferInfo.offset = 0;
		bufferInfo.range = uniformBuffers[i].bufferSize;

		VkDescriptorImageInfo imageInfo = {};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = texture2d->image->imageView;

		VkDescriptorImageInfo samplerInfo = {};
		samplerInfo.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		samplerInfo.sampler = sampler->sampler;

		std::array<VkWriteDescriptorSet, 3> descriptorWrites;
		descriptorWrites[0] = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
		descriptorWrites[0].dstSet = descriptorSets[i];
		descriptorWrites[0].dstBinding = 0;			// binding point in shader
		descriptorWrites[0].dstArrayElement = 0;	// can bind arrays, but we only bind one buffer => startIndex = 0
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;	// how many array elements get updateed
		descriptorWrites[0].pBufferInfo = &bufferInfo;	// used for buffer data
		descriptorWrites[0].pImageInfo = nullptr;		// uded for image data
		descriptorWrites[0].pTexelBufferView = nullptr; // uded for buffer views

		descriptorWrites[1] = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
		descriptorWrites[1].dstSet = descriptorSets[i];
		descriptorWrites[1].dstBinding = 1;            // Binding point for the image
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pBufferInfo = nullptr;
		descriptorWrites[1].pImageInfo = &imageInfo;
		descriptorWrites[1].pTexelBufferView = nullptr;

		descriptorWrites[2] = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
		descriptorWrites[2].dstSet = descriptorSets[i];
		descriptorWrites[2].dstBinding = 2;            // Binding point for the sampler
		descriptorWrites[2].dstArrayElement = 0;
		descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
		descriptorWrites[2].descriptorCount = 1;
		descriptorWrites[2].pBufferInfo = nullptr;
		descriptorWrites[2].pImageInfo = &samplerInfo;
		descriptorWrites[2].pTexelBufferView = nullptr;

		vkUpdateDescriptorSets(logicalDevice->device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		// VkCopyDescriptorSet(...) allows for copying of descriptor sets
	}
}