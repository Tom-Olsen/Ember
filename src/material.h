#pragma once
#ifndef __INCLUDE_GUARD_material_h__
#define __INCLUDE_GUARD_material_h__
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include "spirvReflect.h"
#include "vulkanLogicalDevice.h"
#include "vulkanPhysicalDevice.h"
#include "vulkanPipeline.h"
#include "vulkanDescriptorPool.h"
#include "vulkanRenderpass.h"
#include "vulkanSampler.h"
#include "vulkanUniformBuffer.h"
#include "texture2d.h"



class Material
{
public: // Members:
	std::string vertexSpv;		// Path to vertex shader .spv file, without extension.
	std::string fragmentSpv;	// Path to fragment shader .spv file, without extension.
	std::unique_ptr<VulkanPipeline> pipeline;
	std::vector<VkDescriptorSet> descriptorSets;

private: // Members:
	VulkanLogicalDevice* logicalDevice;
	VulkanPhysicalDevice* physicalDevice;
	VulkanDescriptorPool* descriptorPool;
	std::vector<char> vertexCode;
	std::vector<char> fragmentCode;
	std::unordered_map<std::string, VulkanSampler*> samplerMap;
	std::unordered_map<std::string, Texture2d*> texture2dMap;
	std::unordered_map<std::string, GlobalUniformObject*> uniformBufferMap;
	std::unique_ptr<VulkanSampler> defaultSampler;
	std::unique_ptr<Texture2d> defaultTexture2d;
	std::unique_ptr<GlobalUniformObject> defaultUniformBuffer;

public: // Methods:
	Material(uint32_t framesInFlight, VulkanLogicalDevice* logicalDevice, VulkanPhysicalDevice* physicalDevice, VulkanDescriptorPool* descriptorPool, VulkanRenderpass* renderpass, const std::string& vertexSpv, const std::string& fragmentSpv, const std::vector<VulkanUniformBuffer>& uniformBuffers, Texture2d* texture2d, VulkanSampler* sampler);
	~Material();
	void SetSampler(const std::string& name, VulkanSampler* sampler);
	void SetTexture2d(const std::string& name, Texture2d* texture);
	void SetUniformBuffer(const std::string& name, GlobalUniformObject* constantBuffer);
	VulkanSampler* GetSampler(const std::string& name);
	Texture2d* GetTexture2d(const std::string& name);
	GlobalUniformObject* GetUniformBuffer(const std::string& name);

private: // Methods:
	std::vector<char> ReadShaderCode(const std::string& spvFile);
	std::vector<VkDescriptorSetLayoutBinding> GetDescriptorSetLayoutBindings(const SpirvReflect& vertexShaderReflect, const SpirvReflect& fragmentShaderReflect);
	void CreateDescriptorSets(uint32_t framesInFlight);
	void FillDescriptorSets(uint32_t framesInFlight, const std::vector<VulkanUniformBuffer>& uniformBuffers, Texture2d* texture2d, VulkanSampler* sampler);
};



#endif // __INCLUDE_GUARD_material_h__