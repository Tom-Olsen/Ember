#include "vulkanShaderProperties.h"
#include "emberMath.h"
#include "logger.h"
#include "spirvReflect.h"
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
#include "vulkanSampler.h"
#include "vulkanSampleTexture2d.h"
#include "vulkanSampleTextureCube.h"
#include "vulkanStorageBuffer.h"
#include "vulkanStorageTexture2d.h"
#include "vulkanShader.h"
#include "vulkanShadowRenderPass.h"
#include "vulkanTexture.h"
#include "vulkanTexture2d.h"
#include "vulkanTextureCube.h"
#include "vulkanUniformBuffer.h"
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Public methods:
	// Consructor/Destructor:
	ShaderProperties::ShaderProperties(Shader* pShader)
	{
		m_pShader = pShader;

		// Create resource bindings and uniform buffer update logic for each frameInFlight:
		m_uniformBufferMaps = std::vector<std::unordered_map<std::string, UniformBufferBinding>>(Context::GetFramesInFlight());
		m_samplerMaps = std::vector<std::unordered_map<std::string, SamplerBinding>>(Context::GetFramesInFlight());
		m_textureMaps = std::vector<std::unordered_map<std::string, TextureBinding>>(Context::GetFramesInFlight());
		m_bufferMaps = std::vector<std::unordered_map<std::string, BufferBinding>>(Context::GetFramesInFlight());
		m_updateUniformBuffer = std::vector<std::unordered_map<std::string, bool>>(Context::GetFramesInFlight());

		// Initialize all resource bindings:
		for (uint32_t frameIndex = 0; frameIndex < Context::GetFramesInFlight(); frameIndex++)
		{
			const DescriptorBoundResources* const descriptorBoundResources = m_pShader->GetDescriptorBoundResources();
			for (uint32_t i = 0; i < descriptorBoundResources->bindingCount; i++)
			{
				DescriptorSetLayoutBinding descriptorSetLayoutBinding = descriptorBoundResources->descriptorSetLayoutBindings[i];
				DescriptorType descriptorType = descriptorSetLayoutBinding.descriptorType;
				uint32_t binding = descriptorSetLayoutBinding.binding;
				const std::string& name = descriptorBoundResources->descriptorSetBindingNames[i];

				if (descriptorType == DescriptorTypes::uniform_buffer)
					InitUniformBufferResourceBinding(frameIndex, name, binding);
				else if (descriptorType == DescriptorTypes::sampler)
				{// Samplers are 'static'. Correct one gets selected on initialization and can't be changed later.
					if (name == "colorSampler")
						InitSamplerResourceBinding(frameIndex, name, binding, DefaultGpuResources::GetColorSampler());
					else if (name == "shadowSampler")
						InitSamplerResourceBinding(frameIndex, name, binding, DefaultGpuResources::GetShadowSampler());
				}
				else if (descriptorType == DescriptorTypes::sampled_image)
				{
					ImageViewType viewType = descriptorBoundResources->sampleViewTypeMap.at(name);
					if (viewType == ImageViewTypes::view_type_1d) throw std::runtime_error("Initialization for sampling Texture1d descriptorSet not implemented yet!");
					else if (viewType == ImageViewTypes::view_type_2d) InitTextureResourceBinding(frameIndex, name, binding, static_cast<Texture*>(DefaultGpuResources::GetDefaultSampleTexture2d()), descriptorType);
					else if (viewType == ImageViewTypes::view_type_3d) throw std::runtime_error("Initialization for sampling Texture3d descriptorSet not implemented yet!");
					else if (viewType == ImageViewTypes::view_type_cube) InitTextureResourceBinding(frameIndex, name, binding, static_cast<Texture*>(DefaultGpuResources::GetDefaultSampleTextureCube()), descriptorType);
					else if (viewType == ImageViewTypes::view_type_1d_array) throw std::runtime_error("Initialization for sampling Texture1dArray descriptorSet not implemented yet!");
					else if (viewType == ImageViewTypes::view_type_2d_array) InitTextureResourceBinding(frameIndex, name, binding, static_cast<Texture*>(DefaultGpuResources::GetDefaultDepthTexture2dArray()), descriptorType);
					else if (viewType == ImageViewTypes::view_type_cube_array) throw std::runtime_error("Initialization for sampling CubeTextureArray descriptorSet not implemented yet!");
				}
				else if (descriptorType == DescriptorTypes::storage_image)
				{
					ImageViewType viewType = descriptorBoundResources->storageViewTypeMap.at(name);
					if (viewType == ImageViewTypes::view_type_1d) throw std::runtime_error("Initialization for storage Texture1d descriptorSet not implemented yet!");
					else if (viewType == ImageViewTypes::view_type_2d) InitTextureResourceBinding(frameIndex, name, binding, static_cast<Texture2d*>(DefaultGpuResources::GetDefaultStorageTexture2d()), descriptorType);
					else if (viewType == ImageViewTypes::view_type_3d) throw std::runtime_error("Initialization for storage Texture3d descriptorSet not implemented yet!");
					else if (viewType == ImageViewTypes::view_type_cube) throw std::runtime_error("Initialization for storage CubeTexture descriptorSet not implemented yet!");
					else if (viewType == ImageViewTypes::view_type_1d_array) throw std::runtime_error("Initialization storage for Texture1dArray descriptorSet not implemented yet!");
					else if (viewType == ImageViewTypes::view_type_2d_array) throw std::runtime_error("Initialization storage for Texture2dArray descriptorSet not implemented yet!");
					else if (viewType == ImageViewTypes::view_type_cube_array) throw std::runtime_error("Initialization storage for CubeTextureArray descriptorSet not implemented yet!");
				}
				else if (descriptorType == DescriptorTypes::storage_buffer)
					InitBufferResourceBinding(frameIndex, name, binding, static_cast<Buffer*>(DefaultGpuResources::GetDefaultStorageBuffer()), descriptorType);
				else
					throw std::runtime_error("ShaderProperties::ShaderProperties(Shader*) shader contains currently unsuported DescriptorType:" + DescriptorTypes::ToString(descriptorType) + "!");
			}
		}
		InitStagingMaps();
		InitDescriptorSets();

		// Set default values:
		ShadowRenderPass* pShadowRenderPass = RenderPassManager::GetShadowRenderPass();
		SetTexture("shadowMaps", static_cast<Texture*>(pShadowRenderPass->GetShadowMaps()));
		SetTexture("normalMap", static_cast<Texture*>(DefaultGpuResources::GetNormalMapSampleTexture2d()));
		SetValue("SurfaceProperties", "diffuseColor", Float4::white);
		SetValue("SurfaceProperties", "roughness", 0.5f);
		SetValue("SurfaceProperties", "reflectivity", Float3(0.4f, 0.4f, 0.4f));
		SetValue("SurfaceProperties", "metallicity", 0);
		SetValue("SurfaceProperties", "scaleOffset", Float4(1, 1, 0, 0));
	}
	ShaderProperties::~ShaderProperties()
	{
		// Queue the destruction of each descriptor set for later collection.
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
	ShaderProperties::ShaderProperties(ShaderProperties&& other) noexcept = default;
	ShaderProperties& ShaderProperties::operator=(ShaderProperties&& other) noexcept = default;



	// Setters:
	void ShaderProperties::SetTexture(const std::string& name, emberBackendInterface::ITexture* pTexture)
	{
		// If texture with 'name' doesnt exist, skip:
		auto it = m_textureStagingMap.find(name);
		if (it == m_textureStagingMap.end())
			return;
		it->second = static_cast<Texture*>(pTexture);
	}
	void ShaderProperties::SetBuffer(const std::string& name, emberBackendInterface::IBuffer* pBuffer)
	{
		// If buffer with 'name' doesnt exist, skip:
		auto it = m_bufferStagingMap.find(name);
		if (it == m_bufferStagingMap.end())
			return;
		it->second = static_cast<Buffer*>(pBuffer);
	}



	// Uniform Buffer Setters:
	// Simple members:
	void ShaderProperties::SetInt(const std::string& bufferName, const std::string& memberName, int value)
	{
		SetValue(bufferName, memberName, value);
	}
	void ShaderProperties::SetBool(const std::string& bufferName, const std::string& memberName, bool value)
	{
		SetValue(bufferName, memberName, value);
	}
	void ShaderProperties::SetFloat(const std::string& bufferName, const std::string& memberName, float value)
	{
		SetValue(bufferName, memberName, value);
	}
	void ShaderProperties::SetFloat2(const std::string& bufferName, const std::string& memberName, const Float2& value)
	{
		SetValue(bufferName, memberName, value);
	}
	void ShaderProperties::SetFloat3(const std::string& bufferName, const std::string& memberName, const Float3& value)
	{
		SetValue(bufferName, memberName, value);
	}
	void ShaderProperties::SetFloat4(const std::string& bufferName, const std::string& memberName, const Float4& value)
	{
		SetValue(bufferName, memberName, value);
	}
	void ShaderProperties::SetFloat4x4(const std::string& bufferName, const std::string& memberName, const Float4x4& value)
	{
		SetValue(bufferName, memberName, value);
	}
	// Array members:
	void ShaderProperties::SetInt(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, int value)
	{
		SetValue(bufferName, arrayName, arrayIndex, value);
	}
	void ShaderProperties::SetBool(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, bool value)
	{
		SetValue(bufferName, arrayName, arrayIndex, value);
	}
	void ShaderProperties::SetFloat(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, float value)
	{
		SetValue(bufferName, arrayName, arrayIndex, value);
	}
	void ShaderProperties::SetFloat2(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const Float2& value)
	{
		SetValue(bufferName, arrayName, arrayIndex, value);
	}
	void ShaderProperties::SetFloat3(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const Float3& value)
	{
		SetValue(bufferName, arrayName, arrayIndex, value);
	}
	void ShaderProperties::SetFloat4(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const Float4& value)
	{
		SetValue(bufferName, arrayName, arrayIndex, value);
	}
	void ShaderProperties::SetFloat4x4(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const Float4x4& value)
	{
		SetValue(bufferName, arrayName, arrayIndex, value);
	}
	// Array of structs members:
	void ShaderProperties::SetInt(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, int value)
	{
		SetValue(bufferName, arrayName, arrayIndex, memberName, value);
	}
	void ShaderProperties::SetBool(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, bool value)
	{
		SetValue(bufferName, arrayName, arrayIndex, memberName, value);
	}
	void ShaderProperties::SetFloat(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, float value)
	{
		SetValue(bufferName, arrayName, arrayIndex, memberName, value);
	}
	void ShaderProperties::SetFloat2(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float2& value)
	{
		SetValue(bufferName, arrayName, arrayIndex, memberName, value);
	}
	void ShaderProperties::SetFloat3(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float3& value)
	{
		SetValue(bufferName, arrayName, arrayIndex, memberName, value);
	}
	void ShaderProperties::SetFloat4(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float4& value)
	{
		SetValue(bufferName, arrayName, arrayIndex, memberName, value);
	}
	void ShaderProperties::SetFloat4x4(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName, const Float4x4& value)
	{
		SetValue(bufferName, arrayName, arrayIndex, memberName, value);
	}
	// Array of arrays members:
	void ShaderProperties::SetInt(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, int value)
	{
		SetValue(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex, value);
	}
	void ShaderProperties::SetBool(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, bool value)
	{
		SetValue(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex, value);
	}
	void ShaderProperties::SetFloat(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, float value)
	{
		SetValue(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex, value);
	}
	void ShaderProperties::SetFloat2(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float2& value)
	{
		SetValue(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex, value);
	}
	void ShaderProperties::SetFloat3(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float3& value)
	{
		SetValue(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex, value);
	}
	void ShaderProperties::SetFloat4(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float4& value)
	{
		SetValue(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex, value);
	}
	void ShaderProperties::SetFloat4x4(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex, const Float4x4& value)
	{
		SetValue(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex, value);
	}



	// Uniform Buffer Getters:
	// Simple members:
	int ShaderProperties::GetInt(const std::string& bufferName, const std::string& memberName) const
	{
		return GetValue<int>(bufferName, memberName);
	}
	bool ShaderProperties::GetBool(const std::string& bufferName, const std::string& memberName) const
	{
		return GetValue<bool>(bufferName, memberName);
	}
	float ShaderProperties::GetFloat(const std::string& bufferName, const std::string& memberName) const
	{
		return GetValue<float>(bufferName, memberName);
	}
	Float2 ShaderProperties::GetFloat2(const std::string& bufferName, const std::string& memberName) const
	{
		return GetValue<Float2>(bufferName, memberName);
	}
	Float3 ShaderProperties::GetFloat3(const std::string& bufferName, const std::string& memberName) const
	{
		return GetValue<Float3>(bufferName, memberName);
	}
	Float4 ShaderProperties::GetFloat4(const std::string& bufferName, const std::string& memberName) const
	{
		return GetValue<Float4>(bufferName, memberName);
	}
	Float4x4 ShaderProperties::GetFloat4x4(const std::string& bufferName, const std::string& memberName) const
	{
		return GetValue<Float4x4>(bufferName, memberName);
	}
	// Array members:
	int ShaderProperties::GetInt(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex) const
	{
		return GetValue<int>(bufferName, arrayName, arrayIndex);
	}
	bool ShaderProperties::GetBool(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex) const
	{
		return GetValue<bool>(bufferName, arrayName, arrayIndex);
	}
	float ShaderProperties::GetFloat(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex) const
	{
		return GetValue<float>(bufferName, arrayName, arrayIndex);
	}
	Float2 ShaderProperties::GetFloat2(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex) const
	{
		return GetValue<Float2>(bufferName, arrayName, arrayIndex);
	}
	Float3 ShaderProperties::GetFloat3(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex) const
	{
		return GetValue<Float3>(bufferName, arrayName, arrayIndex);
	}
	Float4 ShaderProperties::GetFloat4(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex) const
	{
		return GetValue<Float4>(bufferName, arrayName, arrayIndex);
	}
	Float4x4 ShaderProperties::GetFloat4x4(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex) const
	{
		return GetValue<Float4x4>(bufferName, arrayName, arrayIndex);
	}
	// Array of structs members:
	int ShaderProperties::GetInt(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const
	{
		return GetValue<int>(bufferName, arrayName, arrayIndex, memberName);
	}
	bool ShaderProperties::GetBool(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const
	{
		return GetValue<bool>(bufferName, arrayName, arrayIndex, memberName);
	}
	float ShaderProperties::GetFloat(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const
	{
		return GetValue<float>(bufferName, arrayName, arrayIndex, memberName);
	}
	Float2 ShaderProperties::GetFloat2(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const
	{
		return GetValue<Float2>(bufferName, arrayName, arrayIndex, memberName);
	}
	Float3 ShaderProperties::GetFloat3(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const
	{
		return GetValue<Float3>(bufferName, arrayName, arrayIndex, memberName);
	}
	Float4 ShaderProperties::GetFloat4(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const
	{
		return GetValue<Float4>(bufferName, arrayName, arrayIndex, memberName);
	}
	Float4x4 ShaderProperties::GetFloat4x4(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& memberName) const
	{
		return GetValue<Float4x4>(bufferName, arrayName, arrayIndex, memberName);
	}
	// Array of arrays members:
	int ShaderProperties::GetInt(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const
	{
		return GetValue<int>(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex);
	}
	bool ShaderProperties::GetBool(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const
	{
		return GetValue<bool>(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex);
	}
	float ShaderProperties::GetFloat(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const
	{
		return GetValue<float>(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex);
	}
	Float2 ShaderProperties::GetFloat2(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const
	{
		return GetValue<Float2>(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex);
	}
	Float3 ShaderProperties::GetFloat3(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const
	{
		return GetValue<Float3>(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex);
	}
	Float4 ShaderProperties::GetFloat4(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const
	{
		return GetValue<Float4>(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex);
	}
	Float4x4 ShaderProperties::GetFloat4x4(const std::string& bufferName, const std::string& arrayName, uint32_t arrayIndex, const std::string& subArrayName, uint32_t subArrayIndex) const
	{
		return GetValue<Float4x4>(bufferName, arrayName, arrayIndex, subArrayName, subArrayIndex);
	}



	// Getters:
	Shader* ShaderProperties::GetShader() const
	{
		return m_pShader;
	}
	std::string ShaderProperties::GetShaderName() const
	{
		return m_pShader->GetName();
	}
	Texture* ShaderProperties::GetTexture(const std::string& name) const
	{
		auto it = m_textureMaps[Context::GetFrameIndex()].find(name);
		if (it != m_textureMaps[Context::GetFrameIndex()].end())
			return it->second.pTexture;
		return nullptr;
	}



	// Backend functionality:
	void ShaderProperties::UpdateShaderData(uint32_t frameIndex)
	{
		// invalid frameIndex (-1 or >= framesInFlight) => use current frameIndex. frameIndex = -1 is default behaviour.
		if (frameIndex == -1 || frameIndex >= Context::GetFramesInFlight())
			frameIndex = Context::GetFrameIndex();

		// Stream uniform buffer data from host memory to GPU only for current frameIndex:
		for (auto& [name, uniformBufferBinding] : m_uniformBufferMaps[frameIndex])
		{
			if (m_updateUniformBuffer[frameIndex].at(name))
			{
				uniformBufferBinding.pUniformBuffer->UpdateBuffer();
				m_updateUniformBuffer[frameIndex].at(name) = false;
			}
		}

		// Change the pointer the descriptor set points at to the new sampler:
		for (auto& [name, samplerBinding] : m_samplerMaps[frameIndex])
		{
			if (samplerBinding.pSampler != m_samplerStagingMap.at(name))
			{
				samplerBinding.pSampler = m_samplerStagingMap.at(name);
				UpdateDescriptorSet(frameIndex, samplerBinding);
			}
		}

		// Change the pointer the descriptor set points at to the new texture:
		for (auto& [name, textureBinding] : m_textureMaps[frameIndex])
		{
			if (textureBinding.pTexture != m_textureStagingMap.at(name))
			{
				textureBinding.pTexture = m_textureStagingMap.at(name);
				UpdateDescriptorSet(frameIndex, textureBinding);
			}
		}

		// Change the pointer the descriptor set points at to the new buffer:
		for (auto& [name, bufferBinding] : m_bufferMaps[frameIndex])
		{
			if (bufferBinding.pBuffer != m_bufferStagingMap.at(name))
			{
				bufferBinding.pBuffer = m_bufferStagingMap.at(name);
				UpdateDescriptorSet(frameIndex, bufferBinding);
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



	// Debugging:
	void ShaderProperties::Print(const std::string& name) const
	{
		LOG_INFO("ShaderProperties: {}, {}", name, fmt::ptr(this));
		LOG_INFO("DescriptorSets: {}, {}", fmt::ptr(m_descriptorSets[0]), fmt::ptr(m_descriptorSets[1]));
	}
	void ShaderProperties::PrintMaps() const
	{
		for (uint32_t frameIndex = 0; frameIndex < Context::GetFramesInFlight(); frameIndex++)
		{
			 LOG_INFO("UniformBufferMaps[{}]:", frameIndex);
			 for (const auto& [name, uniformBufferBinding] : m_uniformBufferMaps[frameIndex])
			 	LOG_TRACE("binding: {}, bindingName: {}", uniformBufferBinding.binding, name);
			 
			 LOG_INFO("SamplerMaps[{}]:", frameIndex);
			 for (const auto& [name, samplerBinding] : m_samplerMaps[frameIndex])
			 	LOG_TRACE("binding: {}, bindingName: {}, samplerName: {}", samplerBinding.binding, name, samplerBinding.pSampler->GetName());
			 
			 LOG_INFO("TextureMaps[{}]:", frameIndex);
			 for (const auto& [name, textureBinding] : m_textureMaps[frameIndex])
			 	LOG_TRACE("binding: {}, bindingName: {}, textureName: {}", textureBinding.binding, name, textureBinding.pTexture->GetName());
			 
			 LOG_INFO("BufferMaps[{}]:", frameIndex);
			 for (const auto& [name, bufferBinding] : m_bufferMaps[frameIndex])
			 	LOG_TRACE("binding: {}, bindingName: {}, bufferSize: {}", bufferBinding.binding, name, bufferBinding.pBuffer->GetSize());
			
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

			std::shared_ptr<UniformBuffer> uniformBuffer = std::make_shared<UniformBuffer>(pUniformBufferBlock, m_pShader->GetName());
			m_uniformBufferMaps[frameIndex].emplace(name, UniformBufferBinding(binding, uniformBuffer));

			for (uint32_t frameIndex = 0; frameIndex < Context::GetFramesInFlight(); frameIndex++)
				m_updateUniformBuffer[frameIndex].emplace(name, true);
		}
	}
	void ShaderProperties::InitSamplerResourceBinding(uint32_t frameIndex, const std::string& name, uint32_t binding, Sampler* pSampler)
	{
		auto it = m_samplerMaps[frameIndex].find(name);
		if (it == m_samplerMaps[frameIndex].end())
			m_samplerMaps[frameIndex].emplace(name, SamplerBinding(binding, pSampler));
	}
	void ShaderProperties::InitTextureResourceBinding(uint32_t frameIndex, const std::string& name, uint32_t binding, Texture* pTexture, DescriptorType descriptorType)
	{
		auto it = m_textureMaps[frameIndex].find(name);
		if (it == m_textureMaps[frameIndex].end())
			m_textureMaps[frameIndex].emplace(name, TextureBinding(binding, pTexture, descriptorType));
	}
	void ShaderProperties::InitBufferResourceBinding(uint32_t frameIndex, const std::string& name, uint32_t binding, Buffer* pBuffer, DescriptorType descriptorType)
	{
		auto it = m_bufferMaps[frameIndex].find(name);
		if (it == m_bufferMaps[frameIndex].end())
			m_bufferMaps[frameIndex].emplace(name, BufferBinding(binding, pBuffer, descriptorType));
	}
	void ShaderProperties::InitStagingMaps()
	{
		for (auto& [name, samplerBinding] : m_samplerMaps[0])
			m_samplerStagingMap.emplace(name, samplerBinding.pSampler);
		for (auto& [name, textureBinding] : m_textureMaps[0])
			m_textureStagingMap.emplace(name, textureBinding.pTexture);
		for (auto& [name, bufferBinding] : m_bufferMaps[0])
			m_bufferStagingMap.emplace(name, bufferBinding.pBuffer);
	}
	void ShaderProperties::InitDescriptorSets()
	{
		CreateDescriptorSets();
		for (uint32_t frameIndex = 0; frameIndex < Context::GetFramesInFlight(); frameIndex++)
		{
			for (auto& [_, uniformBufferBinding] : m_uniformBufferMaps[frameIndex])
				UpdateDescriptorSet(frameIndex, uniformBufferBinding);
			for (auto& [_, samplerBinding] : m_samplerMaps[frameIndex])
				UpdateDescriptorSet(frameIndex, samplerBinding);
			for (auto& [_, textureBinding] : m_textureMaps[frameIndex])
				UpdateDescriptorSet(frameIndex, textureBinding);
			for (auto& [_, bufferBinding] : m_bufferMaps[frameIndex])
				UpdateDescriptorSet(frameIndex, bufferBinding);
		}
	}



	// Descriptor Set management:
	void ShaderProperties::CreateDescriptorSets()
	{
		std::vector<VkDescriptorSetLayout> layouts(Context::GetFramesInFlight(), m_pShader->GetPipeline()->GetVkDescriptorSetLayout());	// same layout for all frames

		VkDescriptorSetAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
		allocInfo.descriptorPool = Context::GetVkDescriptorPool();
		allocInfo.descriptorSetCount = Context::GetFramesInFlight();
		allocInfo.pSetLayouts = layouts.data();

		m_descriptorSets.resize(Context::GetFramesInFlight());
		VKA(vkAllocateDescriptorSets(Context::GetLogicalDevice()->GetVkDevice(), &allocInfo, m_descriptorSets.data()));
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

		vkUpdateDescriptorSets(Context::GetVkDevice(), 1, &descriptorWrite, 0, nullptr);
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

		vkUpdateDescriptorSets(Context::GetVkDevice(), 1, &descriptorWrite, 0, nullptr);
	}
	void ShaderProperties::UpdateDescriptorSet(uint32_t frameIndex, TextureBinding textureResourceBinding)
	{
		VkDescriptorImageInfo imageInfo = {};
		imageInfo.imageLayout = static_cast<VkImageLayout>(textureResourceBinding.pTexture->GetVmaImage()->GetImageLayout());
		imageInfo.imageView = textureResourceBinding.pTexture->GetVmaImage()->GetVkImageView();

		VkWriteDescriptorSet descriptorWrite = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
		descriptorWrite.dstSet = m_descriptorSets[frameIndex];
		descriptorWrite.dstBinding = textureResourceBinding.binding;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = static_cast<VkDescriptorType>(textureResourceBinding.descriptorType);
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = nullptr;
		descriptorWrite.pImageInfo = &imageInfo;
		descriptorWrite.pTexelBufferView = nullptr;

		vkUpdateDescriptorSets(Context::GetVkDevice(), 1, &descriptorWrite, 0, nullptr);
	}
	void ShaderProperties::UpdateDescriptorSet(uint32_t frameIndex, BufferBinding bufferResourceBinding)
	{
		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = bufferResourceBinding.pBuffer->GetVmaBuffer()->GetVkBuffer();
		bufferInfo.offset = 0;
		bufferInfo.range = bufferResourceBinding.pBuffer->GetSize();

		VkWriteDescriptorSet descriptorWrite = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
		descriptorWrite.dstSet = m_descriptorSets[frameIndex];
		descriptorWrite.dstBinding = bufferResourceBinding.binding;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = static_cast<VkDescriptorType>(bufferResourceBinding.descriptorType);
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;
		descriptorWrite.pImageInfo = nullptr;
		descriptorWrite.pTexelBufferView = nullptr;

		vkUpdateDescriptorSets(Context::GetVkDevice(), 1, &descriptorWrite, 0, nullptr);
	}



	// Getter templates, used in actual getters:
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



	// Setter templates, used in actual setters:
	template<typename T>
	void ShaderProperties::SetValue(const std::string& bufferName, const std::string& memberName, const T& value)
	{
		auto it = m_uniformBufferMaps[0].find(bufferName);
		if (it != m_uniformBufferMaps[0].end())
		{// bufferName exists => set value for all frames:
			for (uint32_t frameIndex = 0; frameIndex < Context::GetFramesInFlight(); frameIndex++)
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
			for (uint32_t frameIndex = 0; frameIndex < Context::GetFramesInFlight(); frameIndex++)
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
			for (uint32_t frameIndex = 0; frameIndex < Context::GetFramesInFlight(); frameIndex++)
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
			for (uint32_t frameIndex = 0; frameIndex < Context::GetFramesInFlight(); frameIndex++)
			{
				if (m_uniformBufferMaps[frameIndex].at(bufferName).pUniformBuffer->SetValue(arrayName, arrayIndex, subArrayName, subArrayIndex, value))
					m_updateUniformBuffer[frameIndex].at(bufferName) = true;
			}
		}
	}
}