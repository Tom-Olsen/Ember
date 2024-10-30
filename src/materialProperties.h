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
	std::vector<std::string> frameNames;
};



/// <summary>
/// Each MaterialProperties instance is customized for a specific material.
/// Therefore, do not create one yourself!
/// Always use material.GetMaterialPropertiesCopy() to optain a new MaterialProperties pointer.
/// Tipp: Use smart pointers (std::unique_ptr, std::shared_ptr) to avoid manual deletion.
/// </summary>
class MaterialProperties
{
public: // Members:
	std::unordered_map<std::string, ResourceBinding<VulkanUniformBuffer>> uniformBufferMap;
	std::unordered_map<std::string, ResourceBinding<VulkanSampler*>> samplerMap;
	std::unordered_map<std::string, ResourceBinding<Texture2d*>> texture2dMap;
	std::vector<VkDescriptorSet> descriptorSets;
	Pipeline* pipeline;

private: // Members:
	VulkanContext* context;
	std::vector<VkWriteDescriptorSet> descriptorWrites;

public: // Methods:
	// Constructors/Destructor:
	MaterialProperties(VulkanContext* context);
	~MaterialProperties();

	// Initializers:
	void InitUniformResourceBinding(std::string name, uint32_t binding, const RenderMatrizes& renderMatrizes);
	void InitSamplerResourceBinding(std::string name, uint32_t binding, VulkanSampler* sampler);
	void InitTexture2dResourceBinding(std::string name, uint32_t binding, Texture2d* texture2d);
	void InitDescriptorSets();

	// Setters:
	void SetPipeline(Pipeline* pipeline);
	void SetUniform(const std::string& name, const RenderMatrizes& renderMatrizes);
	void SetSampler(const std::string& name, VulkanSampler* sampler);
	void SetSamplerForAllFrames(const std::string& name, VulkanSampler* sampler);
	void SetTexture2d(const std::string& name, Texture2d* texture2d);
	void SetTexture2dForAllFrames(const std::string& name, Texture2d* texture2d);

	// Getters:
	MaterialProperties* GetCopy();

	// Debugging:
	void PrintMaps() const;

private: // Methods:
	void CreateDescriptorSets();
	void UpdateAllDescriptorSets(uint32_t frameIndex);
	void UpdateUniformDescriptorSets(uint32_t frameIndex);
	void UpdateSamplerDescriptorSets(uint32_t frameIndex);
	void UpdateTexture2dDescriptorSets(uint32_t frameIndex);
};



#endif // __INCLUDE_GUARD_materialProperties_h__