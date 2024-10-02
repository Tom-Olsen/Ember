#pragma once
#ifndef __INCLUDE_GUARD_material_h__
#define __INCLUDE_GUARD_material_h__
#include <memory>
#include <string>
#include <vector>
#include "spirvReflect.h"
#include "vulkanLogicalDevice.h"
#include "vulkanPhysicalDevice.h"
#include "vulkanPipeline.h"
#include "vulkanDescriptorPool.h"
#include "vulkanRenderpass.h"
#include "texture2d.h"
#include "vulkanUniformBuffer.h"



class Material
{
public: // Members:
	std::string vertexSpv;		// Path to vertex shader .spv file, without extension.
	std::string fragmentSpv;	// Path to fragment shader .spv file, without extension.
	std::unique_ptr<VulkanPipeline> pipeline;
	std::vector<VkDescriptorSet> descriptorSets;

private: // Members:
	std::vector<char> vertexCode;
	std::vector<char> fragmentCode;
	VulkanLogicalDevice* logicalDevice;
	VulkanPhysicalDevice* physicalDevice;
	VulkanDescriptorPool* descriptorPool;


public: // Methods:
	Material(uint32_t framesInFlight, VulkanLogicalDevice* logicalDevice, VulkanPhysicalDevice* physicalDevice, VulkanDescriptorPool* descriptorPool, VulkanRenderpass* renderpass, const std::string& vertexSpv, const std::string& fragmentSpv, const std::vector<VulkanUniformBuffer>& uniformBuffers, Texture2d* texture2d);
	~Material();

private: // Methods:
	std::vector<char> ReadShaderCode(const std::string& spvFile);
	void CreateDescriptorSets(uint32_t framesInFlight);
	void FillDescriptorSets(uint32_t framesInFlight, const std::vector<VulkanUniformBuffer>& uniformBuffers, Texture2d* texture2d);
};



#endif // __INCLUDE_GUARD_material_h__