#include "vulkanDescriptorSetBinding.h"
#include "descriptorSetMacros.h"
#include "emberMath.h"
#include "logger.h"
#include "shaderReflection.h"
#include "vmaBuffer.h"
#include "vmaImage.h"
#include "vulkanBuffer.h"
#include "vulkanContext.h"
#include "vulkanDefaultGpuResources.h"
#include "vulkanDepthTexture2dArray.h"
#include "vulkanGarbageCollector.h"
#include "vulkanLogicalDevice.h"
#include "vulkanMacros.h"
#include "vulkanMesh.h"
#include "vulkanPipeline.h"
#include "vulkanRenderPassManager.h"
#include "vulkanSampleTexture2d.h"
#include "vulkanSampleTextureCube.h"
#include "vulkanStorageBuffer.h"
#include "vulkanStorageTexture2d.h"
#include "vulkanShader.h"
#include "vulkanShadowRenderPass.h"
#include "vulkanTexture.h"
#include "vulkanTexture2d.h"
#include "vulkanTextureCube.h"
#include <assert.h>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	DescriptorSetBinding::DescriptorSetBinding(Shader* pShader, uint32_t setIndex)
		: m_pShader(pShader), m_setIndex(setIndex), m_generation(0)
	{
		// Create resource bindings and uniform buffer update logic for each frameInFlight:
		m_textureMaps = std::vector<std::unordered_map<uint32_t, TextureBinding>>(Context::GetFramesInFlight());
		m_bufferMaps = std::vector<std::unordered_map<uint32_t, BufferBinding>>(Context::GetFramesInFlight());
		m_uniformBufferMap = std::unordered_map<uint32_t, UniformBufferBinding>();
        
		// Initialize all resource bindings:
		const emberSpirvReflect::DescriptorSetReflection& descriptorSetReflection = m_pShader->GetShaderReflection().GetDescriptorSetReflection(m_setIndex);
		for (uint32_t frameIndex = 0; frameIndex < Context::GetFramesInFlight(); frameIndex++)
		{
            for (const emberSpirvReflect::DescriptorReflection& descriptorReflection : descriptorSetReflection.GetDescriptorReflections())
			{
				const std::string& name = descriptorReflection.GetName();
				DescriptorType descriptorType = descriptorReflection.GetDescriptorType();
				uint32_t binding = descriptorReflection.GetBinding();
				if (frameIndex == 0)
				{
					m_bindingIndices.emplace(name, binding);
					m_bindingNames.emplace(binding, name);
				}

				if (descriptorType == DescriptorTypes::uniform_buffer)
					InitUniformBufferBinding(binding, descriptorReflection.GetUniformBufferDescriptor()->bufferLayout);
				else if (descriptorType == DescriptorTypes::sampled_image)
				{
					ImageViewType viewType = descriptorReflection.GetImageDescriptor()->imageViewType;;
					if (viewType == ImageViewTypes::view_type_1d) throw std::runtime_error("Initialization for sampling Texture1d descriptorSet not implemented yet!");
					else if (viewType == ImageViewTypes::view_type_2d) InitTextureBinding(frameIndex, binding, static_cast<Texture*>(DefaultGpuResources::GetDefaultSampleTexture2d()), descriptorType);
					else if (viewType == ImageViewTypes::view_type_3d) throw std::runtime_error("Initialization for sampling Texture3d descriptorSet not implemented yet!");
					else if (viewType == ImageViewTypes::view_type_cube) InitTextureBinding(frameIndex, binding, static_cast<Texture*>(DefaultGpuResources::GetDefaultSampleTextureCube()), descriptorType);
					else if (viewType == ImageViewTypes::view_type_1d_array) throw std::runtime_error("Initialization for sampling Texture1dArray descriptorSet not implemented yet!");
					else if (viewType == ImageViewTypes::view_type_2d_array) InitTextureBinding(frameIndex, binding, static_cast<Texture*>(DefaultGpuResources::GetDefaultDepthTexture2dArray()), descriptorType);
					else if (viewType == ImageViewTypes::view_type_cube_array) throw std::runtime_error("Initialization for sampling CubeTextureArray descriptorSet not implemented yet!");
				}
				else if (descriptorType == DescriptorTypes::storage_image)
				{
					ImageViewType viewType = descriptorReflection.GetImageDescriptor()->imageViewType;;
					if (viewType == ImageViewTypes::view_type_1d) throw std::runtime_error("Initialization for storage Texture1d descriptorSet not implemented yet!");
					else if (viewType == ImageViewTypes::view_type_2d) InitTextureBinding(frameIndex, binding, static_cast<Texture2d*>(DefaultGpuResources::GetDefaultStorageTexture2d()), descriptorType);
					else if (viewType == ImageViewTypes::view_type_3d) throw std::runtime_error("Initialization for storage Texture3d descriptorSet not implemented yet!");
					else if (viewType == ImageViewTypes::view_type_cube) throw std::runtime_error("Initialization for storage CubeTexture descriptorSet not implemented yet!");
					else if (viewType == ImageViewTypes::view_type_1d_array) throw std::runtime_error("Initialization storage for Texture1dArray descriptorSet not implemented yet!");
					else if (viewType == ImageViewTypes::view_type_2d_array) throw std::runtime_error("Initialization storage for Texture2dArray descriptorSet not implemented yet!");
					else if (viewType == ImageViewTypes::view_type_cube_array) throw std::runtime_error("Initialization storage for CubeTextureArray descriptorSet not implemented yet!");
				}
				else if (descriptorType == DescriptorTypes::storage_buffer)
					InitBufferBinding(frameIndex, binding, static_cast<Buffer*>(DefaultGpuResources::GetDefaultStorageBuffer()), descriptorType);
				else
					throw std::runtime_error("DescriptorSetBinding::DescriptorSetBinding(Shader*) shader contains currently unsupported DescriptorType:" + DescriptorTypes::ToString(descriptorType) + "!");
			}
		}
		InitStagingMaps();
		InitDescriptorSets();

		// Set default values:
		// GLOBAL_SET_INDEX
		ShadowRenderPass* pShadowRenderPass = RenderPassManager::GetShadowRenderPass();
		SetTexture("shadowMaps", static_cast<Texture*>(pShadowRenderPass->GetShadowMaps()));
		// SHADER_SET_INDEX
		SetTexture("normalMap", static_cast<Texture*>(DefaultGpuResources::GetNormalMapSampleTexture2d()));
		SetValue("SurfaceProperties", "diffuseColor", Float4::white);
		SetValue("SurfaceProperties", "roughness", 0.5f);
		SetValue("SurfaceProperties", "reflectivity", Float3(0.4f, 0.4f, 0.4f));
		SetValue("SurfaceProperties", "metallicity", 0);
		SetValue("SurfaceProperties", "scaleOffset", Float4(1, 1, 0, 0));
	}
	DescriptorSetBinding::~DescriptorSetBinding()
	{
		// Queue the destruction of each descriptor set for later collection:
		for (uint32_t i = 0; i < Context::GetFramesInFlight(); i++)
		{
			VkDescriptorSet descriptorSet = m_descriptorSets[i];
			GarbageCollector::RecordGarbage([descriptorSet]()
			{
				vkFreeDescriptorSets(Context::GetVkDevice(), Context::GetVkDescriptorPool(),1, &descriptorSet);
			});
		}
	}



	// Movable:
	DescriptorSetBinding::DescriptorSetBinding(DescriptorSetBinding&& other) noexcept = default;
	DescriptorSetBinding& DescriptorSetBinding::operator=(DescriptorSetBinding&& other) noexcept = default;



	// Setters:
	void DescriptorSetBinding::SetTexture(const std::string& name, emberBackendInterface::ITexture* pTexture)
	{
		// If texture with 'name' doesn't exist, skip:
		const uint32_t* pBinding = FindBindingIndex(name);
		if (!pBinding)
			return;
		auto it = m_textureStagingMap.find(*pBinding);
		if (it == m_textureStagingMap.end())
			return;
		it->second = static_cast<Texture*>(pTexture);
	}
	void DescriptorSetBinding::SetBuffer(const std::string& name, emberBackendInterface::IBuffer* pBuffer)
	{
		// If buffer with 'name' doesn't exist, skip:
		const uint32_t* pBinding = FindBindingIndex(name);
		if (!pBinding)
			return;
		auto it = m_bufferStagingMap.find(*pBinding);
		if (it == m_bufferStagingMap.end())
			return;
		it->second = static_cast<Buffer*>(pBuffer);
	}



	// Uniform Buffer Setters:
	// Simple members:
	void DescriptorSetBinding::SetInt(const std::string& bufferName, const std::string& memberName, int value)
	{
		SetValue(bufferName, memberName, value);
	}
	void DescriptorSetBinding::SetBool(const std::string& bufferName, const std::string& memberName, bool value)
	{
		SetValue(bufferName, memberName, value);
	}
	void DescriptorSetBinding::SetFloat(const std::string& bufferName, const std::string& memberName, float value)
	{
		SetValue(bufferName, memberName, value);
	}
	void DescriptorSetBinding::SetFloat2(const std::string& bufferName, const std::string& memberName, const Float2& value)
	{
		SetValue(bufferName, memberName, value);
	}
	void DescriptorSetBinding::SetFloat3(const std::string& bufferName, const std::string& memberName, const Float3& value)
	{
		SetValue(bufferName, memberName, value);
	}
	void DescriptorSetBinding::SetFloat4(const std::string& bufferName, const std::string& memberName, const Float4& value)
	{
		SetValue(bufferName, memberName, value);
	}
	void DescriptorSetBinding::SetFloat4x4(const std::string& bufferName, const std::string& memberName, const Float4x4& value)
	{
		SetValue(bufferName, memberName, value);
	}
	// Array members:
	void DescriptorSetBinding::SetInt(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, int value)
	{
		SetValue(bufferName, arrayName, arrayIndex, value);
	}
	void DescriptorSetBinding::SetBool(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, bool value)
	{
		SetValue(bufferName, arrayName, arrayIndex, value);
	}
	void DescriptorSetBinding::SetFloat(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, float value)
	{
		SetValue(bufferName, arrayName, arrayIndex, value);
	}
	void DescriptorSetBinding::SetFloat2(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const Float2& value)
	{
		SetValue(bufferName, arrayName, arrayIndex, value);
	}
	void DescriptorSetBinding::SetFloat3(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const Float3& value)
	{
		SetValue(bufferName, arrayName, arrayIndex, value);
	}
	void DescriptorSetBinding::SetFloat4(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const Float4& value)
	{
		SetValue(bufferName, arrayName, arrayIndex, value);
	}
	void DescriptorSetBinding::SetFloat4x4(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const Float4x4& value)
	{
		SetValue(bufferName, arrayName, arrayIndex, value);
	}
	// Array of structs members:
	void DescriptorSetBinding::SetInt(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, int value)
	{
		SetValue(bufferName, arrayName, arrayIndex, memberName, value);
	}
	void DescriptorSetBinding::SetBool(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, bool value)
	{
		SetValue(bufferName, arrayName, arrayIndex, memberName, value);
	}
	void DescriptorSetBinding::SetFloat(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, float value)
	{
		SetValue(bufferName, arrayName, arrayIndex, memberName, value);
	}
	void DescriptorSetBinding::SetFloat2(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float2& value)
	{
		SetValue(bufferName, arrayName, arrayIndex, memberName, value);
	}
	void DescriptorSetBinding::SetFloat3(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float3& value)
	{
		SetValue(bufferName, arrayName, arrayIndex, memberName, value);
	}
	void DescriptorSetBinding::SetFloat4(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float4& value)
	{
		SetValue(bufferName, arrayName, arrayIndex, memberName, value);
	}
	void DescriptorSetBinding::SetFloat4x4(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float4x4& value)
	{
		SetValue(bufferName, arrayName, arrayIndex, memberName, value);
	}
	// Array of arrays members:
	void DescriptorSetBinding::SetInt(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, int value)
	{
		SetValue(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex, value);
	}
	void DescriptorSetBinding::SetBool(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, bool value)
	{
		SetValue(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex, value);
	}
	void DescriptorSetBinding::SetFloat(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, float value)
	{
		SetValue(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex, value);
	}
	void DescriptorSetBinding::SetFloat2(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float2& value)
	{
		SetValue(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex, value);
	}
	void DescriptorSetBinding::SetFloat3(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float3& value)
	{
		SetValue(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex, value);
	}
	void DescriptorSetBinding::SetFloat4(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float4& value)
	{
		SetValue(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex, value);
	}
	void DescriptorSetBinding::SetFloat4x4(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float4x4& value)
	{
		SetValue(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex, value);
	}



	// Uniform Buffer Getters:
	// Simple members:
	int DescriptorSetBinding::GetInt(const std::string& bufferName, const std::string& memberName) const
	{
		return GetValue<int>(bufferName, memberName);
	}
	bool DescriptorSetBinding::GetBool(const std::string& bufferName, const std::string& memberName) const
	{
		return GetValue<bool>(bufferName, memberName);
	}
	float DescriptorSetBinding::GetFloat(const std::string& bufferName, const std::string& memberName) const
	{
		return GetValue<float>(bufferName, memberName);
	}
	Float2 DescriptorSetBinding::GetFloat2(const std::string& bufferName, const std::string& memberName) const
	{
		return GetValue<Float2>(bufferName, memberName);
	}
	Float3 DescriptorSetBinding::GetFloat3(const std::string& bufferName, const std::string& memberName) const
	{
		return GetValue<Float3>(bufferName, memberName);
	}
	Float4 DescriptorSetBinding::GetFloat4(const std::string& bufferName, const std::string& memberName) const
	{
		return GetValue<Float4>(bufferName, memberName);
	}
	Float4x4 DescriptorSetBinding::GetFloat4x4(const std::string& bufferName, const std::string& memberName) const
	{
		return GetValue<Float4x4>(bufferName, memberName);
	}
	// Array members:
	int DescriptorSetBinding::GetInt(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex) const
	{
		return GetValue<int>(bufferName, arrayName, arrayIndex);
	}
	bool DescriptorSetBinding::GetBool(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex) const
	{
		return GetValue<bool>(bufferName, arrayName, arrayIndex);
	}
	float DescriptorSetBinding::GetFloat(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex) const
	{
		return GetValue<float>(bufferName, arrayName, arrayIndex);
	}
	Float2 DescriptorSetBinding::GetFloat2(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex) const
	{
		return GetValue<Float2>(bufferName, arrayName, arrayIndex);
	}
	Float3 DescriptorSetBinding::GetFloat3(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex) const
	{
		return GetValue<Float3>(bufferName, arrayName, arrayIndex);
	}
	Float4 DescriptorSetBinding::GetFloat4(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex) const
	{
		return GetValue<Float4>(bufferName, arrayName, arrayIndex);
	}
	Float4x4 DescriptorSetBinding::GetFloat4x4(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex) const
	{
		return GetValue<Float4x4>(bufferName, arrayName, arrayIndex);
	}
	// Array of structs members:
	int DescriptorSetBinding::GetInt(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const
	{
		return GetValue<int>(bufferName, arrayName, arrayIndex, memberName);
	}
	bool DescriptorSetBinding::GetBool(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const
	{
		return GetValue<bool>(bufferName, arrayName, arrayIndex, memberName);
	}
	float DescriptorSetBinding::GetFloat(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const
	{
		return GetValue<float>(bufferName, arrayName, arrayIndex, memberName);
	}
	Float2 DescriptorSetBinding::GetFloat2(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const
	{
		return GetValue<Float2>(bufferName, arrayName, arrayIndex, memberName);
	}
	Float3 DescriptorSetBinding::GetFloat3(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const
	{
		return GetValue<Float3>(bufferName, arrayName, arrayIndex, memberName);
	}
	Float4 DescriptorSetBinding::GetFloat4(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const
	{
		return GetValue<Float4>(bufferName, arrayName, arrayIndex, memberName);
	}
	Float4x4 DescriptorSetBinding::GetFloat4x4(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const
	{
		return GetValue<Float4x4>(bufferName, arrayName, arrayIndex, memberName);
	}
	// Array of arrays members:
	int DescriptorSetBinding::GetInt(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const
	{
		return GetValue<int>(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex);
	}
	bool DescriptorSetBinding::GetBool(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const
	{
		return GetValue<bool>(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex);
	}
	float DescriptorSetBinding::GetFloat(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const
	{
		return GetValue<float>(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex);
	}
	Float2 DescriptorSetBinding::GetFloat2(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const
	{
		return GetValue<Float2>(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex);
	}
	Float3 DescriptorSetBinding::GetFloat3(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const
	{
		return GetValue<Float3>(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex);
	}
	Float4 DescriptorSetBinding::GetFloat4(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const
	{
		return GetValue<Float4>(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex);
	}
	Float4x4 DescriptorSetBinding::GetFloat4x4(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const
	{
		return GetValue<Float4x4>(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex);
	}



	// Getters:
	Shader* DescriptorSetBinding::GetShader() const
	{
		return m_pShader;
	}
	uint32_t DescriptorSetBinding::GetSetIndex() const
	{
		return m_setIndex;
	}
	uint64_t DescriptorSetBinding::GetGeneration() const
	{
		return m_generation;
	}
	bool DescriptorSetBinding::HasBinding(const std::string& name) const
	{
		return m_bindingIndices.contains(name);
	}
	std::string DescriptorSetBinding::GetShaderName() const
	{
		return m_pShader->GetName();
	}
	Texture* DescriptorSetBinding::GetTexture(const std::string& name) const
	{
        // Texture not found, return null:
		const uint32_t* pBinding = FindBindingIndex(name);
		if (!pBinding)
			return nullptr;
		auto it = m_textureMaps[Context::GetFrameIndex()].find(*pBinding);
		if (it != m_textureMaps[Context::GetFrameIndex()].end())
			return it->second.pTexture;
		return nullptr;
	}



	// Backend functionality:
	void DescriptorSetBinding::InvalidateBorrowedHandles()
	{
		m_generation++;
	}
	void DescriptorSetBinding::UpdateShaderData(uint32_t frameIndex)
	{
		// Stream uniform buffer data from host memory to GPU only for current frameIndex:
		for (auto& [binding, uniformBufferBinding] : m_uniformBufferMap)
			uniformBufferBinding.uniformBuffer.UpdateBuffer(frameIndex);

		// Change the pointer the descriptor set points at to the new texture:
		for (auto& [binding, textureBinding] : m_textureMaps[frameIndex])
		{
			Texture* pStagedTexture = m_textureStagingMap.at(binding);
			if (textureBinding.pTexture != pStagedTexture)
			{
				textureBinding.pTexture = pStagedTexture;
				UpdateDescriptorSet(frameIndex, textureBinding);
			}
		}

		// Change the pointer the descriptor set points at to the new buffer:
		for (auto& [binding, bufferBinding] : m_bufferMaps[frameIndex])
		{
			Buffer* pStagedBuffer = m_bufferStagingMap.at(binding);
			if (bufferBinding.pBuffer != pStagedBuffer)
			{
				bufferBinding.pBuffer = pStagedBuffer;
				UpdateDescriptorSet(frameIndex, bufferBinding);
			}
		}
	}
	const VkDescriptorSet& DescriptorSetBinding::GetVkDescriptorSet(uint32_t frameIndex) const
	{
		return m_descriptorSets[frameIndex];
	}
	const std::vector<VkDescriptorSet>& DescriptorSetBinding::GetVkDescriptorSets() const
	{
		return m_descriptorSets;
	}



	// Debugging:
	void DescriptorSetBinding::Print() const
	{
		LOG_INFO("DescriptorSetBinding: {}, {}", m_pShader->GetName(), fmt::ptr(this));
		LOG_INFO("DescriptorSets: {}, {}", fmt::ptr(m_descriptorSets[0]), fmt::ptr(m_descriptorSets[1]));
	}
	void DescriptorSetBinding::PrintMaps() const
	{
		for (uint32_t frameIndex = 0; frameIndex < Context::GetFramesInFlight(); frameIndex++)
		{
			 LOG_INFO("UniformBufferMaps[{}]:", frameIndex);
			 for (const auto& [binding, uniformBufferBinding] : m_uniformBufferMap)
			 	LOG_TRACE("binding: {}, bindingName: {}", uniformBufferBinding.binding, m_bindingNames.at(binding));

			 LOG_INFO("TextureMaps[{}]:", frameIndex);
			 for (const auto& [binding, textureBinding] : m_textureMaps[frameIndex])
			 	LOG_TRACE("binding: {}, bindingName: {}", textureBinding.binding, m_bindingNames.at(binding));
			 
			 LOG_INFO("BufferMaps[{}]:", frameIndex);
			 for (const auto& [binding, bufferBinding] : m_bufferMaps[frameIndex])
			 	LOG_TRACE("binding: {}, bindingName: {}, bufferSize: {}", bufferBinding.binding, m_bindingNames.at(binding), bufferBinding.pBuffer->GetSize());
			
			LOG_TRACE("\n");
		}
	}



	// Private methods:
	const uint32_t* DescriptorSetBinding::FindBindingIndex(const std::string& name) const
	{
		auto it = m_bindingIndices.find(name);
		if (it != m_bindingIndices.end())
			return &it->second;
        LOG_WARN("Shader {} does not have a binding names {}", m_pShader->GetName(), name);
		return nullptr;
	}


    
	// Initializers:
	void DescriptorSetBinding::InitUniformBufferBinding(uint32_t binding, const emberBufferLayout::BufferLayout& bufferLayout)
	{
		if (m_uniformBufferMap.contains(binding))
			return; // already initialized.

		UniformBuffer uniformBuffer(bufferLayout);
		m_uniformBufferMap.emplace(binding, UniformBufferBinding{binding, std::move(uniformBuffer)});
	}
	void DescriptorSetBinding::InitTextureBinding(uint32_t frameIndex, uint32_t binding, Texture* pTexture, DescriptorType descriptorType)
	{
		auto it = m_textureMaps[frameIndex].find(binding);
		if (it == m_textureMaps[frameIndex].end())
			m_textureMaps[frameIndex].emplace(binding, TextureBinding{binding, pTexture, descriptorType});
	}
	void DescriptorSetBinding::InitBufferBinding(uint32_t frameIndex, uint32_t binding, Buffer* pBuffer, DescriptorType descriptorType)
	{
		auto it = m_bufferMaps[frameIndex].find(binding);
		if (it == m_bufferMaps[frameIndex].end())
			m_bufferMaps[frameIndex].emplace(binding, BufferBinding{binding, pBuffer, descriptorType});
	}
	void DescriptorSetBinding::InitStagingMaps()
	{
		for (auto& [binding, textureBinding] : m_textureMaps[0])
			m_textureStagingMap.emplace(binding, textureBinding.pTexture);
		for (auto& [binding, bufferBinding] : m_bufferMaps[0])
			m_bufferStagingMap.emplace(binding, bufferBinding.pBuffer);
	}
	void DescriptorSetBinding::InitDescriptorSets()
	{
		CreateDescriptorSets();
		for (uint32_t frameIndex = 0; frameIndex < Context::GetFramesInFlight(); frameIndex++)
		{
			for (auto& [_, uniformBufferBinding] : m_uniformBufferMap)
				UpdateDescriptorSet(frameIndex, uniformBufferBinding);
			for (auto& [_, textureBinding] : m_textureMaps[frameIndex])
				UpdateDescriptorSet(frameIndex, textureBinding);
			for (auto& [_, bufferBinding] : m_bufferMaps[frameIndex])
				UpdateDescriptorSet(frameIndex, bufferBinding);
		}
	}



	// Descriptor Set management:
	void DescriptorSetBinding::CreateDescriptorSets()
	{
		std::vector<VkDescriptorSetLayout> layouts(Context::GetFramesInFlight(), m_pShader->GetVkDescriptorSetLayout()[m_setIndex]);	// same layout for all frames

		VkDescriptorSetAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
		allocInfo.descriptorPool = Context::GetVkDescriptorPool();
		allocInfo.descriptorSetCount = Context::GetFramesInFlight();
		allocInfo.pSetLayouts = layouts.data();

		m_descriptorSets.resize(Context::GetFramesInFlight());
		VKA(vkAllocateDescriptorSets(Context::GetLogicalDevice()->GetVkDevice(), &allocInfo, m_descriptorSets.data()));
	}
	void DescriptorSetBinding::UpdateDescriptorSet(uint32_t frameIndex, const UniformBufferBinding& uniformBufferBinding)
	{
		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = uniformBufferBinding.uniformBuffer.GetVmaBuffer()->GetVkBuffer();
		bufferInfo.offset = 0;
		bufferInfo.range = uniformBufferBinding.uniformBuffer.GetSize();

		VkWriteDescriptorSet descriptorWrite = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
		descriptorWrite.dstSet = m_descriptorSets[frameIndex];
		descriptorWrite.dstBinding = uniformBufferBinding.binding;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;
		descriptorWrite.pImageInfo = nullptr;
		descriptorWrite.pTexelBufferView = nullptr;

		vkUpdateDescriptorSets(Context::GetVkDevice(), 1, &descriptorWrite, 0, nullptr);
	}
	void DescriptorSetBinding::UpdateDescriptorSet(uint32_t frameIndex, const TextureBinding& textureBinding)
	{
		VkDescriptorImageInfo imageInfo = {};
		imageInfo.imageLayout = static_cast<VkImageLayout>(textureBinding.pTexture->GetVmaImage()->GetImageLayout());
		imageInfo.imageView = textureBinding.pTexture->GetVmaImage()->GetVkImageView();

		VkWriteDescriptorSet descriptorWrite = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
		descriptorWrite.dstSet = m_descriptorSets[frameIndex];
		descriptorWrite.dstBinding = textureBinding.binding;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = static_cast<VkDescriptorType>(textureBinding.descriptorType);
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = nullptr;
		descriptorWrite.pImageInfo = &imageInfo;
		descriptorWrite.pTexelBufferView = nullptr;

		vkUpdateDescriptorSets(Context::GetVkDevice(), 1, &descriptorWrite, 0, nullptr);
	}
	void DescriptorSetBinding::UpdateDescriptorSet(uint32_t frameIndex, const BufferBinding& bufferBinding)
	{
		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = bufferBinding.pBuffer->GetVmaBuffer()->GetVkBuffer();
		bufferInfo.offset = 0;
		bufferInfo.range = bufferBinding.pBuffer->GetSize();

		VkWriteDescriptorSet descriptorWrite = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
		descriptorWrite.dstSet = m_descriptorSets[frameIndex];
		descriptorWrite.dstBinding = bufferBinding.binding;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = static_cast<VkDescriptorType>(bufferBinding.descriptorType);
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;
		descriptorWrite.pImageInfo = nullptr;
		descriptorWrite.pTexelBufferView = nullptr;

		vkUpdateDescriptorSets(Context::GetVkDevice(), 1, &descriptorWrite, 0, nullptr);
	}



	// Getter templates, used in actual getters:
	template<typename T>
	T DescriptorSetBinding::GetValue(const std::string& bufferName, const std::string& memberName) const
	{
		const uint32_t* pBinding = FindBindingIndex(bufferName);
		if (!pBinding)
			return T();

		auto it = m_uniformBufferMap.find(*pBinding);
		if (it != m_uniformBufferMap.end())
			return it->second.uniformBuffer.GetValue<T>(memberName);
		return T();
	}
	template<typename T>
	T DescriptorSetBinding::GetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex) const
	{
		const uint32_t* pBinding = FindBindingIndex(bufferName);
		if (!pBinding)
			return T();

		auto it = m_uniformBufferMap.find(*pBinding);
		if (it != m_uniformBufferMap.end())
			return it->second.uniformBuffer.GetValue<T>(arrayName, arrayIndex);
		return T();
	}
	template<typename T>
	T DescriptorSetBinding::GetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const
	{
		const uint32_t* pBinding = FindBindingIndex(bufferName);
		if (!pBinding)
			return T();

		auto it = m_uniformBufferMap.find(*pBinding);
		if (it != m_uniformBufferMap.end())
			return it->second.uniformBuffer.GetValue<T>(arrayName, arrayIndex, memberName);
		return T();
	}
	template<typename T>
	T DescriptorSetBinding::GetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const
	{
		const uint32_t* pBinding = FindBindingIndex(bufferName);
		if (!pBinding)
			return T();

		auto it = m_uniformBufferMap.find(*pBinding);
		if (it != m_uniformBufferMap.end())
			return it->second.uniformBuffer.GetValue<T>(arrayName, arrayIndex, subArrayName, subArrayIndex);
		return T();
	}



	// Setter templates, used in actual setters:
	template<typename T>
	void DescriptorSetBinding::SetValue(const std::string& bufferName, const std::string& memberName, const T& value)
	{
		const uint32_t* pBinding = FindBindingIndex(bufferName);
		if (!pBinding)
			return;

		auto it = m_uniformBufferMap.find(*pBinding);
		if (it != m_uniformBufferMap.end())
			it->second.uniformBuffer.SetValue(memberName, value);
	}
	template<typename T>
	void DescriptorSetBinding::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const T& value)
	{
		const uint32_t* pBinding = FindBindingIndex(bufferName);
		if (!pBinding)
			return;

		auto it = m_uniformBufferMap.find(*pBinding);
		if (it != m_uniformBufferMap.end())
			it->second.uniformBuffer.SetValue(arrayName, arrayIndex, value);
	}
	template<typename T>
	void DescriptorSetBinding::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const T& value)
	{
		const uint32_t* pBinding = FindBindingIndex(bufferName);
		if (!pBinding)
			return;

		auto it = m_uniformBufferMap.find(*pBinding);
		if (it != m_uniformBufferMap.end())
			it->second.uniformBuffer.SetValue(arrayName, arrayIndex, memberName, value);
	}
	template<typename T>
	void DescriptorSetBinding::SetValue(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const T& value)
	{
		const uint32_t* pBinding = FindBindingIndex(bufferName);
		if (!pBinding)
			return;

		auto it = m_uniformBufferMap.find(*pBinding);
		if (it != m_uniformBufferMap.end())
			it->second.uniformBuffer.SetValue(arrayName, arrayIndex, subArrayName, subArrayIndex, value);
	}
}