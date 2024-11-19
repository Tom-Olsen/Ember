#include "materialProperties.h"
#include "vulkanMacros.h"
#include "samplerManager.h"
#include "textureManager.h"



// Consructor:
MaterialProperties::MaterialProperties(Material* material)
{
	this->material = material;
	this->context = material->context;

	// Create resource bindings for each frameInFlight:
	uniformBufferMaps = std::vector<std::unordered_map<std::string, ResourceBinding<VulkanUniformBuffer>>>(context->framesInFlight);
	samplerMaps = std::vector<std::unordered_map<std::string, ResourceBinding<VulkanSampler*>>>(context->framesInFlight);
	texture2dMaps = std::vector<std::unordered_map<std::string, ResourceBinding<Texture2d*>>>(context->framesInFlight);

	for (uint32_t frameIndex = 0; frameIndex < context->framesInFlight; frameIndex++)
	{
		for (uint32_t i = 0; i < material->GetBindingCount(); i++)
		{
			VkDescriptorType type = material->GetBindingType(i);
			if (type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
				InitUniformBufferResourceBinding(material->GetBindingName(i), material->GetBindingIndex(i), frameIndex);
			else if (type == VK_DESCRIPTOR_TYPE_SAMPLER)
				InitSamplerResourceBinding(material->GetBindingName(i), material->GetBindingIndex(i), SamplerManager::GetSampler("colorSampler"), frameIndex);
			else if (type == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE)
				InitTexture2dResourceBinding(material->GetBindingName(i), material->GetBindingIndex(i), TextureManager::GetTexture2d("white"), frameIndex);
		}
	}
	InitDescriptorSets();
}



// Destructor:
MaterialProperties::~MaterialProperties()
{

}



// Public methods:
void MaterialProperties::UpdateUniformBuffers(uint32_t frameIndex)
{
	for (auto& [name, resourceBinding] : uniformBufferMaps[frameIndex])
	{
		if (updateUniformBuffer[name])
		{
			resourceBinding.resource.UpdateBuffer();
			updateUniformBuffer[name] = false;
		}
	}
}




// Setters:
template<typename T>
void MaterialProperties::SetValue(const std::string& blockName, const std::string& memberName, const T& value)
{
	// If cbuffer with 'blockName' doesnt exist, skip:
	auto it = uniformBufferMaps[context->frameIndex].find(blockName);
	if (it == uniformBufferMaps[context->frameIndex].end())
		return;

	// T = bool needs special handling:
	if constexpr (std::is_same<T, bool>::value)
		it->second.resource.SetValue(memberName, static_cast<int>(value));
	else
		it->second.resource.SetValue(memberName, value);
	updateUniformBuffer[blockName] = true;
}
template<typename T>
void MaterialProperties::SetValue(const std::string& blockName, const std::string& memberName, uint32_t arrayindex, const T& value)
{
	// If cbuffer with 'blockName' doesnt exist, skip:
	auto it = uniformBufferMaps[context->frameIndex].find(blockName);
	if (it == uniformBufferMaps[context->frameIndex].end())
		return;

	// check if type is bool:
	if constexpr (std::is_same<T, bool>::value)
		it->second.resource.SetValue(memberName, arrayindex, static_cast<int>(value));
	else
		it->second.resource.SetValue(memberName, arrayindex, value);
	updateUniformBuffer[blockName] = true;
}
void MaterialProperties::SetSampler(const std::string& name, VulkanSampler* sampler)
{
	// If sampler with 'name' doesnt exist, skip:
	auto it = samplerMaps[context->frameIndex].find(name);
	if (it == samplerMaps[context->frameIndex].end() || it->second.resource == sampler)
		return;

	it->second.resource = sampler;
	UpdateDescriptorSet(context->frameIndex, it->second);
}
void MaterialProperties::SetSamplerForAllFrames(const std::string& name, VulkanSampler* sampler)
{
	for (uint32_t frameIndex = 0; frameIndex < context->framesInFlight; frameIndex++)
	{
		// If sampler with 'name' doesnt exist, skip:
		auto it = samplerMaps[frameIndex].find(name);
		if (it == samplerMaps[frameIndex].end() || it->second.resource == sampler)
			return;

		it->second.resource = sampler;
		VKA(vkDeviceWaitIdle(context->LogicalDevice()));
		UpdateDescriptorSet(frameIndex, it->second);
	}
}
void MaterialProperties::SetTexture2d(const std::string& name, Texture2d* texture)
{
	// If texture with 'name' doesnt exist, skip:
	auto it = texture2dMaps[context->frameIndex].find(name);
	if (it == texture2dMaps[context->frameIndex].end() || it->second.resource == texture)
		return;

	it->second.resource = texture;
	UpdateDescriptorSet(context->frameIndex, it->second);
}
void MaterialProperties::SetTexture2dForAllFrames(const std::string& name, Texture2d* texture)
{
	for (uint32_t frameIndex = 0; frameIndex < context->framesInFlight; frameIndex++)
	{
		// If texture with 'name' doesnt exist, skip:
		auto it = texture2dMaps[frameIndex].find(name);
		if (it == texture2dMaps[frameIndex].end() || it->second.resource == texture)
			return;

		it->second.resource = texture;
		VKA(vkDeviceWaitIdle(context->LogicalDevice()));
		UpdateDescriptorSet(frameIndex, it->second);
	}
}



// Debugging:
void MaterialProperties::PrintMaps() const
{
	for (uint32_t frameIndex = 0; frameIndex < context->framesInFlight; frameIndex++)
	{
		LOG_INFO("UniformBufferMaps[{}]:", frameIndex);
		for (const auto& [name, resourceBinding] : uniformBufferMaps[frameIndex])
			LOG_TRACE("binding: {}, bindingName: {}", resourceBinding.binding, name);

		LOG_INFO("SamplerMaps[{}]:", frameIndex);
		for (const auto& [name, resourceBinding] : samplerMaps[frameIndex])
			LOG_TRACE("binding: {}, bindingName: {}, samplerName: {}", resourceBinding.binding, name, resourceBinding.resource->name);

		LOG_INFO("Texture2dMaps[{}]:", frameIndex);
		for (const auto& [name, resourceBinding] : texture2dMaps[frameIndex])
			LOG_TRACE("binding: {}, bindingName: {}, textureName: {}", resourceBinding.binding, name, resourceBinding.resource->name);

		LOG_TRACE("\n");
	}
}



// Private methods:
// Initializers:
void MaterialProperties::InitUniformBufferResourceBinding(std::string name, uint32_t binding, uint32_t frameIndex)
{
	auto it = uniformBufferMaps[frameIndex].find(name);
	if (it == uniformBufferMaps[frameIndex].end())
	{
		VulkanUniformBuffer uniformBuffer(context, material->GetUniformBufferBlock(name));
		uniformBufferMaps[frameIndex].emplace(name, ResourceBinding<VulkanUniformBuffer>(uniformBuffer, binding));
		updateUniformBuffer.emplace(name, true);
	}
}
void MaterialProperties::InitSamplerResourceBinding(std::string name, uint32_t binding, VulkanSampler* sampler, uint32_t frameIndex)
{
	auto it = samplerMaps[frameIndex].find(name);
	if (it == samplerMaps[frameIndex].end())
		samplerMaps[frameIndex].emplace(name, ResourceBinding<VulkanSampler*>(sampler, binding));
}
void MaterialProperties::InitTexture2dResourceBinding(std::string name, uint32_t binding, Texture2d* texture2d, uint32_t frameIndex)
{
	auto it = texture2dMaps[frameIndex].find(name);
	if (it == texture2dMaps[frameIndex].end())
		texture2dMaps[frameIndex].emplace(name, ResourceBinding<Texture2d*>(texture2d, binding));
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
	std::vector<VkDescriptorSetLayout> layouts(context->framesInFlight, material->pipeline->descriptorSetLayout);	// same layout for all frames

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
	bufferInfo.range = samplerResourceBinding.resource.GetSize();

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



// Explicit template instantiation:
template void MaterialProperties::SetValue<int>(const std::string& blockName, const std::string& memberName, const int& value);
template void MaterialProperties::SetValue<bool>(const std::string& blockName, const std::string& memberName, const bool& value);
template void MaterialProperties::SetValue<float>(const std::string& blockName, const std::string& memberName, const float& value);
template void MaterialProperties::SetValue<Float2>(const std::string& blockName, const std::string& memberName, const Float2& value);
template void MaterialProperties::SetValue<Float3>(const std::string& blockName, const std::string& memberName, const Float3& value);
template void MaterialProperties::SetValue<Float4>(const std::string& blockName, const std::string& memberName, const Float4& value);
template void MaterialProperties::SetValue<Float4x4>(const std::string& blockName, const std::string& memberName, const Float4x4& value);

template void MaterialProperties::SetValue<int>(const std::string& blockName, const std::string& memberName, uint32_t arrayindex, const int& value);
template void MaterialProperties::SetValue<bool>(const std::string& blockName, const std::string& memberName, uint32_t arrayindex, const bool& value);
template void MaterialProperties::SetValue<float>(const std::string& blockName, const std::string& memberName, uint32_t arrayindex, const float& value);
template void MaterialProperties::SetValue<Float2>(const std::string& blockName, const std::string& memberName, uint32_t arrayindex, const Float2& value);
template void MaterialProperties::SetValue<Float3>(const std::string& blockName, const std::string& memberName, uint32_t arrayindex, const Float3& value);
template void MaterialProperties::SetValue<Float4>(const std::string& blockName, const std::string& memberName, uint32_t arrayindex, const Float4& value);
template void MaterialProperties::SetValue<Float4x4>(const std::string& blockName, const std::string& memberName, uint32_t arrayindex, const Float4x4& value);