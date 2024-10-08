#pragma once
#ifndef __INCLUDE_GUARD_materialProperties_h__
#define __INCLUDE_GUARD_materialProperties_h__
#include <string>
#include "vulkanUniformBuffer.h"
#include "vulkanSampler.h"
#include "texture2d.h"



template<typename T>
struct ResourceBinding
{
	uint32_t binding;
	T resource;
};



class MaterialProperties
{
public: // Members:
	std::unordered_map<std::string, ResourceBinding<VulkanUniformBuffer>> uniformBufferMap;
	std::unordered_map<std::string, ResourceBinding<VulkanSampler*>> samplerMap;
	std::unordered_map<std::string, ResourceBinding<Texture2d*>> texture2dMap;

private: // Members:

public: // Methods:
	MaterialProperties();
	~MaterialProperties();

	// Initializers:
	void InitUniformObjectResourceBinding(std::string name, ResourceBinding<VulkanUniformBuffer> uniformBuffer);
	void InitSamplerResourceBinding(std::string name, ResourceBinding<VulkanSampler*> sampler);
	void InitTexture2dResourceBinding(std::string name, ResourceBinding<Texture2d*> texture2d);

	// Setters:
	void SetUniformBuffer(const std::string& name, const UniformObject& data);
	void SetSampler(const std::string& name, VulkanSampler* sampler);
	void SetTexture2d(const std::string& name, Texture2d* texture2d);
private: // Methods:
};



#endif // __INCLUDE_GUARD_materialProperties_h__