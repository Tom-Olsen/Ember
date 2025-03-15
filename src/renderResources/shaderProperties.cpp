#include "shaderProperties.h"
#include "bufferManager.h"
#include "computeShader.h"
#include "logger.h"
#include "material.h"
#include "emberMath.h"
#include "pipeline.h"
#include "renderPassManager.h"
#include "sampler.h"
#include "samplerManager.h"
#include "shadowRenderPass.h"
#include "spirvReflect.h"
#include "storageBuffer.h"
#include "texture2d.h"
#include "textureManager.h"
#include "uniformBuffer.h"
#include "vmaBuffer.h"
#include "vmaImage.h"
#include "vulkanContext.h"
#include "vulkanUtility.h"
#include "vulkanMacros.h"



namespace emberEngine
{
	// Consructor/Destructor:
	ShaderProperties::ShaderProperties(Shader* pShader)
	{
		m_pShader = pShader;

		// Create resource bindings and uniform buffer update logic for each frameInFlight:
		m_uniformBufferMaps = std::vector<std::unordered_map<std::string, UniformBufferBinding>>(VulkanContext::framesInFlight);
		m_samplerMaps = std::vector<std::unordered_map<std::string, SamplerBinding>>(VulkanContext::framesInFlight);
		m_texture2dMaps = std::vector<std::unordered_map<std::string, TextureBinding>>(VulkanContext::framesInFlight);
		m_storageBufferMaps = std::vector<std::unordered_map<std::string, StorageBufferBinding>>(VulkanContext::framesInFlight);
		m_updateUniformBuffer = std::vector<std::unordered_map<std::string, bool>>(VulkanContext::framesInFlight);

		// Initialize all resource bindings:
		for (uint32_t frameIndex = 0; frameIndex < VulkanContext::framesInFlight; frameIndex++)
		{
			const DescriptorBoundResources* const descriptorBoundResources = m_pShader->GetDescriptorBoundResources();
			for (uint32_t i = 0; i < descriptorBoundResources->bindingCount; i++)
			{
				VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = descriptorBoundResources->descriptorSetLayoutBindings[i];
				VkDescriptorType descriptorType = descriptorSetLayoutBinding.descriptorType;
				uint32_t binding = descriptorSetLayoutBinding.binding;
				const std::string& name = descriptorBoundResources->descriptorSetBindingNames[i];

				if (descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
					InitUniformBufferResourceBinding(frameIndex, name, binding);
				else if (descriptorType == VK_DESCRIPTOR_TYPE_SAMPLER)
					InitSamplerResourceBinding(frameIndex, name, binding, SamplerManager::GetSampler("colorSampler"));
				else if (descriptorType == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE)
				{
					VkImageViewType viewType = descriptorBoundResources->sampleViewTypeMap.at(name);
					switch (viewType)
					{
						case VK_IMAGE_VIEW_TYPE_1D: throw std::runtime_error("Initialization for sampling Texture1D descriptorSet not implemented yet!"); break;
						case VK_IMAGE_VIEW_TYPE_2D: InitTexture2dResourceBinding(frameIndex, name, binding, TextureManager::GetTexture2d("white"), descriptorType); break;
						case VK_IMAGE_VIEW_TYPE_3D: throw std::runtime_error("Initialization for sampling Texture3D descriptorSet not implemented yet!"); break;
						case VK_IMAGE_VIEW_TYPE_CUBE: InitTexture2dResourceBinding(frameIndex, name, binding, TextureManager::GetTexture2d("skyBoxWhite"), descriptorType); break;
						case VK_IMAGE_VIEW_TYPE_1D_ARRAY: throw std::runtime_error("Initialization for sampling Texture1DArray descriptorSet not implemented yet!"); break;
						case VK_IMAGE_VIEW_TYPE_2D_ARRAY: InitTexture2dResourceBinding(frameIndex, name, binding, TextureManager::GetTexture2d("defaultArrayTexture2d"), descriptorType); break;
						case VK_IMAGE_VIEW_TYPE_CUBE_ARRAY: throw std::runtime_error("Initialization for sampling CubeTextureArray descriptorSet not implemented yet!"); break;
					}
				}
				else if (descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_IMAGE)
				{
					VkImageViewType viewType = descriptorBoundResources->storageViewTypeMap.at(name);
					switch (viewType)
					{
						case VK_IMAGE_VIEW_TYPE_1D: throw std::runtime_error("Initialization for storage Texture1D descriptorSet not implemented yet!"); break;
						case VK_IMAGE_VIEW_TYPE_2D: InitTexture2dResourceBinding(frameIndex, name, binding, TextureManager::GetTexture2d("defaultStorageTexture2d"), descriptorType); break;
						case VK_IMAGE_VIEW_TYPE_3D: throw std::runtime_error("Initialization for storage Texture3D descriptorSet not implemented yet!"); break;
						case VK_IMAGE_VIEW_TYPE_CUBE: throw std::runtime_error("Initialization for storage CubeTexture descriptorSet not implemented yet!"); break;
						case VK_IMAGE_VIEW_TYPE_1D_ARRAY: throw std::runtime_error("Initialization storage for Texture1DArray descriptorSet not implemented yet!"); break;
						case VK_IMAGE_VIEW_TYPE_2D_ARRAY: throw std::runtime_error("Initialization storage for Texture2DArray descriptorSet not implemented yet!"); break;
						case VK_IMAGE_VIEW_TYPE_CUBE_ARRAY: throw std::runtime_error("Initialization storage for CubeTextureArray descriptorSet not implemented yet!"); break;
					}
				}
				else if (descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER)
					InitStorageBufferResourceBinding(frameIndex, name, binding, (StorageBuffer*)BufferManager::GetBuffer("dummyStorageBuffer"));
			}
		}
		InitStagingMaps();
		InitDescriptorSets();

		// Set default values:
		ShadowRenderPass* pShadowRenderPass = RenderPassManager::GetShadowRenderPass();
		SetSampler("shadowSampler", SamplerManager::GetSampler("shadowSampler"));
		SetTexture2d("shadowMaps", (Texture2d*)pShadowRenderPass->GetShadowMaps());
		SetTexture2d("normalMap", TextureManager::GetTexture2d("defaultNormalMap"));
		SetValue("SurfaceProperties", "diffuseColor", Float4::white);
		SetValue("SurfaceProperties", "roughness", 0.5f);
		SetValue("SurfaceProperties", "reflectivity", Float3(0.4f));
		SetValue("SurfaceProperties", "metallicity", 0);
		SetValue("SurfaceProperties", "scaleOffset", Float4(1, 1, 0, 0));
	}
	ShaderProperties::~ShaderProperties()
	{

	}



	// Public methods:
	void ShaderProperties::UpdateShaderData()
	{
		// Stream uniform buffer data from host memory to GPU only for current frameIndex:
		for (auto& [name, uniformBufferBinding] : m_uniformBufferMaps[VulkanContext::frameIndex])
		{
			if (m_updateUniformBuffer[VulkanContext::frameIndex].at(name))
			{
				uniformBufferBinding.pUniformBuffer->UpdateBuffer();
				m_updateUniformBuffer[VulkanContext::frameIndex].at(name) = false;
			}
		}

		// Change the pointer the descriptor set points at to the new sampler:
		for (auto& [name, samplerBinding] : m_samplerMaps[VulkanContext::frameIndex])
		{
			if (samplerBinding.pSampler != m_samplerStagingMap.at(name))
			{
				samplerBinding.pSampler = m_samplerStagingMap.at(name);
				UpdateDescriptorSet(VulkanContext::frameIndex, samplerBinding);
			}
		}

		// Change the pointer the descriptor set points at to the new texture2d:
		for (auto& [name, texture2dBinding] : m_texture2dMaps[VulkanContext::frameIndex])
		{
			if (texture2dBinding.pTexture2d != m_texture2dStagingMap.at(name))
			{
				texture2dBinding.pTexture2d = m_texture2dStagingMap.at(name);
				UpdateDescriptorSet(VulkanContext::frameIndex, texture2dBinding);
			}
		}

		// Change the pointer the descriptor set points at to the new storageBuffer:
		for (auto& [name, storageBufferBinding] : m_storageBufferMaps[VulkanContext::frameIndex])
		{
			if (storageBufferBinding.pStorageBuffer != m_storageBufferStagingMap.at(name))
			{
				storageBufferBinding.pStorageBuffer = m_storageBufferStagingMap.at(name);
				UpdateDescriptorSet(VulkanContext::frameIndex, storageBufferBinding);
			}
		}
	}
	const VkDescriptorSet& ShaderProperties::GetDescriptorSet(uint32_t frameIndex)
	{
		return m_descriptorSets[frameIndex];
	}
	std::vector<VkDescriptorSet>& ShaderProperties::GetDescriptorSets()
	{
		return m_descriptorSets;
	}




	// Setters:
	void ShaderProperties::SetSampler(const std::string& name, Sampler* pSampler)
	{
		// If sampler with 'name' doesnt exist, skip:
		auto it = m_samplerStagingMap.find(name);
		if (it == m_samplerStagingMap.end())
			return;
		it->second = pSampler;
	}
	void ShaderProperties::SetTexture2d(const std::string& name, Texture2d* pTexture2d)
	{
		// If texture with 'name' doesnt exist, skip:
		auto it = m_texture2dStagingMap.find(name);
		if (it == m_texture2dStagingMap.end())
			return;
		it->second = pTexture2d;
	}
	void ShaderProperties::SetStorageBuffer(const std::string& name, StorageBuffer* pStorageBuffer)
	{
		// If storageBuffer with 'name' doesnt exist, skip:
		auto it = m_storageBufferStagingMap.find(name);
		if (it == m_storageBufferStagingMap.end())
			return;
		it->second = pStorageBuffer;
	}
	// Uniform Buffer Setters:
	template<typename T>
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& memberName, const T& value)
	{
		auto it = m_uniformBufferMaps[0].find(bufferName);
		if (it != m_uniformBufferMaps[0].end())
		{// bufferName exists => set value for all frames:
			for (uint32_t frameIndex = 0; frameIndex < VulkanContext::framesInFlight; frameIndex++)
			{
				if (m_uniformBufferMaps[frameIndex].at(bufferName).pUniformBuffer->SetValue(memberName, value))
					m_updateUniformBuffer[frameIndex].at(bufferName) = true;
			}
		}
	}
	template<typename T>
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const T& value)
	{
		auto it = m_uniformBufferMaps[0].find(bufferName);
		if (it != m_uniformBufferMaps[0].end())
		{// bufferName exists => set value for all frames:
			for (uint32_t frameIndex = 0; frameIndex < VulkanContext::framesInFlight; frameIndex++)
			{
				if (m_uniformBufferMaps[frameIndex].at(bufferName).pUniformBuffer->SetValue(arrayName, arrayIndex, value))
					m_updateUniformBuffer[frameIndex].at(bufferName) = true;
			}
		}
	}
	template<typename T>
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const T& value)
	{
		auto it = m_uniformBufferMaps[0].find(bufferName);
		if (it != m_uniformBufferMaps[0].end())
		{// bufferName exists => set value for all frames:
			for (uint32_t frameIndex = 0; frameIndex < VulkanContext::framesInFlight; frameIndex++)
			{
				if (m_uniformBufferMaps[frameIndex].at(bufferName).pUniformBuffer->SetValue(arrayName, arrayIndex, memberName, value))
					m_updateUniformBuffer[frameIndex].at(bufferName) = true;
			}
		}
	}
	template<typename T>
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const T& value)
	{
		auto it = m_uniformBufferMaps[0].find(bufferName);
		if (it != m_uniformBufferMaps[0].end())
		{// bufferName exists => set value for all frames:
			for (uint32_t frameIndex = 0; frameIndex < VulkanContext::framesInFlight; frameIndex++)
			{
				if (m_uniformBufferMaps[frameIndex].at(bufferName).pUniformBuffer->SetValue(arrayName, arrayIndex, subArrayName, subArrayIndex, value))
					m_updateUniformBuffer[frameIndex].at(bufferName) = true;
			}
		}
	}



	// Getters:
	Shader* ShaderProperties::GetShader() const
	{
		return m_pShader;
	}
	Sampler* ShaderProperties::GetSampler(const std::string& name) const
	{
		auto it = m_samplerMaps[VulkanContext::frameIndex].find(name);
		if (it != m_samplerMaps[VulkanContext::frameIndex].end())
			return it->second.pSampler;
		return nullptr;
	}
	Texture2d* ShaderProperties::GetTexture2d(const std::string& name) const
	{
		auto it = m_texture2dMaps[VulkanContext::frameIndex].find(name);
		if (it != m_texture2dMaps[VulkanContext::frameIndex].end())
			return it->second.pTexture2d;
		return nullptr;
	}
	// Uniform Buffer Getters:
	template<typename T>
	T ShaderProperties::GetValue(const std::string& bufferName, const std::string& memberName) const
	{
		auto it = m_uniformBufferMaps[0].find(bufferName);
		if (it != m_uniformBufferMaps[0].end())
			return it->second.pUniformBuffer->GetValue<T>(memberName);
		return T();
	}
	template<typename T>
	T ShaderProperties::GetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex) const
	{
		auto it = m_uniformBufferMaps[0].find(bufferName);
		if (it != m_uniformBufferMaps[0].end())
			return it->second.pUniformBuffer->GetValue<T>(arrayName, arrayIndex);
		return T();
	}
	template<typename T>
	T ShaderProperties::GetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const
	{
		auto it = m_uniformBufferMaps[0].find(bufferName);
		if (it != m_uniformBufferMaps[0].end())
			return it->second.pUniformBuffer->GetValue<T>(arrayName, arrayIndex, memberName);
		return T();
	}
	template<typename T>
	T ShaderProperties::GetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const
	{
		auto it = m_uniformBufferMaps[0].find(bufferName);
		if (it != m_uniformBufferMaps[0].end())
			return it->second.pUniformBuffer->GetValue<T>(arrayName, arrayIndex, subArrayName, subArrayIndex);
		return T();
	}




	// Debugging:
	void ShaderProperties::Print(const std::string& name) const
	{
		LOG_INFO("ShaderProperties: {}, {}", name, fmt::ptr(this));
		LOG_INFO("DescriptorSets: {}, {}", fmt::ptr(m_descriptorSets[0]), fmt::ptr(m_descriptorSets[1]));
	}
	void ShaderProperties::PrintMaps() const
	{
		for (uint32_t frameIndex = 0; frameIndex < VulkanContext::framesInFlight; frameIndex++)
		{
			LOG_INFO("UniformBufferMaps[{}]:", frameIndex);
			for (const auto& [name, uniformBufferBinding] : m_uniformBufferMaps[frameIndex])
				LOG_TRACE("binding: {}, bindingName: {}", uniformBufferBinding.binding, name);

			LOG_INFO("SamplerMaps[{}]:", frameIndex);
			for (const auto& [name, samplerBinding] : m_samplerMaps[frameIndex])
				LOG_TRACE("binding: {}, bindingName: {}, samplerName: {}", samplerBinding.binding, name, samplerBinding.pSampler->GetName());

			LOG_INFO("Texture2dMaps[{}]:", frameIndex);
			for (const auto& [name, texture2dBinding] : m_texture2dMaps[frameIndex])
				LOG_TRACE("binding: {}, bindingName: {}, textureName: {}", texture2dBinding.binding, name, texture2dBinding.pTexture2d->GetName());

			LOG_INFO("StorageBufferMaps[{}]:", frameIndex);
			for (const auto& [name, storageBufferBinding] : m_storageBufferMaps[frameIndex])
				LOG_TRACE("binding: {}, bindingName: {}, storageBufferSize: {}", storageBufferBinding.binding, name, storageBufferBinding.pStorageBuffer->GetSize());

			LOG_TRACE("\n");
		}
	}



	// Private methods:
	// Initializers:
	void ShaderProperties::InitUniformBufferResourceBinding(uint32_t frameIndex, const std::string& name, uint32_t binding)
	{
		auto it = m_uniformBufferMaps[frameIndex].find(name);
		if (it == m_uniformBufferMaps[frameIndex].end())
		{
			const DescriptorBoundResources* const descriptorBoundResources = m_pShader->GetDescriptorBoundResources();
			UniformBufferBlock* pUniformBufferBlock = descriptorBoundResources->uniformBufferBlockMap.at(name);

			std::shared_ptr<UniformBuffer> uniformBuffer = std::make_shared<UniformBuffer>(pUniformBufferBlock);
			m_uniformBufferMaps[frameIndex].emplace(name, UniformBufferBinding(binding, uniformBuffer));

			for (uint32_t frameIndex = 0; frameIndex < VulkanContext::framesInFlight; frameIndex++)
				m_updateUniformBuffer[frameIndex].emplace(name, true);
		}
	}
	void ShaderProperties::InitSamplerResourceBinding(uint32_t frameIndex, const std::string& name, uint32_t binding, Sampler* pSampler)
	{
		auto it = m_samplerMaps[frameIndex].find(name);
		if (it == m_samplerMaps[frameIndex].end())
			m_samplerMaps[frameIndex].emplace(name, SamplerBinding(binding, pSampler));
	}
	void ShaderProperties::InitTexture2dResourceBinding(uint32_t frameIndex, const std::string& name, uint32_t binding, Texture2d* pTexture2d, VkDescriptorType descriptorType)
	{
		auto it = m_texture2dMaps[frameIndex].find(name);
		if (it == m_texture2dMaps[frameIndex].end())
			m_texture2dMaps[frameIndex].emplace(name, TextureBinding(binding, pTexture2d, descriptorType));
	}
	void ShaderProperties::InitStorageBufferResourceBinding(uint32_t frameIndex, const std::string& name, uint32_t binding, StorageBuffer* pStorageBuffer)
	{
		auto it = m_storageBufferMaps[frameIndex].find(name);
		if (it == m_storageBufferMaps[frameIndex].end())
			m_storageBufferMaps[frameIndex].emplace(name, StorageBufferBinding(binding, pStorageBuffer));
	}
	void ShaderProperties::InitStagingMaps()
	{
		for (auto& [name, samplerBinding] : m_samplerMaps[0])
			m_samplerStagingMap.emplace(name, samplerBinding.pSampler);
		for (auto& [name, texture2dBinding] : m_texture2dMaps[0])
			m_texture2dStagingMap.emplace(name, texture2dBinding.pTexture2d);
		for (auto& [name, storageBufferBinding] : m_storageBufferMaps[0])
			m_storageBufferStagingMap.emplace(name, storageBufferBinding.pStorageBuffer);
	}
	void ShaderProperties::InitDescriptorSets()
	{
		CreateDescriptorSets();
		for (uint32_t frameIndex = 0; frameIndex < VulkanContext::framesInFlight; frameIndex++)
		{
			for (auto& [_, uniformBufferBinding] : m_uniformBufferMaps[frameIndex])
				UpdateDescriptorSet(frameIndex, uniformBufferBinding);
			for (auto& [_, samplerBinding] : m_samplerMaps[frameIndex])
				UpdateDescriptorSet(frameIndex, samplerBinding);
			for (auto& [_, texture2dBinding] : m_texture2dMaps[frameIndex])
				UpdateDescriptorSet(frameIndex, texture2dBinding);
			for (auto& [_, storageBufferBinding] : m_storageBufferMaps[frameIndex])
				UpdateDescriptorSet(frameIndex, storageBufferBinding);
		}
	}



	// Descriptor Set management:
	void ShaderProperties::CreateDescriptorSets()
	{
		std::vector<VkDescriptorSetLayout> layouts(VulkanContext::framesInFlight, m_pShader->GetPipeline()->GetVkDescriptorSetLayout());	// same layout for all frames

		VkDescriptorSetAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
		allocInfo.descriptorPool = VulkanContext::GetVkDescriptorPool();
		allocInfo.descriptorSetCount = VulkanContext::framesInFlight;
		allocInfo.pSetLayouts = layouts.data();

		m_descriptorSets.resize(VulkanContext::framesInFlight);
		VKA(vkAllocateDescriptorSets(VulkanContext::pLogicalDevice->GetVkDevice(), &allocInfo, m_descriptorSets.data()));
	}
	void ShaderProperties::UpdateDescriptorSet(uint32_t frameIndex, UniformBufferBinding uniformBufferResourceBinding)
	{
		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = uniformBufferResourceBinding.pUniformBuffer->GetVmaBuffer()->GetVkBuffer();
		bufferInfo.offset = 0;
		bufferInfo.range = uniformBufferResourceBinding.pUniformBuffer->GetSize();

		VkWriteDescriptorSet descriptorWrite = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
		descriptorWrite.dstSet = m_descriptorSets[frameIndex];
		descriptorWrite.dstBinding = uniformBufferResourceBinding.binding;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;
		descriptorWrite.pImageInfo = nullptr;
		descriptorWrite.pTexelBufferView = nullptr;

		vkUpdateDescriptorSets(VulkanContext::GetVkDevice(), 1, &descriptorWrite, 0, nullptr);
	}
	void ShaderProperties::UpdateDescriptorSet(uint32_t frameIndex, SamplerBinding samplerResourceBinding)
	{
		VkDescriptorImageInfo samplerInfo = {};
		samplerInfo.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		samplerInfo.sampler = samplerResourceBinding.pSampler->GetVkSampler();

		VkWriteDescriptorSet descriptorWrite = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
		descriptorWrite.dstSet = m_descriptorSets[frameIndex];
		descriptorWrite.dstBinding = samplerResourceBinding.binding;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = nullptr;
		descriptorWrite.pImageInfo = &samplerInfo;
		descriptorWrite.pTexelBufferView = nullptr;

		vkUpdateDescriptorSets(VulkanContext::GetVkDevice(), 1, &descriptorWrite, 0, nullptr);
	}
	void ShaderProperties::UpdateDescriptorSet(uint32_t frameIndex, TextureBinding texture2dResourceBinding)
	{
		VkDescriptorImageInfo imageInfo = {};
		imageInfo.imageLayout = texture2dResourceBinding.pTexture2d->GetVmaImage()->GetLayout();
		imageInfo.imageView = texture2dResourceBinding.pTexture2d->GetVmaImage()->GetVkImageView();

		VkWriteDescriptorSet descriptorWrite = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
		descriptorWrite.dstSet = m_descriptorSets[frameIndex];
		descriptorWrite.dstBinding = texture2dResourceBinding.binding;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = texture2dResourceBinding.descriptorType;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = nullptr;
		descriptorWrite.pImageInfo = &imageInfo;
		descriptorWrite.pTexelBufferView = nullptr;

		vkUpdateDescriptorSets(VulkanContext::GetVkDevice(), 1, &descriptorWrite, 0, nullptr);
	}
	void ShaderProperties::UpdateDescriptorSet(uint32_t frameIndex, StorageBufferBinding storageBufferResourceBinding)
	{
		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = storageBufferResourceBinding.pStorageBuffer->GetVmaBuffer()->GetVkBuffer();
		bufferInfo.offset = 0;
		bufferInfo.range = storageBufferResourceBinding.pStorageBuffer->GetSize();

		VkWriteDescriptorSet descriptorWrite = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
		descriptorWrite.dstSet = m_descriptorSets[frameIndex];
		descriptorWrite.dstBinding = storageBufferResourceBinding.binding;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;
		descriptorWrite.pImageInfo = nullptr;
		descriptorWrite.pTexelBufferView = nullptr;

		vkUpdateDescriptorSets(VulkanContext::GetVkDevice(), 1, &descriptorWrite, 0, nullptr);
	}



	// Explicit template instantiation:
	template void ShaderProperties::SetValue<int>(const std::string& bufferName, const std::string& memberName, const int& value);
	template void ShaderProperties::SetValue<bool>(const std::string& bufferName, const std::string& memberName, const bool& value);
	template void ShaderProperties::SetValue<float>(const std::string& bufferName, const std::string& memberName, const float& value);
	template void ShaderProperties::SetValue<Float2>(const std::string& bufferName, const std::string& memberName, const Float2& value);
	template void ShaderProperties::SetValue<Float3>(const std::string& bufferName, const std::string& memberName, const Float3& value);
	template void ShaderProperties::SetValue<Float4>(const std::string& bufferName, const std::string& memberName, const Float4& value);
	template void ShaderProperties::SetValue<Float4x4>(const std::string& bufferName, const std::string& memberName, const Float4x4& value);

	template void ShaderProperties::SetValue<int>(const std::string& bufferName, const std::string& arrayName, uint32_t arrayindex, const int& value);
	template void ShaderProperties::SetValue<bool>(const std::string& bufferName, const std::string& arrayName, uint32_t arrayindex, const bool& value);
	template void ShaderProperties::SetValue<float>(const std::string& bufferName, const std::string& arrayName, uint32_t arrayindex, const float& value);
	template void ShaderProperties::SetValue<Float2>(const std::string& bufferName, const std::string& arrayName, uint32_t arrayindex, const Float2& value);
	template void ShaderProperties::SetValue<Float3>(const std::string& bufferName, const std::string& arrayName, uint32_t arrayindex, const Float3& value);
	template void ShaderProperties::SetValue<Float4>(const std::string& bufferName, const std::string& arrayName, uint32_t arrayindex, const Float4& value);
	template void ShaderProperties::SetValue<Float4x4>(const std::string& bufferName, const std::string& arrayName, uint32_t arrayindex, const Float4x4& value);

	template void ShaderProperties::SetValue<int>(const std::string& bufferName, const std::string& arrayName, uint32_t arrayindex, const std::string& memberName, const int& value);
	template void ShaderProperties::SetValue<bool>(const std::string& bufferName, const std::string& arrayName, uint32_t arrayindex, const std::string& memberName, const bool& value);
	template void ShaderProperties::SetValue<float>(const std::string& bufferName, const std::string& arrayName, uint32_t arrayindex, const std::string& memberName, const float& value);
	template void ShaderProperties::SetValue<Float2>(const std::string& bufferName, const std::string& arrayName, uint32_t arrayindex, const std::string& memberName, const Float2& value);
	template void ShaderProperties::SetValue<Float3>(const std::string& bufferName, const std::string& arrayName, uint32_t arrayindex, const std::string& memberName, const Float3& value);
	template void ShaderProperties::SetValue<Float4>(const std::string& bufferName, const std::string& arrayName, uint32_t arrayindex, const std::string& memberName, const Float4& value);
	template void ShaderProperties::SetValue<Float4x4>(const std::string& bufferName, const std::string& arrayName, uint32_t arrayindex, const std::string& memberName, const Float4x4& value);

	template void ShaderProperties::SetValue<int>(const std::string& bufferName, const std::string& arrayName, uint32_t arrayindex, const std::string& subArrayName, uint32_t subArrayIndex, const int& value);
	template void ShaderProperties::SetValue<bool>(const std::string& bufferName, const std::string& arrayName, uint32_t arrayindex, const std::string& subArrayName, uint32_t subArrayIndex, const bool& value);
	template void ShaderProperties::SetValue<float>(const std::string& bufferName, const std::string& arrayName, uint32_t arrayindex, const std::string& subArrayName, uint32_t subArrayIndex, const float& value);
	template void ShaderProperties::SetValue<Float2>(const std::string& bufferName, const std::string& arrayName, uint32_t arrayindex, const std::string& subArrayName, uint32_t subArrayIndex, const Float2& value);
	template void ShaderProperties::SetValue<Float3>(const std::string& bufferName, const std::string& arrayName, uint32_t arrayindex, const std::string& subArrayName, uint32_t subArrayIndex, const Float3& value);
	template void ShaderProperties::SetValue<Float4>(const std::string& bufferName, const std::string& arrayName, uint32_t arrayindex, const std::string& subArrayName, uint32_t subArrayIndex, const Float4& value);
	template void ShaderProperties::SetValue<Float4x4>(const std::string& bufferName, const std::string& arrayName, uint32_t arrayindex, const std::string& subArrayName, uint32_t subArrayIndex, const Float4x4& value);

	template int ShaderProperties::GetValue<int>(const std::string& bufferName, const std::string& memberName) const;
	template bool ShaderProperties::GetValue<bool>(const std::string& bufferName, const std::string& memberName) const;
	template float ShaderProperties::GetValue<float>(const std::string& bufferName, const std::string& memberName) const;
	template Float2 ShaderProperties::GetValue<Float2>(const std::string& bufferName, const std::string& memberName) const;
	template Float3 ShaderProperties::GetValue<Float3>(const std::string& bufferName, const std::string& memberName) const;
	template Float4 ShaderProperties::GetValue<Float4>(const std::string& bufferName, const std::string& memberName) const;
	template Float4x4 ShaderProperties::GetValue<Float4x4>(const std::string& bufferName, const std::string& memberName) const;

	template int ShaderProperties::GetValue<int>(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex) const;
	template bool ShaderProperties::GetValue<bool>(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex) const;
	template float ShaderProperties::GetValue<float>(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex) const;
	template Float2 ShaderProperties::GetValue<Float2>(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex) const;
	template Float3 ShaderProperties::GetValue<Float3>(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex) const;
	template Float4 ShaderProperties::GetValue<Float4>(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex) const;
	template Float4x4 ShaderProperties::GetValue<Float4x4>(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex) const;

	template int ShaderProperties::GetValue<int>(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const;
	template bool ShaderProperties::GetValue<bool>(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const;
	template float ShaderProperties::GetValue<float>(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const;
	template Float2 ShaderProperties::GetValue<Float2>(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const;
	template Float3 ShaderProperties::GetValue<Float3>(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const;
	template Float4 ShaderProperties::GetValue<Float4>(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const;
	template Float4x4 ShaderProperties::GetValue<Float4x4>(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const;

	template int ShaderProperties::GetValue<int>(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const;
	template bool ShaderProperties::GetValue<bool>(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const;
	template float ShaderProperties::GetValue<float>(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const;
	template Float2 ShaderProperties::GetValue<Float2>(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const;
	template Float3 ShaderProperties::GetValue<Float3>(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const;
	template Float4 ShaderProperties::GetValue<Float4>(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const;
	template Float4x4 ShaderProperties::GetValue<Float4x4>(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const;
}