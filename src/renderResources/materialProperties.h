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
#include "material.h"



template<typename T>
struct ResourceBinding
{
	T resource;
	uint32_t binding;
};



/// <summary>
/// Each MaterialProperties instance is customized for a specific material.
/// MaterialProperties construction is expensive, do not create them in an update loop.
/// </summary>
class MaterialProperties
{
public: // Members:
	std::vector<VkDescriptorSet> descriptorSets;

private: // Members:
	Material* material;
	VulkanContext* context;
	std::vector<std::unordered_map<std::string, ResourceBinding<VulkanUniformBuffer>>> uniformBufferMaps;
	std::vector<std::unordered_map<std::string, ResourceBinding<VulkanSampler*>>> samplerMaps;
	std::vector<std::unordered_map<std::string, ResourceBinding<Texture2d*>>> texture2dMaps;
	std::unordered_map<std::string, bool> updateUniformBuffer;

public: // Methods:
	// Constructors/Destructor:
	MaterialProperties(Material* material);
	~MaterialProperties();

	void UpdateUniformBuffers(uint32_t frameIndex);

	// Setters:
	template<typename T>
	void SetValue(const std::string& blockName, const std::string& memberName, const T& value);
	template<typename T>
	void SetValue(const std::string& blockName, const std::string& memberName, uint32_t arrayindex, const T& value);
	void SetSampler(const std::string& name, VulkanSampler* sampler);
	void SetSamplerForAllFrames(const std::string& name, VulkanSampler* sampler);
	void SetTexture2d(const std::string& name, Texture2d* texture2d);
	void SetTexture2dForAllFrames(const std::string& name, Texture2d* texture2d);

	// Debugging:
	void PrintMaps() const;

private: // Methods:
	// Initializers:
	void InitUniformBufferResourceBinding(std::string name, uint32_t binding, uint32_t frameIndex);
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