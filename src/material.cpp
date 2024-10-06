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

	// Default resources:
	defaultSampler = std::make_unique<VulkanSampler>(logicalDevice, physicalDevice);
	defaultTexture2d = std::make_unique<Texture2d>(logicalDevice, physicalDevice, "../textures/white.png");
	defaultUniformBuffer = std::make_unique<GlobalUniformObject>(glm::mat4x4(1.0f), glm::mat4x4(1.0f), glm::mat4x4(1.0f));

	// Read .spv shader files:
	vertexCode = ReadShaderCode(vertexSpv);
	fragmentCode = ReadShaderCode(fragmentSpv);

	// Create shader reflections:
	SpirvReflect vertexShaderReflect(vertexCode);
	SpirvReflect fragmentShaderReflect(fragmentCode);

	fragmentShaderReflect.PrintDescriptorSetsInfo();
	vertexShaderReflect.PrintDescriptorSetsInfo();

	// Create pipeline:
	std::vector<VkDescriptorSetLayoutBinding> bindings = GetDescriptorSetLayoutBindings(vertexShaderReflect, fragmentShaderReflect);
	pipeline = std::make_unique<VulkanPipeline>(logicalDevice, physicalDevice, renderpass, vertexCode, fragmentCode, bindings);

	// Create & fill descriptor sets:
	CreateDescriptorSets(framesInFlight);
	FillDescriptorSets(framesInFlight, uniformBuffers, texture2d, sampler);

	for (const auto& pair : samplerMap)
		LOG_INFO("Sampler: {}", pair.first);
	for (const auto& pair : texture2dMap)
		LOG_INFO("Texture2d: {}", pair.first);
	for (const auto& pair : uniformBufferMap)
		LOG_INFO("UniformBuffer: {}", pair.first);
}



// Destructor:
Material::~Material()
{

}



// Public setters:
void Material::SetUniformBuffer(const std::string& name, GlobalUniformObject* constantBuffer)
{
	// if key exists, replace its value
	auto it = uniformBufferMap.find(name);
	if (it != uniformBufferMap.end())
		it->second = constantBuffer;
}
void Material::SetTexture2d(const std::string& name, Texture2d* texture)
{
	// if key exists, replace its value
	auto it = texture2dMap.find(name);
	if (it != texture2dMap.end())
		it->second = texture;
}
void Material::SetSampler(const std::string& name, VulkanSampler* sampler)
{
	// if key exists, replace its value
	auto it = samplerMap.find(name);
	if (it != samplerMap.end())
		it->second = sampler;
}

// Public getters:
VulkanSampler* Material::GetSampler(const std::string& name)
{
	auto it = samplerMap.find(name);
	if (it == samplerMap.end())
	{
		LOG_WARN("Sampler not found: {}", name);
		return defaultSampler.get();
	}
	return it->second;
}
Texture2d* Material::GetTexture2d(const std::string& name)
{
	auto it = texture2dMap.find(name);
	if (it == texture2dMap.end())
	{
		LOG_WARN("Texture2d not found: {}", name);
		return defaultTexture2d.get();
	}
	return it->second;
}
GlobalUniformObject* Material::GetUniformBuffer(const std::string& name)
{
	auto it = uniformBufferMap.find(name);
	if (it == uniformBufferMap.end())
	{
		LOG_WARN("Uniform buffer not found: {}", name);
		return defaultUniformBuffer.get();
	}
	return it->second;
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

std::vector<VkDescriptorSetLayoutBinding> Material::GetDescriptorSetLayoutBindings(const SpirvReflect& vertexShaderReflect, const SpirvReflect& fragmentShaderReflect)
{
	if (vertexShaderReflect.module.shader_stage != SPV_REFLECT_SHADER_STAGE_VERTEX_BIT)
	{
		LOG_CRITICAL("Vertex shader is not a vertex shader.");
		std::abort();
	}
	if (fragmentShaderReflect.module.shader_stage != SPV_REFLECT_SHADER_STAGE_FRAGMENT_BIT)
	{
		LOG_CRITICAL("Fragment shader is not a fragment shader.");
		std::abort();
	}

	std::vector<SpvReflectDescriptorSet*> vertexDescriptorSets = vertexShaderReflect.GetDescriptorSetsReflection();
	std::vector<SpvReflectDescriptorSet*> fragmentDescriptorSets = fragmentShaderReflect.GetDescriptorSetsReflection();
	std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;

	// Get vertex shader specific descriptor set layout bindings:
	for (uint32_t i = 0; i < vertexDescriptorSets.size(); i++)
	{
		SpvReflectDescriptorSet* set = vertexDescriptorSets[i];
		for (uint32_t j = 0; j < set->binding_count; j++)
		{
			SpvReflectDescriptorBinding* binding = set->bindings[j];
			VkDescriptorSetLayoutBinding layoutBinding = {};
			layoutBinding.binding = binding->binding;
			layoutBinding.descriptorType = VkDescriptorType((int)binding->descriptor_type);
			layoutBinding.descriptorCount = binding->count;
			layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
			layoutBinding.pImmutableSamplers = nullptr;
			descriptorSetLayoutBindings.push_back(layoutBinding);

			// Create resources based on descriptor type:
			switch (layoutBinding.descriptorType)
			{
			case VK_DESCRIPTOR_TYPE_SAMPLER:
				samplerMap.emplace(binding->name, defaultSampler.get());
				break;
			case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
				texture2dMap.emplace(binding->name, defaultTexture2d.get());
				break;
			case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
				uniformBufferMap.emplace(binding->name, defaultUniformBuffer.get());
				break;
			default:
				break;
			}
		}
	}

	// Get fragment shader specific descriptor set layout bindings:
	for (uint32_t i = 0; i < fragmentDescriptorSets.size(); i++)
	{
		SpvReflectDescriptorSet* set = fragmentDescriptorSets[i];
		for (uint32_t j = 0; j < set->binding_count; j++)
		{
			SpvReflectDescriptorBinding* binding = set->bindings[j];
			VkDescriptorSetLayoutBinding layoutBinding = {};
			layoutBinding.binding = binding->binding;
			layoutBinding.descriptorType = VkDescriptorType((int)binding->descriptor_type);
			layoutBinding.descriptorCount = binding->count;
			layoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
			layoutBinding.pImmutableSamplers = nullptr;
			descriptorSetLayoutBindings.push_back(layoutBinding);

			// Create resources based on descriptor type:
			switch (layoutBinding.descriptorType)
			{
			case VK_DESCRIPTOR_TYPE_SAMPLER:
				samplerMap.emplace(binding->name, defaultSampler.get());
				break;
			case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
				texture2dMap.emplace(binding->name, defaultTexture2d.get());
				break;
			case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
				uniformBufferMap.emplace(binding->name, defaultUniformBuffer.get());
				break;
			default:
				break;
			}
		}
	}
	return descriptorSetLayoutBindings;
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
		descriptorWrites[0].dstBinding = 0;				// binding point in shader
		descriptorWrites[0].dstArrayElement = 0;		// can bind arrays, but we only bind one buffer => startIndex = 0
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;		// how many array elements get updateed
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