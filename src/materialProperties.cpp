#include "materialProperties.h"



// Consructor:
MaterialProperties::MaterialProperties(VulkanContext* context)
{
	this->context = context;
	this->pipeline = nullptr;
}



// Destructor:
MaterialProperties::~MaterialProperties()
{

}



// Public initializers:
void MaterialProperties::InitUniformResourceBinding(std::string name, uint32_t binding, const RenderMatrizes& renderMatrizes)
{
	auto it = uniformBufferMap.find(name);
	if (it == uniformBufferMap.end())
	{
		VulkanUniformBuffer uniformBuffer(context, sizeof(RenderMatrizes));
		uniformBuffer.UpdateBuffer(renderMatrizes);
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
void MaterialProperties::SetPipeline(VulkanPipeline* pipeline)
{
	if (this->pipeline == nullptr)
		this->pipeline = pipeline;
}
void MaterialProperties::SetUniform(const std::string& name, const RenderMatrizes& renderMatrizes)
{
	// if key exists, replace its value
	auto it = uniformBufferMap.find(name);
	if (it != uniformBufferMap.end())
		it->second.resource.UpdateBuffer(renderMatrizes);
	else
		LOG_WARN("Uniform '{}' not found in uniformBufferMap!", name);
}
void MaterialProperties::SetSampler(const std::string& name, VulkanSampler* sampler)
{
	// if key exists, replace its value
	auto it = samplerMap.find(name);
	if (it != samplerMap.end())
	{
		if (it->second.frameNames[context->frameIndex] != sampler->name)
		{
			it->second.resource = sampler;
			it->second.frameNames[context->frameIndex] = sampler->name;
			UpdateSamplerDescriptorSets(context->frameIndex);
		}
	}
	else
		LOG_WARN("Sampler '{}' not found in samplerMap!", name);
}
void MaterialProperties::SetSamplerForAllFrames(const std::string& name, VulkanSampler* sampler)
{
	// if key exists, replace its value
	auto it = samplerMap.find(name);
	if (it != samplerMap.end())
	{
		VKA(vkDeviceWaitIdle(context->LogicalDevice()));
		for (uint32_t i = 0; i < context->framesInFlight; i++)
		{
			it->second.resource = sampler;
			it->second.frameNames[i] = sampler->name;
			UpdateSamplerDescriptorSets(i);
		}
	}
	else
		LOG_WARN("Sampler '{}' not found in samplerMap!", name);
}
void MaterialProperties::SetTexture2d(const std::string& name, Texture2d* texture)
{
	// if key exists, replace its value
	auto it = texture2dMap.find(name);
	if (it != texture2dMap.end())
	{
		if (it->second.frameNames[context->frameIndex] != texture->name)
		{
			it->second.resource = texture;
			it->second.frameNames[context->frameIndex] = texture->name;
			UpdateTexture2dDescriptorSets(context->frameIndex);
		}
	}
	else
		LOG_WARN("Texture2d '{}' not found in texture2dMap!", name);
}
void MaterialProperties::SetTexture2dForAllFrames(const std::string& name, Texture2d* texture)
{
	// if key exists, replace its value
	auto it = texture2dMap.find(name);
	if (it != texture2dMap.end())
	{
		VKA(vkDeviceWaitIdle(context->LogicalDevice()));
		for (uint32_t i = 0; i < context->framesInFlight; i++)
		{
			it->second.resource = texture;
			it->second.frameNames[i] = texture->name;
			UpdateTexture2dDescriptorSets(i);
		}
	}
	else
		LOG_WARN("Texture2d '{}' not found in texture2dMap!", name);
}



// Getters:
MaterialProperties* MaterialProperties::GetCopy()
{
	MaterialProperties* copy = new MaterialProperties(context);

	// Copy bindings:
	for (const auto& pair : uniformBufferMap)
		copy->InitUniformResourceBinding(pair.first, pair.second.binding, RenderMatrizes());
	for (const auto& pair : samplerMap)
		copy->InitSamplerResourceBinding(pair.first, pair.second.binding, pair.second.resource);
	for (const auto& pair : texture2dMap)
		copy->InitTexture2dResourceBinding(pair.first, pair.second.binding, pair.second.resource);

	// Initialize descriptor sets:
	copy->SetPipeline(pipeline);
	copy->InitDescriptorSets();

	return copy;
}



// Debugging:
void MaterialProperties::PrintMaps() const
{
	LOG_TRACE("UniformBufferMap:");
	for (const auto& pair : uniformBufferMap)
		LOG_TRACE("name: {}", pair.first);
	LOG_TRACE("SamplerMap:");
	for (const auto& pair : samplerMap)
		LOG_TRACE("name: {}", pair.first);
	LOG_TRACE("Texture2dMap:");
	for (const auto& pair : texture2dMap)
		LOG_TRACE("name: {}", pair.first);
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

	UpdateUniformDescriptorSets(frameIndex);
	UpdateSamplerDescriptorSets(frameIndex);
	UpdateTexture2dDescriptorSets(frameIndex);
}
void MaterialProperties::UpdateUniformDescriptorSets(uint32_t frameIndex)
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
}