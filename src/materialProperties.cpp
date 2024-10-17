#include "materialProperties.h"



// Consructor:
MaterialProperties::MaterialProperties()
{
	this->context = nullptr;
	this->pipeline = nullptr;
}
MaterialProperties::MaterialProperties(VulkanContext* context, VulkanPipeline* pipeline)
{
	this->context = context;
	this->pipeline = pipeline;
}



// Destructor:
MaterialProperties::~MaterialProperties()
{

}



// Public initializers:
void MaterialProperties::InitUniformObjectResourceBinding(std::string name, uint32_t binding, const UniformObject& uniformObject)
{
	auto it = uniformBufferMap.find(name);
	if (it == uniformBufferMap.end())
	{
		VulkanUniformBuffer uniformBuffer(context, sizeof(UniformObject));
		uniformBuffer.UpdateBuffer(uniformObject);
		std::vector<std::string> frameNames(context->framesInFlight);
		for (uint32_t i = 0; i < context->framesInFlight; i++)
			frameNames[i] = std::to_string(i);	// name for uniform buffers not needed, so just index them.
		uniformBufferMap.emplace(name, ResourceBinding<VulkanUniformBuffer>(binding, uniformBuffer, frameNames));
	}
}
void MaterialProperties::InitSamplerResourceBinding(std::string name, uint32_t binding, VulkanSampler* sampler)
{
	auto it = samplerMap.find(name);
	if (it == samplerMap.end())
	{
		std::vector<std::string> frameNames(context->framesInFlight);
		for (uint32_t i = 0; i < context->framesInFlight; i++)
			frameNames[i] = sampler->name;
		samplerMap.emplace(name, ResourceBinding<VulkanSampler*>(binding, sampler, frameNames));
	}
}
void MaterialProperties::InitTexture2dResourceBinding(std::string name, uint32_t binding, Texture2d* texture2d)
{
	auto it = texture2dMap.find(name);
	if (it == texture2dMap.end())
	{
		std::vector<std::string> frameNames(context->framesInFlight);
		for (uint32_t i = 0; i < context->framesInFlight; i++)
			frameNames[i] = texture2d->name;
		texture2dMap.emplace(name, ResourceBinding<Texture2d*>(binding, texture2d, frameNames));
	}
}
void MaterialProperties::InitDescriptorSets()
{
	CreateDescriptorSets();
	for (uint32_t frameIndex = 0; frameIndex < context->framesInFlight; frameIndex++)
		UpdateAllDescriptorSets(frameIndex);
}



// Setters:
void MaterialProperties::SetContext(VulkanContext* context)
{
	if (this->context == nullptr)
		this->context = context;
}
void MaterialProperties::SetPipeline(VulkanPipeline* pipeline)
{
	if (this->pipeline == nullptr)
		this->pipeline = pipeline;
}
void MaterialProperties::SetUniformBuffer(const std::string& name, const UniformObject& data)
{
	// if key exists, replace its value
	auto it = uniformBufferMap.find(name);
	if (it != uniformBufferMap.end())
		it->second.resource.UpdateBuffer(data);
}
void MaterialProperties::SetSampler(const std::string& name, VulkanSampler* sampler)
{
	// if key exists, replace its value
	auto it = samplerMap.find(name);
	if (it != samplerMap.end())
		if (it->second.frameNames[context->frameIndex] != sampler->name)
		{
			it->second.resource = sampler;
			it->second.frameNames[context->frameIndex] = sampler->name;
			UpdateSamplerDescriptorSets(context->frameIndex);
		}
}
void MaterialProperties::SetTexture2d(const std::string& name, Texture2d* texture)
{
	// if key exists, replace its value
	auto it = texture2dMap.find(name);
	if (it != texture2dMap.end())
		if (it->second.frameNames[context->frameIndex] != texture->name)
		{
			it->second.resource = texture;
			it->second.frameNames[context->frameIndex] = texture->name;
			UpdateTexture2dDescriptorSets(context->frameIndex);
		}
}



// Getters:
MaterialProperties MaterialProperties::GetCopy()
{
	MaterialProperties copy(context, pipeline);

	// Copy bindings:
	for (const auto& pair : uniformBufferMap)
		copy.InitUniformObjectResourceBinding(pair.first, pair.second.binding, UniformObject());
	for (const auto& pair : samplerMap)
		copy.InitSamplerResourceBinding(pair.first, pair.second.binding, pair.second.resource);
	for (const auto& pair : texture2dMap)
		copy.InitTexture2dResourceBinding(pair.first, pair.second.binding, pair.second.resource);

	// Initialize descriptor sets:
	copy.InitDescriptorSets();

	return copy;
}



// Private methods:
void MaterialProperties::CreateDescriptorSets()
{
	std::vector<VkDescriptorSetLayout> layouts;
	layouts.reserve(context->framesInFlight);
	for (uint32_t i = 0; i < context->framesInFlight; i++)	// same layout for all frames
		layouts.push_back(pipeline->descriptorSetLayout);

	VkDescriptorSetAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
	allocInfo.descriptorPool = context->DescriptorPool();
	allocInfo.descriptorSetCount = context->framesInFlight;
	allocInfo.pSetLayouts = layouts.data();

	descriptorSets.resize(context->framesInFlight);
	VKA(vkAllocateDescriptorSets(context->logicalDevice->device, &allocInfo, descriptorSets.data()));
}
void MaterialProperties::UpdateAllDescriptorSets(uint32_t frameIndex)
{
	size_t count = std::max({ uniformBufferMap.size(), samplerMap.size(), texture2dMap.size() });
	descriptorWrites.reserve(count);

	UpdateUniformBufferDescriptorSets(frameIndex);
	UpdateSamplerDescriptorSets(frameIndex);
	UpdateTexture2dDescriptorSets(frameIndex);
}
void MaterialProperties::UpdateUniformBufferDescriptorSets(uint32_t frameIndex)
{
	descriptorWrites.clear();
	for (const auto& pair : uniformBufferMap)
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
	LOG_INFO("UpdateUniformBufferDescriptorSets");
}
void MaterialProperties::UpdateSamplerDescriptorSets(uint32_t frameIndex)
{
	descriptorWrites.clear();
	for (const auto& pair : samplerMap)
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
	LOG_INFO("UpdateSamplerDescriptorSets");
}
void MaterialProperties::UpdateTexture2dDescriptorSets(uint32_t frameIndex)
{
	descriptorWrites.clear();
	for (const auto& pair : texture2dMap)
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
	LOG_INFO("UpdateTexture2dDescriptorSets");
}