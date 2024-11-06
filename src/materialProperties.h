#pragma once
#ifndef __INCLUDE_GUARD_materialProperties_h__
#define __INCLUDE_GUARD_materialProperties_h__
#include <string>
#include "vulkanContext.h"
#include "forwardPipeline.h"
#include "shadowPipeline.h"
#include "vulkanUniformBuffer.h"
#include "vulkanSampler.h"
#include "texture2d.h"



template<typename T>
struct ResourceBinding
{
	uint32_t binding;
	T resource;
};



/// <summary>
/// Each MaterialProperties instance is customized for a specific material.
/// Therefore, do not create one yourself!
/// Either use material.GetNewMaterialProperties() or materialProperties.GetCopy() to optain a new MaterialProperties pointer.
/// Tipp: Use smart pointers (std::unique_ptr, std::shared_ptr) to avoid manual deletion.
/// </summary>
class MaterialProperties
{
public: // Members:
	std::vector<VkDescriptorSet> descriptorSets;
	Pipeline* pipeline;

private: // Members:
	VulkanContext* context;
	std::vector<std::unordered_map<std::string, ResourceBinding<VulkanUniformBuffer>>> uniformBufferMaps;
	std::vector<std::unordered_map<std::string, ResourceBinding<VulkanSampler*>>> samplerMaps;
	std::vector<std::unordered_map<std::string, ResourceBinding<Texture2d*>>> texture2dMaps;

public: // Methods:
	// Constructors/Destructor:
	MaterialProperties(VulkanContext* context, Pipeline* pipeline, std::vector<VkDescriptorSetLayoutBinding>& bindings, std::vector<std::string>& bindingNames);
	~MaterialProperties();

	// Setters:
	void SetUniform(const std::string& name, const RenderMatrizes& renderMatrizes);
	void SetUniform(const std::string& name, const LightData& lightData);
	void SetSampler(const std::string& name, VulkanSampler* sampler);
	void SetSamplerForAllFrames(const std::string& name, VulkanSampler* sampler);
	void SetTexture2d(const std::string& name, Texture2d* texture2d);
	void SetTexture2dForAllFrames(const std::string& name, Texture2d* texture2d);

	// Getters:
	MaterialProperties* GetCopy();

	// Debugging:
	void PrintMaps() const;

private: // Methods:
	// Hidden constructor:
	MaterialProperties(VulkanContext* context, Pipeline* pipeline);

	// Initializers:
	void InitUniformBufferResourceBinding(std::string name, uint32_t binding, const RenderMatrizes& renderMatrizes, uint32_t frameIndex);
	void InitUniformBufferResourceBinding(std::string name, uint32_t binding, const LightData& lightData, uint32_t frameIndex);
	void InitSamplerResourceBinding(std::string name, uint32_t binding, VulkanSampler* sampler, uint32_t frameIndex);
	void InitTexture2dResourceBinding(std::string name, uint32_t binding, Texture2d* texture2d, uint32_t frameIndex);
	void InitDescriptorSets();

	// Descriptor set management:
	void CreateDescriptorSets();
	void UpdateDescriptorSet(uint32_t frameIndex, ResourceBinding<VulkanUniformBuffer> samplerResourceBinding);
	void UpdateDescriptorSet(uint32_t frameIndex, ResourceBinding<VulkanSampler*> samplerResourceBinding);
	void UpdateDescriptorSet(uint32_t frameIndex, ResourceBinding<Texture2d*> texture2dResourceBinding);
};



#endif // __INCLUDE_GUARD_materialProperties_h__