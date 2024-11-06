#include "materialProperties.h"
#include "samplerManager.h"
#include "textureManager.h"



// Consructor:
MaterialProperties::MaterialProperties(VulkanContext* context, Pipeline* pipeline)
{
	this->context = context;
	this->pipeline = pipeline;

	// Create resource bindings for each frameInFlight:
	uniformBufferMaps = std::vector<std::unordered_map<std::string, ResourceBinding<VulkanUniformBuffer>>>(context->framesInFlight);
	samplerMaps = std::vector<std::unordered_map<std::string, ResourceBinding<VulkanSampler*>>>(context->framesInFlight);
	texture2dMaps = std::vector<std::unordered_map<std::string, ResourceBinding<Texture2d*>>>(context->framesInFlight);
}
MaterialProperties::MaterialProperties(VulkanContext* context, Pipeline* pipeline, std::vector<VkDescriptorSetLayoutBinding>& bindings, std::vector<std::string>& bindingNames)
	: MaterialProperties(context, pipeline)
{
	for (uint32_t frameIndex = 0; frameIndex < context->framesInFlight; frameIndex++)
	{
		for (size_t i = 0; i < bindings.size(); i++)
		{
			if (bindings[i].descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
			{
				if (bindingNames[i] == "RenderMatrizes")
					InitUniformBufferResourceBinding(bindingNames[i], bindings[i].binding, RenderMatrizes(), frameIndex);
				else if (bindingNames[i] == "LightData")
					InitUniformBufferResourceBinding(bindingNames[i], bindings[i].binding, LightData(), frameIndex);
				else
					LOG_WARN("Unknown uniform buffer name: {}", bindingNames[i]);
			}
			else if (bindings[i].descriptorType == VK_DESCRIPTOR_TYPE_SAMPLER)
				InitSamplerResourceBinding(bindingNames[i], bindings[i].binding, SamplerManager::GetSampler("colorSampler"), frameIndex);
			else if (bindings[i].descriptorType == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE)
				InitTexture2dResourceBinding(bindingNames[i], bindings[i].binding, TextureManager::GetTexture2d("white"), frameIndex);
		}
	}
	InitDescriptorSets();
}



// Destructor:
MaterialProperties::~MaterialProperties()
{

}



// Public methods:
// Setters:
void MaterialProperties::SetUniform(const std::string& name, const RenderMatrizes& renderMatrizes)
{
	// if key exists, replace its value
	auto it = uniformBufferMaps[context->frameIndex].find(name);
	if (it != uniformBufferMaps[context->frameIndex].end())
		it->second.resource.UpdateBuffer(renderMatrizes);
	else
		LOG_WARN("Uniform '{}' not found in uniformBufferMap!", name);
}
void MaterialProperties::SetUniform(const std::string& name, const LightData& lightData)
{
	// if key exists, replace its value
	auto it = uniformBufferMaps[context->frameIndex].find(name);
	if (it != uniformBufferMaps[context->frameIndex].end())
		it->second.resource.UpdateBuffer(lightData);
	else
		LOG_WARN("Uniform '{}' not found in uniformBufferMap!", name);
}
void MaterialProperties::SetSampler(const std::string& name, VulkanSampler* sampler)
{
	// if key exists, replace its value
	auto it = samplerMaps[context->frameIndex].find(name);
	if (it != samplerMaps[context->frameIndex].end())
	{
		if (it->second.resource != sampler)
		{
			it->second.resource = sampler;
			UpdateDescriptorSet(context->frameIndex, it->second);
		}
	}
	else
		LOG_WARN("Sampler '{}' not found in samplerMap!", name);
}
void MaterialProperties::SetSamplerForAllFrames(const std::string& name, VulkanSampler* sampler)
{
	// if key exists, replace its value
	for (uint32_t frameIndex = 0; frameIndex < context->framesInFlight; frameIndex++)
	{
		auto it = samplerMaps[frameIndex].find(name);
		if (it != samplerMaps[frameIndex].end())
		{
			VKA(vkDeviceWaitIdle(context->LogicalDevice()));
			if (it->second.resource != sampler)
			{
				it->second.resource = sampler;
				UpdateDescriptorSet(frameIndex, it->second);
			}
		}
		else
			LOG_WARN("Sampler '{}' not found in samplerMap!", name);
	}
}
void MaterialProperties::SetTexture2d(const std::string& name, Texture2d* texture)
{
	// if key exists, replace its value
	auto it = texture2dMaps[context->frameIndex].find(name);
	if (it != texture2dMaps[context->frameIndex].end())
	{
		if (it->second.resource != texture)
		{
			it->second.resource = texture;
			UpdateDescriptorSet(context->frameIndex, it->second);
		}
	}
	else
		LOG_WARN("Texture2d '{}' not found in texture2dMap!", name);
}
void MaterialProperties::SetTexture2dForAllFrames(const std::string& name, Texture2d* texture)
{
	// if key exists, replace its value
	for (uint32_t frameIndex = 0; frameIndex < context->framesInFlight; frameIndex++)
	{
		auto it = texture2dMaps[frameIndex].find(name);
		if (it != texture2dMaps[frameIndex].end())
		{
			VKA(vkDeviceWaitIdle(context->LogicalDevice()));
			if (it->second.resource != texture)
			{
				it->second.resource = texture;
				UpdateDescriptorSet(frameIndex, it->second);
			}
		}
		else
			LOG_WARN("Texture2d '{}' not found in texture2dMap!", name);
	}
}



// Getters:
MaterialProperties* MaterialProperties::GetCopy()
{
	MaterialProperties* copy = new MaterialProperties(context, pipeline);

	// Copy bindings:
	for (uint32_t frameIndex = 0; frameIndex < context->framesInFlight; frameIndex++)
	{
		for (const auto& [name, resourceBinding] : uniformBufferMaps[frameIndex])
		{
			if (name == "RenderMatrizes")
				copy->InitUniformBufferResourceBinding(name, resourceBinding.binding, RenderMatrizes(), frameIndex);
			else if (name == "LightData")
				copy->InitUniformBufferResourceBinding(name, resourceBinding.binding, LightData(), frameIndex);
			else
				LOG_WARN("Unknown uniform buffer name: {}", name);
		}
		for (const auto& [name, resourceBinding] : samplerMaps[frameIndex])
			copy->InitSamplerResourceBinding(name, resourceBinding.binding, resourceBinding.resource, frameIndex);
		for (const auto& [name, resourceBinding] : texture2dMaps[frameIndex])
			copy->InitTexture2dResourceBinding(name, resourceBinding.binding, resourceBinding.resource, frameIndex);
	}

	// Initialize descriptor sets:
	copy->InitDescriptorSets();

	return copy;
}



// Debugging:
void MaterialProperties::PrintMaps() const
{
	for (uint32_t frameIndex = 0; frameIndex < context->framesInFlight; frameIndex++)
	{
		LOG_INFO("UniformBufferMaps[{}]:", frameIndex);
		for (const auto& [name, resourceBinding] : uniformBufferMaps[frameIndex])
		{
			LOG_TRACE("binding: {}, bindingName: {}", resourceBinding.binding, name);
		}
		LOG_INFO("SamplerMaps[{}]:", frameIndex);
		for (const auto& [name, resourceBinding] : samplerMaps[frameIndex])
		{
			LOG_TRACE("binding: {}, bindingName: {}, samplerName: {}", resourceBinding.binding, name, resourceBinding.resource->name);
		}
		LOG_INFO("Texture2dMaps[{}]:", frameIndex);
		for (const auto& [name, resourceBinding] : texture2dMaps[frameIndex])
		{
			LOG_TRACE("binding: {}, bindingName: {}, textureName: {}", resourceBinding.binding, name, resourceBinding.resource->name);
		}
		LOG_TRACE("\n");
	}
}



// Private methods:
// Initializers:
void MaterialProperties::InitUniformBufferResourceBinding(std::string name, uint32_t binding, const RenderMatrizes& renderMatrizes, uint32_t frameIndex)
{
	auto it = uniformBufferMaps[frameIndex].find(name);
	if (it == uniformBufferMaps[frameIndex].end())
	{
		VulkanUniformBuffer uniformBuffer(context, sizeof(RenderMatrizes));
		uniformBuffer.UpdateBuffer(renderMatrizes);
		uniformBufferMaps[frameIndex].emplace(name, ResourceBinding<VulkanUniformBuffer>(binding, uniformBuffer));
	}
}
void MaterialProperties::InitUniformBufferResourceBinding(std::string name, uint32_t binding, const LightData& lightData, uint32_t frameIndex)
{
	auto it = uniformBufferMaps[frameIndex].find(name);
	if (it == uniformBufferMaps[frameIndex].end())
	{
		VulkanUniformBuffer uniformBuffer(context, sizeof(LightData));
		uniformBuffer.UpdateBuffer(lightData);
		uniformBufferMaps[frameIndex].emplace(name, ResourceBinding<VulkanUniformBuffer>(binding, uniformBuffer));
	}
}
void MaterialProperties::InitSamplerResourceBinding(std::string name, uint32_t binding, VulkanSampler* sampler, uint32_t frameIndex)
{
	auto it = samplerMaps[frameIndex].find(name);
	if (it == samplerMaps[frameIndex].end())
			samplerMaps[frameIndex].emplace(name, ResourceBinding<VulkanSampler*>(binding, sampler));
}
void MaterialProperties::InitTexture2dResourceBinding(std::string name, uint32_t binding, Texture2d* texture2d, uint32_t frameIndex)
{
	auto it = texture2dMaps[frameIndex].find(name);
	if (it == texture2dMaps[frameIndex].end())
			texture2dMaps[frameIndex].emplace(name, ResourceBinding<Texture2d*>(binding, texture2d));
}
void MaterialProperties::InitDescriptorSets()
{
	CreateDescriptorSets();
	for (uint32_t frameIndex = 0; frameIndex < context->framesInFlight; frameIndex++)
	{
		for (auto& [_, resourceBinding] : uniformBufferMaps[frameIndex])
			UpdateDescriptorSet(frameIndex, resourceBinding);
		for (auto& [_, resourceBinding] : samplerMaps[frameIndex])
			UpdateDescriptorSet(frameIndex, resourceBinding);
		for (auto& [_, resourceBinding] : texture2dMaps[frameIndex])
			UpdateDescriptorSet(frameIndex, resourceBinding);
	}
}



// Descriptor set management:
void MaterialProperties::CreateDescriptorSets()
{
	std::vector<VkDescriptorSetLayout> layouts(context->framesInFlight, pipeline->descriptorSetLayout);	// same layout for all frames

	VkDescriptorSetAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
	allocInfo.descriptorPool = context->DescriptorPool();
	allocInfo.descriptorSetCount = context->framesInFlight;
	allocInfo.pSetLayouts = layouts.data();

	descriptorSets.resize(context->framesInFlight);
	VKA(vkAllocateDescriptorSets(context->logicalDevice->device, &allocInfo, descriptorSets.data()));
}
void MaterialProperties::UpdateDescriptorSet(uint32_t frameIndex, ResourceBinding<VulkanUniformBuffer> samplerResourceBinding)
{
	VkDescriptorBufferInfo bufferInfo = {};
	bufferInfo.buffer = samplerResourceBinding.resource.buffer->buffer;
	bufferInfo.offset = 0;
	bufferInfo.range = samplerResourceBinding.resource.size;

	VkWriteDescriptorSet descriptorWrite = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
	descriptorWrite.dstSet = descriptorSets[frameIndex];
	descriptorWrite.dstBinding = samplerResourceBinding.binding;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.pBufferInfo = &bufferInfo;
	descriptorWrite.pImageInfo = nullptr;
	descriptorWrite.pTexelBufferView = nullptr;

	vkUpdateDescriptorSets(context->LogicalDevice(), 1, &descriptorWrite, 0, nullptr);
}
void MaterialProperties::UpdateDescriptorSet(uint32_t frameIndex, ResourceBinding<VulkanSampler*> samplerResourceBinding)
{
	VkDescriptorImageInfo samplerInfo = {};
	samplerInfo.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	samplerInfo.sampler = samplerResourceBinding.resource->sampler;

	VkWriteDescriptorSet descriptorWrite = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
	descriptorWrite.dstSet = descriptorSets[frameIndex];
	descriptorWrite.dstBinding = samplerResourceBinding.binding;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.pBufferInfo = nullptr;
	descriptorWrite.pImageInfo = &samplerInfo;
	descriptorWrite.pTexelBufferView = nullptr;

	vkUpdateDescriptorSets(context->LogicalDevice(), 1, &descriptorWrite, 0, nullptr);
}
void MaterialProperties::UpdateDescriptorSet(uint32_t frameIndex, ResourceBinding<Texture2d*> texture2dResourceBinding)
{
	VkDescriptorImageInfo imageInfo = {};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	//imageInfo.imageLayout = texture2dResourceBinding.resource->image->GetLayout();	// maybe use this? currently throws validation errors.
	imageInfo.imageView = texture2dResourceBinding.resource->image->imageView;

	VkWriteDescriptorSet descriptorWrite = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
	descriptorWrite.dstSet = descriptorSets[frameIndex];
	descriptorWrite.dstBinding = texture2dResourceBinding.binding;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.pBufferInfo = nullptr;
	descriptorWrite.pImageInfo = &imageInfo;
	descriptorWrite.pTexelBufferView = nullptr;

	vkUpdateDescriptorSets(context->LogicalDevice(), 1, &descriptorWrite, 0, nullptr);
}