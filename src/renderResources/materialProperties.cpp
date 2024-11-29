#include "materialProperties.h"
#include "vulkanMacros.h"
#include "samplerManager.h"
#include "textureManager.h"
#include "logger.h"



// Consructor:
MaterialProperties::MaterialProperties(Material* material)
{
	this->material = material;
	this->context = material->GetContext();

	// Create resource bindings for each frameInFlight:
	uniformBufferMaps = std::vector<std::unordered_map<std::string, ResourceBinding<VulkanUniformBuffer>>>(context->framesInFlight);
	samplerMaps = std::vector<std::unordered_map<std::string, ResourceBinding<VulkanSampler*>>>(context->framesInFlight);
	texture2dMaps = std::vector<std::unordered_map<std::string, ResourceBinding<Texture2d*>>>(context->framesInFlight);
	updateUniformBuffer = std::vector<std::unordered_map<std::string, bool>>(context->framesInFlight);

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
	InitStagingMaps();
	InitDescriptorSets();

	// Set default values:
	SetValue("SurfaceProperties", "diffuseColor", Float4(1.0f, 1.0f, 1.0f, 1.0f));
	SetValue("SurfaceProperties", "roughness", 0.5f);
	SetValue("SurfaceProperties", "reflectivity", Float3(0.4f));
	SetValue("SurfaceProperties", "metallic", 0);
}



// Destructor:
MaterialProperties::~MaterialProperties()
{

}



// Public methods:
void MaterialProperties::UpdateShaderData()
{
	// Stream uniform buffer data from host memory to GPU only for current frameIndex:
	for (auto& [name, resourceBinding] : uniformBufferMaps[context->frameIndex])
	{
		if (updateUniformBuffer[context->frameIndex].at(name))
		{
			resourceBinding.resource.UpdateBuffer();
			updateUniformBuffer[context->frameIndex].at(name) = false;
		}
	}

	// Change the pointer the descriptor set points at to the new sampler:
	for (auto& [name, resourceBinding] : samplerMaps[context->frameIndex])
	{
		if (resourceBinding.resource != samplerStagingMap.at(name))
		{
			resourceBinding.resource = samplerStagingMap.at(name);
			UpdateDescriptorSet(context->frameIndex, resourceBinding);
		}
	}


	// Change the pointer the descriptor set points at to the new texture2d:
	for (auto& [name, resourceBinding] : texture2dMaps[context->frameIndex])
	{
		if (resourceBinding.resource != texture2dStagingMap.at(name))
		{
			resourceBinding.resource = texture2dStagingMap.at(name);
			UpdateDescriptorSet(context->frameIndex, resourceBinding);
		}
	}
}




// Uniform buffer setters:
template<typename T>
void MaterialProperties::SetValue(const std::string& blockName, const std::string& memberName, const T& value)
{
	auto it = uniformBufferMaps[0].find(blockName);
	if (it != uniformBufferMaps[0].end())
	{// blockname exists => set value for all frames:
		for (uint32_t frameIndex = 0; frameIndex < context->framesInFlight; frameIndex++)
		{
			uniformBufferMaps[frameIndex].at(blockName).resource.SetValue(memberName, value);
			updateUniformBuffer[frameIndex].at(blockName) = true;
		}
	}
}
template<typename T>
void MaterialProperties::SetValue(const std::string& blockName, const std::string& arrayName, uint32_t arrayIndex, const T& value)
{
	auto it = uniformBufferMaps[0].find(blockName);
	if (it != uniformBufferMaps[0].end())
	{// blockname exists => set value for all frames:
		for (uint32_t frameIndex = 0; frameIndex < context->framesInFlight; frameIndex++)
		{
			uniformBufferMaps[frameIndex].at(blockName).resource.SetValue(arrayName, arrayIndex, value);
			updateUniformBuffer[frameIndex].at(blockName) = true;
		}
	}
}
template<typename T>
void MaterialProperties::SetValue(const std::string& blockName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const T& value)
{
	auto it = uniformBufferMaps[0].find(blockName);
	if (it != uniformBufferMaps[0].end())
	{// blockname exists => set value for all frames:
		for (uint32_t frameIndex = 0; frameIndex < context->framesInFlight; frameIndex++)
		{
			uniformBufferMaps[frameIndex].at(blockName).resource.SetValue(arrayName, arrayIndex, memberName, value);
			updateUniformBuffer[frameIndex].at(blockName) = true;
		}
	}
}
template<typename T>
void MaterialProperties::SetValue(const std::string& blockName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const T& value)
{
	auto it = uniformBufferMaps[0].find(blockName);
	if (it != uniformBufferMaps[0].end())
	{// blockname exists => set value for all frames:
		for (uint32_t frameIndex = 0; frameIndex < context->framesInFlight; frameIndex++)
		{
			uniformBufferMaps[frameIndex].at(blockName).resource.SetValue(arrayName, arrayIndex, subArrayName, subArrayIndex, value);
			updateUniformBuffer[frameIndex].at(blockName) = true;
		}
	}
}


// Sampler setters:
void MaterialProperties::SetSampler(const std::string& name, VulkanSampler* sampler)
{
	// If sampler with 'name' doesnt exist, skip:
	auto it = samplerStagingMap.find(name);
	if (it == samplerStagingMap.end() || it->second == sampler)
		return;

	it->second = sampler;
}



