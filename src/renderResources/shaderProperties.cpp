#include "shaderProperties.h"
#include "computeShader.h"
#include "logger.h"
#include "material.h"
#include "mathf.h"
#include "pipeline.h"
#include "renderPassManager.h"
#include "sampler.h"
#include "samplerManager.h"
#include "shadowRenderPass.h"
#include "spirvReflect.h"
#include "texture2d.h"
#include "textureManager.h"
#include "uniformBuffer.h"
#include "vmaBuffer.h"
#include "vmaImage.h"
#include "vulkanContext.h"
#include "vulkanEnumToString.h"
#include "vulkanMacros.h"
#include <iostream>



namespace emberEngine
{
	// Consructor/Destructor:
	ShaderProperties::ShaderProperties(Shader* pShader)
	{
		m_pShader = pShader;
		m_pContext = pShader->GetContext();

		// Create resource bindings and uniform buffer update logic for each frameInFlight:
		m_uniformBufferMaps = std::vector<std::unordered_map<std::string, ResourceBinding<std::shared_ptr<UniformBuffer>>>>(m_pContext->framesInFlight);
		m_samplerMaps = std::vector<std::unordered_map<std::string, ResourceBinding<Sampler*>>>(m_pContext->framesInFlight);
		m_texture2dMaps = std::vector<std::unordered_map<std::string, ResourceBinding<Texture2d*>>>(m_pContext->framesInFlight);
		m_updateUniformBuffer = std::vector<std::unordered_map<std::string, bool>>(m_pContext->framesInFlight);


		for (uint32_t frameIndex = 0; frameIndex < m_pContext->framesInFlight; frameIndex++)
		{
			const DescriptorBoundResources* const descriptorBoundResources = m_pShader->GetDescriptorBoundResources();
			for (uint32_t i = 0; i < descriptorBoundResources->bindingCount; i++)
			{
				VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = descriptorBoundResources->descriptorSetLayoutBindings[i];
				VkDescriptorType type = descriptorSetLayoutBinding.descriptorType;
				uint32_t binding = descriptorSetLayoutBinding.binding;
				const std::string& name = descriptorBoundResources->descriptorSetBindingNames[i];

				if (type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
					InitUniformBufferResourceBinding(name, binding, frameIndex);
				else if (type == VK_DESCRIPTOR_TYPE_SAMPLER)
					InitSamplerResourceBinding(name, binding, SamplerManager::GetSampler("colorSampler"), frameIndex);
				else if (type == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE)
				{
					VkImageViewType viewType = descriptorBoundResources->sampleViewTypeMap.at(name);
					switch (viewType)
					{
						case VK_IMAGE_VIEW_TYPE_1D: throw std::runtime_error("Initialization for sampling Texture1D descriptorSet not implemented yet!"); break;
						case VK_IMAGE_VIEW_TYPE_2D: InitTexture2dResourceBinding(name, binding, TextureManager::GetTexture2d("white"), frameIndex); break;
						case VK_IMAGE_VIEW_TYPE_3D: throw std::runtime_error("Initialization for sampling Texture3D descriptorSet not implemented yet!"); break;
						case VK_IMAGE_VIEW_TYPE_CUBE: InitTexture2dResourceBinding(name, binding, TextureManager::GetTexture2d("skyBoxWhite"), frameIndex); break;
						case VK_IMAGE_VIEW_TYPE_1D_ARRAY: throw std::runtime_error("Initialization for sampling Texture1DArray descriptorSet not implemented yet!"); break;
						case VK_IMAGE_VIEW_TYPE_2D_ARRAY: InitTexture2dResourceBinding(name, binding, TextureManager::GetTexture2d("blackArray"), frameIndex); break;
						case VK_IMAGE_VIEW_TYPE_CUBE_ARRAY: throw std::runtime_error("Initialization for sampling CubeTextureArray descriptorSet not implemented yet!"); break;
					}
				}
				else if (type == VK_DESCRIPTOR_TYPE_STORAGE_IMAGE)
				{
					VkImageViewType viewType = descriptorBoundResources->storageViewTypeMap.at(name);
					switch (viewType)
					{
						case VK_IMAGE_VIEW_TYPE_1D: throw std::runtime_error("Initialization for storage Texture1D descriptorSet not implemented yet!"); break;
						case VK_IMAGE_VIEW_TYPE_2D: InitTexture2dResourceBinding(name, binding, TextureManager::GetTexture2d("storageTexture8x8"), frameIndex); break;
						case VK_IMAGE_VIEW_TYPE_3D: throw std::runtime_error("Initialization for storage Texture3D descriptorSet not implemented yet!"); break;
						case VK_IMAGE_VIEW_TYPE_CUBE: throw std::runtime_error("Initialization for storage CubeTexture descriptorSet not implemented yet!"); break;
						case VK_IMAGE_VIEW_TYPE_1D_ARRAY: throw std::runtime_error("Initialization storage for Texture1DArray descriptorSet not implemented yet!"); break;
						case VK_IMAGE_VIEW_TYPE_2D_ARRAY: throw std::runtime_error("Initialization storage for Texture2DArray descriptorSet not implemented yet!"); break;
						case VK_IMAGE_VIEW_TYPE_CUBE_ARRAY: throw std::runtime_error("Initialization storage for CubeTextureArray descriptorSet not implemented yet!"); break;
					}
					
				}
			}
		}
		InitStagingMaps();
		InitDescriptorSets();

		// Set default values:
		ShadowRenderPass* pShadowRenderPass = (ShadowRenderPass*)RenderPassManager::GetRenderPass("shadowRenderPass");
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
	std::vector<VkDescriptorSet>& ShaderProperties::GetDescriptorSets()
	{
		return m_descriptorSets;
	}




	// Uniform Buffer Setters:
	template<typename T>
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& memberName, const T& value)
	{
		auto it = m_uniformBufferMaps[0].find(bufferName);
		if (it != m_uniformBufferMaps[0].end())
		{// bufferName exists => set value for all frames:
			for (uint32_t frameIndex = 0; frameIndex < m_pContext->framesInFlight; frameIndex++)
			{
				if (m_uniformBufferMaps[frameIndex].at(bufferName).resource->SetValue(memberName, value))
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
			for (uint32_t frameIndex = 0; frameIndex < m_pContext->framesInFlight; frameIndex++)
			{
				if (m_uniformBufferMaps[frameIndex].at(bufferName).resource->SetValue(arrayName, arrayIndex, value))
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
			for (uint32_t frameIndex = 0; frameIndex < m_pContext->framesInFlight; frameIndex++)
			{
				if (m_uniformBufferMaps[frameIndex].at(bufferName).resource->SetValue(arrayName, arrayIndex, memberName, value))
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
			for (uint32_t frameIndex = 0; frameIndex < m_pContext->framesInFlight; frameIndex++)
			{
				if (m_uniformBufferMaps[frameIndex].at(bufferName).resource->SetValue(arrayName, arrayIndex, subArrayName, subArrayIndex, value))
					m_updateUniformBuffer[frameIndex].at(bufferName) = true;
			}
		}
	}

	// Sampler setters:
	void ShaderProperties::SetSampler(const std::string& name, Sampler* pSampler)
	{
		// If sampler with 'name' doesnt exist, skip:
		auto it = m_samplerStagingMap.find(name);
		if (it == m_samplerStagingMap.end() || it->second == pSampler)
			return;

		it->second = pSampler;
	}

	// Texture2d setters:
	void ShaderProperties::SetTexture2d(const std::string& name, Texture2d* pTexture2d)
	{
		// If texture with 'name' doesnt exist, skip:
		auto it = m_texture2dStagingMap.find(name);
		if (it == m_texture2dStagingMap.end() || it->second == pTexture2d)
			return;

		// Check of storage/sampler mode of the imput Texture2d matched with the descriptorSet:
		if (it->second->GetType() != pTexture2d->GetType())
		{
			LOG_WARN("Trying to asign wrong texture type to descriptor set. Expected type: '{}', given type: '{}'", it->second->GetTypeName(), pTexture2d->GetTypeName());
			return;
		}

		it->second = pTexture2d;
	}



	// Uniform Buffer Getters:
	template<typename T>
	T ShaderProperties::GetValue(const std::string& bufferName, const std::string& memberName) const
	{
		auto it = m_uniformBufferMaps[0].find(bufferName);
		if (it != m_uniformBufferMaps[0].end())
			return it->second.resource->GetValue<T>(memberName);
		return T();
	}
	template<typename T>
	T ShaderProperties::GetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex) const
	{
		auto it = m_uniformBufferMaps[0].find(bufferName);
		if (it != m_uniformBufferMaps[0].end())
			return it->second.resource->GetValue<T>(arrayName, arrayIndex);
		return T();
	}
	template<typename T>
	T ShaderProperties::GetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const
	{
		auto it = m_uniformBufferMaps[0].find(bufferName);
		if (it != m_uniformBufferMaps[0].end())
			return it->second.resource->GetValue<T>(arrayName, arrayIndex, memberName);
		return T();
	}
	template<typename T>
	T ShaderProperties::GetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const
	{
		auto it = m_uniformBufferMaps[0].find(bufferName);
		if (it != m_uniformBufferMaps[0].end())
			return it->second.resource->GetValue<T>(arrayName, arrayIndex, subArrayName, subArrayIndex);
		return T();
	}

	// Sampler and Texture2d getters:
	Sampler* ShaderProperties::GetSampler(const std::string& name) const
	{
		auto it = m_samplerMaps[m_pContext->frameIndex].find(name);
		if (it != m_samplerMaps[m_pContext->frameIndex].end())
			return it->second.resource;
		return nullptr;
	}
	Texture2d* ShaderProperties::GetTexture2d(const std::string& name) const
	{
		auto it = m_texture2dMaps[m_pContext->frameIndex].find(name);
		if (it != m_texture2dMaps[m_pContext->frameIndex].end())
			return it->second.resource;
		return nullptr;
	}



	// Debugging:
	void ShaderProperties::Print(const std::string& name) const
	{
		std::cout << "ShaderProperties: " << name << "\n";
		std::cout << "DescriptorSets: " << m_descriptorSets[0] << ", " << m_descriptorSets[1] << "\n";
	}
	void ShaderProperties::PrintMaps() const
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
	void ShaderProperties::InitUniformBufferResourceBinding(const std::string& name, uint32_t binding, uint32_t frameIndex)
	{
		auto it = m_uniformBufferMaps[frameIndex].find(name);
		if (it == m_uniformBufferMaps[frameIndex].end())
		{
			const DescriptorBoundResources* const descriptorBoundResources = m_pShader->GetDescriptorBoundResources();
			UniformBufferBlock* pUniformBufferBlock = descriptorBoundResources->uniformBufferBlockMap.at(name);

			std::shared_ptr<UniformBuffer> uniformBuffer = std::make_shared<UniformBuffer>(m_pContext, pUniformBufferBlock);
			m_uniformBufferMaps[frameIndex].emplace(name, ResourceBinding<std::shared_ptr<UniformBuffer>>(uniformBuffer, binding));

			for (uint32_t frameIndex = 0; frameIndex < m_pContext->framesInFlight; frameIndex++)
				m_updateUniformBuffer[frameIndex].emplace(name, true);
		}
	}
	void ShaderProperties::InitSamplerResourceBinding(const std::string& name, uint32_t binding, Sampler* pSampler, uint32_t frameIndex)
	{
		auto it = m_samplerMaps[frameIndex].find(name);
		if (it == m_samplerMaps[frameIndex].end())
			m_samplerMaps[frameIndex].emplace(name, ResourceBinding<Sampler*>(pSampler, binding));
	}
	void ShaderProperties::InitTexture2dResourceBinding(const std::string& name, uint32_t binding, Texture2d* pTexture2d, uint32_t frameIndex)
	{
		auto it = m_texture2dMaps[frameIndex].find(name);
		if (it == m_texture2dMaps[frameIndex].end())
			m_texture2dMaps[frameIndex].emplace(name, ResourceBinding<Texture2d*>(pTexture2d, binding));
	}
	void ShaderProperties::InitStagingMaps()
	{
		for (auto& [name, resourceBinding] : m_samplerMaps[0])
			m_samplerStagingMap.emplace(name, resourceBinding.resource);
		for (auto& [name, resourceBinding] : m_texture2dMaps[0])
			m_texture2dStagingMap.emplace(name, resourceBinding.resource);
	}
	void ShaderProperties::InitDescriptorSets()
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
	void ShaderProperties::CreateDescriptorSets()
	{
		std::vector<VkDescriptorSetLayout> layouts(m_pContext->framesInFlight, m_pShader->GetPipeline()->GetVkDescriptorSetLayout());	// same layout for all frames

		VkDescriptorSetAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
		allocInfo.descriptorPool = m_pContext->GetVkDescriptorPool();
		allocInfo.descriptorSetCount = m_pContext->framesInFlight;
		allocInfo.pSetLayouts = layouts.data();

		m_descriptorSets.resize(m_pContext->framesInFlight);
		VKA(vkAllocateDescriptorSets(m_pContext->pLogicalDevice->GetVkDevice(), &allocInfo, m_descriptorSets.data()));
	}
	void ShaderProperties::UpdateDescriptorSet(uint32_t frameIndex, ResourceBinding<std::shared_ptr<UniformBuffer>> uniformBufferResourceBinding)
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
	void ShaderProperties::UpdateDescriptorSet(uint32_t frameIndex, ResourceBinding<Sampler*> samplerResourceBinding)
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
	#include <iostream>
	void ShaderProperties::UpdateDescriptorSet(uint32_t frameIndex, ResourceBinding<Texture2d*> texture2dResourceBinding)
	{
		VkDescriptorImageInfo imageInfo = {};
		imageInfo.imageLayout = texture2dResourceBinding.resource->GetVmaImage()->GetLayout();
		imageInfo.imageView = texture2dResourceBinding.resource->GetVmaImage()->GetVkImageView();

		VkWriteDescriptorSet descriptorWrite = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
		descriptorWrite.dstSet = m_descriptorSets[frameIndex];
		descriptorWrite.dstBinding = texture2dResourceBinding.binding;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = texture2dResourceBinding.resource->GetVkDescriptorType();
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = nullptr;
		descriptorWrite.pImageInfo = &imageInfo;
		descriptorWrite.pTexelBufferView = nullptr;

		vkUpdateDescriptorSets(m_pContext->GetVkDevice(), 1, &descriptorWrite, 0, nullptr);
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