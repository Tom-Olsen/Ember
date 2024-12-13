#include "materialProperties.h"
#include "logger.h"
#include "material.h"
#include "mathf.h"
#include "pipeline.h"
#include "renderPassManager.h"
#include "sampler.h"
#include "samplerManager.h"
#include "shadowRenderPass.h"
#include "texture2d.h"
#include "textureManager.h"
#include "uniformBuffer.h"
#include "vmaBuffer.h"
#include "vmaImage.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"



// Consructor/Destructor:
MaterialProperties::MaterialProperties(Material* pMaterial)
{
	m_pMaterial = pMaterial;
	m_pContext = pMaterial->GetContext();

	// Create resource bindings for each frameInFlight:
	m_uniformBufferMaps = std::vector<std::unordered_map<std::string, ResourceBinding<std::shared_ptr<UniformBuffer>>>>(m_pContext->framesInFlight);
	m_samplerMaps = std::vector<std::unordered_map<std::string, ResourceBinding<Sampler*>>>(m_pContext->framesInFlight);
	m_texture2dMaps = std::vector<std::unordered_map<std::string, ResourceBinding<Texture2d*>>>(m_pContext->framesInFlight);
	m_updateUniformBuffer = std::vector<std::unordered_map<std::string, bool>>(m_pContext->framesInFlight);

	for (uint32_t frameIndex = 0; frameIndex < m_pContext->framesInFlight; frameIndex++)
	{
		for (uint32_t i = 0; i < m_pMaterial->GetBindingCount(); i++)
		{
			VkDescriptorType type = m_pMaterial->GetBindingType(i);
			if (type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
				InitUniformBufferResourceBinding(m_pMaterial->GetBindingName(i), m_pMaterial->GetBindingIndex(i), frameIndex);
			else if (type == VK_DESCRIPTOR_TYPE_SAMPLER)
				InitSamplerResourceBinding(m_pMaterial->GetBindingName(i), m_pMaterial->GetBindingIndex(i), SamplerManager::GetSampler("colorSampler"), frameIndex);
			else if (type == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE)
				InitTexture2dResourceBinding(m_pMaterial->GetBindingName(i), m_pMaterial->GetBindingIndex(i), TextureManager::GetTexture2d("white"), frameIndex);
		}
	}
	InitStagingMaps();
	InitDescriptorSets();

	// Set default values:
	ShadowRenderPass* pShadowRenderPass = dynamic_cast<ShadowRenderPass*>(RenderPassManager::GetRenderPass("shadowRenderPass"));
	SetSampler("shadowSampler", SamplerManager::GetSampler("shadowSampler"));
	SetTexture2d("shadowMaps", pShadowRenderPass->GetShadowMaps());
	SetTexture2d("normalMap", TextureManager::GetTexture2d("defaultNormalMap"));
	SetValue("SurfaceProperties", "scaleOffset", Float4(1.0f, 1.0f, 1.0f, 1.0f));
	SetValue("SurfaceProperties", "diffuseColor", Float4(1.0f, 1.0f, 1.0f, 1.0f));
	SetValue("SurfaceProperties", "roughness", 0.5f);
	SetValue("SurfaceProperties", "reflectivity", Float3(0.4f));
	SetValue("SurfaceProperties", "metallic", 0);
}
MaterialProperties::~MaterialProperties()
{

}



// Public methods:
void MaterialProperties::UpdateShaderData()
{
	// Stream uniform buffer data from host memory to GPU only for current frameIndex:
	for (auto& [name, resourceBinding] : m_uniformBufferMaps[m_pContext->frameIndex])
	{
		if (m_updateUniformBuffer[m_pContext->frameIndex].at(name))
		{
			resourceBinding.resource->UpdateBuffer();
			m_updateUniformBuffer[m_pContext->frameIndex].at(name) = false;
		}
	}

	// Change the pointer the descriptor set points at to the new sampler:
	for (auto& [name, resourceBinding] : m_samplerMaps[m_pContext->frameIndex])
	{
		if (resourceBinding.resource != m_samplerStagingMap.at(name))
		{
			resourceBinding.resource = m_samplerStagingMap.at(name);
			UpdateDescriptorSet(m_pContext->frameIndex, resourceBinding);
		}
	}

	// Change the pointer the descriptor set points at to the new texture2d:
	for (auto& [name, resourceBinding] : m_texture2dMaps[m_pContext->frameIndex])
	{
		if (resourceBinding.resource != m_texture2dStagingMap.at(name))
		{
			resourceBinding.resource = m_texture2dStagingMap.at(name);
			UpdateDescriptorSet(m_pContext->frameIndex, resourceBinding);
		}
	}
}
const std::vector<VkDescriptorSet>& MaterialProperties::GetDescriptorSets() const
{
	return m_descriptorSets;
}




// Uniform Buffer Setters:
template<typename T>
void MaterialProperties::SetValue(const std::string& blockName, const std::string& memberName, const T& value)
{
	auto it = m_uniformBufferMaps[0].find(blockName);
	if (it != m_uniformBufferMaps[0].end())
	{// blockname exists => set value for all frames:
		for (uint32_t frameIndex = 0; frameIndex < m_pContext->framesInFlight; frameIndex++)
		{
			if (m_uniformBufferMaps[frameIndex].at(blockName).resource->SetValue(memberName, value))
				m_updateUniformBuffer[frameIndex].at(blockName) = true;
		}
	}
}
template<typename T>
void MaterialProperties::SetValue(const std::string& blockName, const std::string& arrayName, uint32_t arrayIndex, const T& value)
{
	auto it = m_uniformBufferMaps[0].find(blockName);
	if (it != m_uniformBufferMaps[0].end())
	{// blockname exists => set value for all frames:
		for (uint32_t frameIndex = 0; frameIndex < m_pContext->framesInFlight; frameIndex++)
		{
			if (m_uniformBufferMaps[frameIndex].at(blockName).resource->SetValue(arrayName, arrayIndex, value))
				m_updateUniformBuffer[frameIndex].at(blockName) = true;
		}
	}
}
template<typename T>
void MaterialProperties::SetValue(const std::string& blockName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const T& value)
{
	auto it = m_uniformBufferMaps[0].find(blockName);
	if (it != m_uniformBufferMaps[0].end())
	{// blockname exists => set value for all frames:
		for (uint32_t frameIndex = 0; frameIndex < m_pContext->framesInFlight; frameIndex++)
		{
			if (m_uniformBufferMaps[frameIndex].at(blockName).resource->SetValue(arrayName, arrayIndex, memberName, value))
				m_updateUniformBuffer[frameIndex].at(blockName) = true;
		}
	}
}
template<typename T>
void MaterialProperties::SetValue(const std::string& blockName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const T& value)
{
	auto it = m_uniformBufferMaps[0].find(blockName);
	if (it != m_uniformBufferMaps[0].end())
	{// blockname exists => set value for all frames:
		for (uint32_t frameIndex = 0; frameIndex < m_pContext->framesInFlight; frameIndex++)
		{
			if (m_uniformBufferMaps[frameIndex].at(blockName).resource->SetValue(arrayName, arrayIndex, subArrayName, subArrayIndex, value))
				m_updateUniformBuffer[frameIndex].at(blockName) = true;
		}
	}
}

// Sampler setters:
void MaterialProperties::SetSampler(const std::string& name, Sampler* pSampler)
{
	// If sampler with 'name' doesnt exist, skip:
	auto it = m_samplerStagingMap.find(name);
	if (it == m_samplerStagingMap.end() || it->second == pSampler)
		return;

	it->second = pSampler;
}

// Texture2d setters:
void MaterialProperties::SetTexture2d(const std::string& name, Texture2d* pTexture2d)
{
	// If texture with 'name' doesnt exist, skip:
	auto it = m_texture2dStagingMap.find(name);
	if (it == m_texture2dStagingMap.end() || it->second == pTexture2d)
		return;

	it->second = pTexture2d;
}



// Uniform Buffer Getters:
template<typename T>
T MaterialProperties::GetValue(const std::string& blockName, const std::string& memberName) const
{
	auto it = m_uniformBufferMaps[0].find(blockName);
	if (it != m_uniformBufferMaps[0].end())
		return it->second.resource->GetValue<T>(memberName);
	return T();
}
template<typename T>
T MaterialProperties::GetValue(const std::string& blockName, const std::string& arrayName, uint32_t arrayIndex) const
{
	auto it = m_uniformBufferMaps[0].find(blockName);
	if (it != m_uniformBufferMaps[0].end())
		return it->second.resource->GetValue<T>(arrayName, arrayIndex);
	return T();
}
template<typename T>
T MaterialProperties::GetValue(const std::string& blockName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const
{
	auto it = m_uniformBufferMaps[0].find(blockName);
	if (it != m_uniformBufferMaps[0].end())
		return it->second.resource->GetValue<T>(arrayName, arrayIndex, memberName);
	return T();
}
template<typename T>
T MaterialProperties::GetValue(const std::string& blockName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const
{
	auto it = m_uniformBufferMaps[0].find(blockName);
	if (it != m_uniformBufferMaps[0].end())
		return it->second.resource->GetValue<T>(arrayName, arrayIndex, subArrayName, subArrayIndex);
	return T();
}



// Debugging:
void MaterialProperties::PrintMaps() const
{
	for (uint32_t frameIndex = 0; frameIndex < m_pContext->framesInFlight; frameIndex++)
	{
		LOG_INFO("UniformBufferMaps[{}]:", frameIndex);
		for (const auto& [name, resourceBinding] : m_uniformBufferMaps[frameIndex])
			LOG_TRACE("binding: {}, bindingName: {}", resourceBinding.binding, name);

		LOG_INFO("SamplerMaps[{}]:", frameIndex);
		for (const auto& [name, resourceBinding] : m_samplerMaps[frameIndex])
			LOG_TRACE("binding: {}, bindingName: {}, samplerName: {}", resourceBinding.binding, name, resourceBinding.resource->GetName());

		LOG_INFO("Texture2dMaps[{}]:", frameIndex);
		for (const auto& [name, resourceBinding] : m_texture2dMaps[frameIndex])
			LOG_TRACE("binding: {}, bindingName: {}, textureName: {}", resourceBinding.binding, name, resourceBinding.resource->GetName());

		LOG_TRACE("\n");
	}
}



// Private methods:
// Initializers:
void MaterialProperties::InitUniformBufferResourceBinding(std::string name, uint32_t binding, uint32_t frameIndex)
{
	auto it = m_uniformBufferMaps[frameIndex].find(name);
	if (it == m_uniformBufferMaps[frameIndex].end())
	{
		std::shared_ptr<UniformBuffer> uniformBuffer = std::make_shared<UniformBuffer>(m_pContext, m_pMaterial->GetUniformBufferBlock(name));
		m_uniformBufferMaps[frameIndex].emplace(name, ResourceBinding<std::shared_ptr<UniformBuffer>>(uniformBuffer, binding));

		for (uint32_t frameIndex = 0; frameIndex < m_pContext->framesInFlight; frameIndex++)
			m_updateUniformBuffer[frameIndex].emplace(name, true);
	}
}
void MaterialProperties::InitSamplerResourceBinding(std::string name, uint32_t binding, Sampler* pSampler, uint32_t frameIndex)
{
	auto it = m_samplerMaps[frameIndex].find(name);
	if (it == m_samplerMaps[frameIndex].end())
		m_samplerMaps[frameIndex].emplace(name, ResourceBinding<Sampler*>(pSampler, binding));
}
void MaterialProperties::InitTexture2dResourceBinding(std::string name, uint32_t binding, Texture2d* pTexture2d, uint32_t frameIndex)
{
	auto it = m_texture2dMaps[frameIndex].find(name);
	if (it == m_texture2dMaps[frameIndex].end())
		m_texture2dMaps[frameIndex].emplace(name, ResourceBinding<Texture2d*>(pTexture2d, binding));
}
void MaterialProperties::InitStagingMaps()
{
	for (auto& [name, resourceBinding] : m_samplerMaps[0])
		m_samplerStagingMap.emplace(name, resourceBinding.resource);
	for (auto& [name, resourceBinding] : m_texture2dMaps[0])
		m_texture2dStagingMap.emplace(name, resourceBinding.resource);
}
void MaterialProperties::InitDescriptorSets()
{
	CreateDescriptorSets();
	for (uint32_t frameIndex = 0; frameIndex < m_pContext->framesInFlight; frameIndex++)
	{
		for (auto& [_, resourceBinding] : m_uniformBufferMaps[frameIndex])
			UpdateDescriptorSet(frameIndex, resourceBinding);
		for (auto& [_, resourceBinding] : m_samplerMaps[frameIndex])
			UpdateDescriptorSet(frameIndex, resourceBinding);
		for (auto& [_, resourceBinding] : m_texture2dMaps[frameIndex])
			UpdateDescriptorSet(frameIndex, resourceBinding);
	}
}



// Descriptor Set management:
void MaterialProperties::CreateDescriptorSets()
{
	std::vector<VkDescriptorSetLayout> layouts(m_pContext->framesInFlight, m_pMaterial->GetPipeline()->GetVkDescriptorSetLayout());	// same layout for all frames

	VkDescriptorSetAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
	allocInfo.descriptorPool = m_pContext->GetVkDescriptorPool();
	allocInfo.descriptorSetCount = m_pContext->framesInFlight;
	allocInfo.pSetLayouts = layouts.data();

	m_descriptorSets.resize(m_pContext->framesInFlight);
	VKA(vkAllocateDescriptorSets(m_pContext->pLogicalDevice->GetVkDevice(), &allocInfo, m_descriptorSets.data()));
}
void MaterialProperties::UpdateDescriptorSet(uint32_t frameIndex, ResourceBinding<std::shared_ptr<UniformBuffer>> uniformBufferResourceBinding)
{
	VkDescriptorBufferInfo bufferInfo = {};
	bufferInfo.buffer = uniformBufferResourceBinding.resource->GetVmaBuffer()->GetVkBuffer();
	bufferInfo.offset = 0;
	bufferInfo.range = uniformBufferResourceBinding.resource->GetSize();

	VkWriteDescriptorSet descriptorWrite = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
	descriptorWrite.dstSet = m_descriptorSets[frameIndex];
	descriptorWrite.dstBinding = uniformBufferResourceBinding.binding;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.pBufferInfo = &bufferInfo;
	descriptorWrite.pImageInfo = nullptr;
	descriptorWrite.pTexelBufferView = nullptr;

	vkUpdateDescriptorSets(m_pContext->GetVkDevice(), 1, &descriptorWrite, 0, nullptr);
}
void MaterialProperties::UpdateDescriptorSet(uint32_t frameIndex, ResourceBinding<Sampler*> samplerResourceBinding)
{
	VkDescriptorImageInfo samplerInfo = {};
	samplerInfo.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	samplerInfo.sampler = samplerResourceBinding.resource->GetVkSampler();

	VkWriteDescriptorSet descriptorWrite = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
	descriptorWrite.dstSet = m_descriptorSets[frameIndex];
	descriptorWrite.dstBinding = samplerResourceBinding.binding;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.pBufferInfo = nullptr;
	descriptorWrite.pImageInfo = &samplerInfo;
	descriptorWrite.pTexelBufferView = nullptr;

	vkUpdateDescriptorSets(m_pContext->GetVkDevice(), 1, &descriptorWrite, 0, nullptr);
}
void MaterialProperties::UpdateDescriptorSet(uint32_t frameIndex, ResourceBinding<Texture2d*> texture2dResourceBinding)
{
	VkDescriptorImageInfo imageInfo = {};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	//imageInfo.imageLayout = texture2dResourceBinding.resource->GetVmaImage()->GetLayout();	// maybe use this? currently throws validation errors.
	imageInfo.imageView = texture2dResourceBinding.resource->GetVmaImage()->GetVkImageView();

	VkWriteDescriptorSet descriptorWrite = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
	descriptorWrite.dstSet = m_descriptorSets[frameIndex];
	descriptorWrite.dstBinding = texture2dResourceBinding.binding;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.pBufferInfo = nullptr;
	descriptorWrite.pImageInfo = &imageInfo;
	descriptorWrite.pTexelBufferView = nullptr;

	vkUpdateDescriptorSets(m_pContext->GetVkDevice(), 1, &descriptorWrite, 0, nullptr);
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

template int MaterialProperties::GetValue<int>(const std::string& blockName, const std::string& memberName) const;
template bool MaterialProperties::GetValue<bool>(const std::string& blockName, const std::string& memberName) const;
template float MaterialProperties::GetValue<float>(const std::string& blockName, const std::string& memberName) const;
template Float2 MaterialProperties::GetValue<Float2>(const std::string& blockName, const std::string& memberName) const;
template Float3 MaterialProperties::GetValue<Float3>(const std::string& blockName, const std::string& memberName) const;
template Float4 MaterialProperties::GetValue<Float4>(const std::string& blockName, const std::string& memberName) const;
template Float4x4 MaterialProperties::GetValue<Float4x4>(const std::string& blockName, const std::string& memberName) const;

template int MaterialProperties::GetValue<int>(const std::string& blockName, const std::string& arrayName, uint32_t arrayIndex) const;
template bool MaterialProperties::GetValue<bool>(const std::string& blockName, const std::string& arrayName, uint32_t arrayIndex) const;
template float MaterialProperties::GetValue<float>(const std::string& blockName, const std::string& arrayName, uint32_t arrayIndex) const;
template Float2 MaterialProperties::GetValue<Float2>(const std::string& blockName, const std::string& arrayName, uint32_t arrayIndex) const;
template Float3 MaterialProperties::GetValue<Float3>(const std::string& blockName, const std::string& arrayName, uint32_t arrayIndex) const;
template Float4 MaterialProperties::GetValue<Float4>(const std::string& blockName, const std::string& arrayName, uint32_t arrayIndex) const;
template Float4x4 MaterialProperties::GetValue<Float4x4>(const std::string& blockName, const std::string& arrayName, uint32_t arrayIndex) const;

template int MaterialProperties::GetValue<int>(const std::string& blockName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const;
template bool MaterialProperties::GetValue<bool>(const std::string& blockName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const;
template float MaterialProperties::GetValue<float>(const std::string& blockName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const;
template Float2 MaterialProperties::GetValue<Float2>(const std::string& blockName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const;
template Float3 MaterialProperties::GetValue<Float3>(const std::string& blockName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const;
template Float4 MaterialProperties::GetValue<Float4>(const std::string& blockName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const;
template Float4x4 MaterialProperties::GetValue<Float4x4>(const std::string& blockName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const;

template int MaterialProperties::GetValue<int>(const std::string& blockName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const;
template bool MaterialProperties::GetValue<bool>(const std::string& blockName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const;
template float MaterialProperties::GetValue<float>(const std::string& blockName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const;
template Float2 MaterialProperties::GetValue<Float2>(const std::string& blockName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const;
template Float3 MaterialProperties::GetValue<Float3>(const std::string& blockName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const;
template Float4 MaterialProperties::GetValue<Float4>(const std::string& blockName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const;
template Float4x4 MaterialProperties::GetValue<Float4x4>(const std::string& blockName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const;