// Texture2d setters:
void MaterialProperties::SetTexture2d(const std::string& name, Texture2d* texture)
{
	// If texture with 'name' doesnt exist, skip:
	auto it = texture2dStagingMap.find(name);
	if (it == texture2dStagingMap.end() || it->second == texture)
		return;

	it->second = texture;
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

		for (uint32_t frameIndex = 0; frameIndex < context->framesInFlight; frameIndex++)
			updateUniformBuffer[frameIndex].emplace(name, true);
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
void MaterialProperties::InitStagingMaps()
{
	for (auto& [name, resourceBinding] : samplerMaps[0])
		samplerStagingMap.emplace(name, resourceBinding.resource);
	for (auto& [name, resourceBinding] : texture2dMaps[0])
		texture2dStagingMap.emplace(name, resourceBinding.resource);
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
void MaterialProperties::UpdateDescriptorSet(uint32_t frameIndex, ResourceBinding<VulkanUniformBuffer> uniformBufferResourceBinding)
{
	VkDescriptorBufferInfo bufferInfo = {};
	bufferInfo.buffer = uniformBufferResourceBinding.resource.buffer->buffer;
	bufferInfo.offset = 0;
	bufferInfo.range = uniformBufferResourceBinding.resource.GetSize();

	VkWriteDescriptorSet descriptorWrite = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
	descriptorWrite.dstSet = descriptorSets[frameIndex];
	descriptorWrite.dstBinding = uniformBufferResourceBinding.binding;
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

template void MaterialProperties::SetValue<int>(const std::string& blockName, const std::string& arrayName, uint32_t arrayindex, const int& value);
template void MaterialProperties::SetValue<bool>(const std::string& blockName, const std::string& arrayName, uint32_t arrayindex, const bool& value);
template void MaterialProperties::SetValue<float>(const std::string& blockName, const std::string& arrayName, uint32_t arrayindex, const float& value);
template void MaterialProperties::SetValue<Float2>(const std::string& blockName, const std::string& arrayName, uint32_t arrayindex, const Float2& value);
template void MaterialProperties::SetValue<Float3>(const std::string& blockName, const std::string& arrayName, uint32_t arrayindex, const Float3& value);
template void MaterialProperties::SetValue<Float4>(const std::string& blockName, const std::string& arrayName, uint32_t arrayindex, const Float4& value);
template void MaterialProperties::SetValue<Float4x4>(const std::string& blockName, const std::string& arrayName, uint32_t arrayindex, const Float4x4& value);

template void MaterialProperties::SetValue<int>(const std::string& blockName, const std::string& arrayName, uint32_t arrayindex, const std::string& memberName, const int& value);
template void MaterialProperties::SetValue<bool>(const std::string& blockName, const std::string& arrayName, uint32_t arrayindex, const std::string& memberName, const bool& value);
template void MaterialProperties::SetValue<float>(const std::string& blockName, const std::string& arrayName, uint32_t arrayindex, const std::string& memberName, const float& value);
template void MaterialProperties::SetValue<Float2>(const std::string& blockName, const std::string& arrayName, uint32_t arrayindex, const std::string& memberName, const Float2& value);
template void MaterialProperties::SetValue<Float3>(const std::string& blockName, const std::string& arrayName, uint32_t arrayindex, const std::string& memberName, const Float3& value);
template void MaterialProperties::SetValue<Float4>(const std::string& blockName, const std::string& arrayName, uint32_t arrayindex, const std::string& memberName, const Float4& value);
template void MaterialProperties::SetValue<Float4x4>(const std::string& blockName, const std::string& arrayName, uint32_t arrayindex, const std::string& memberName, const Float4x4& value);

template void MaterialProperties::SetValue<int>(const std::string& blockName, const std::string& arrayName, uint32_t arrayindex, const std::string& subArrayName, uint32_t subArrayIndex, const int& value);
template void MaterialProperties::SetValue<bool>(const std::string& blockName, const std::string& arrayName, uint32_t arrayindex, const std::string& subArrayName, uint32_t subArrayIndex, const bool& value);
template void MaterialProperties::SetValue<float>(const std::string& blockName, const std::string& arrayName, uint32_t arrayindex, const std::string& subArrayName, uint32_t subArrayIndex, const float& value);
template void MaterialProperties::SetValue<Float2>(const std::string& blockName, const std::string& arrayName, uint32_t arrayindex, const std::string& subArrayName, uint32_t subArrayIndex, const Float2& value);
template void MaterialProperties::SetValue<Float3>(const std::string& blockName, const std::string& arrayName, uint32_t arrayindex, const std::string& subArrayName, uint32_t subArrayIndex, const Float3& value);
template void MaterialProperties::SetValue<Float4>(const std::string& blockName, const std::string& arrayName, uint32_t arrayindex, const std::string& subArrayName, uint32_t subArrayIndex, const Float4& value);
template void MaterialProperties::SetValue<Float4x4>(const std::string& blockName, const std::string& arrayName, uint32_t arrayindex, const std::string& subArrayName, uint32_t subArrayIndex, const Float4x4& value);