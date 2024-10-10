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
#include "materialProperties.h"
#include "mesh.h"



// TODO:
// - logic for descriptor set recreation (only if needed) (use nextMaterialProperties object?)
// - move render call into material class

class Material
{
public: // Members:
	std::unique_ptr<VulkanPipeline> pipeline;
	std::vector<VkDescriptorSet> descriptorSets;

private: // Members:
	// Internal:
	VulkanLogicalDevice* logicalDevice;
	VulkanPhysicalDevice* physicalDevice;
	VulkanDescriptorPool* descriptorPool;
	uint32_t framesInFlight;
	uint32_t frameIndex;
	std::vector<VkWriteDescriptorSet> descriptorWrites;
	std::vector<VkDescriptorSetLayoutBinding> bindings;

	// Render resources:
	std::vector<MaterialProperties> materialProperties;
	MaterialProperties emptyMaterialProperties;
	MaterialProperties nextMaterialProperties;

	// Default resources:
	UniformObject defaultUniformObject;
	std::unique_ptr<VulkanSampler> defaultSampler;
	std::unique_ptr<Texture2d> defaultTexture2d;

public: // Methods:
	Material(uint32_t framesInFlight, VulkanLogicalDevice* logicalDevice, VulkanPhysicalDevice* physicalDevice, VulkanDescriptorPool* descriptorPool, VulkanRenderpass* renderpass, const std::string& vertexSpv, const std::string& fragmentSpv);
	~Material();
	MaterialProperties GetEmptyMaterialProperties();
	// TODO: renderloop
	//void Render(uint32_t frameIndex, Mesh* mesh, , MaterialProperties* materialProperties);
	void SetMaterialProperties(const MaterialProperties& materialProperties, uint32_t frameIndex);

private: // Methods:
	std::vector<char> ReadShaderCode(const std::string& spvFile);
	void GetDescriptorSetLayoutBindings(const SpirvReflect& shaderReflect, VkShaderStageFlagBits shaderStage);
	void CreateDescriptorSets();
	void FillUniformBufferDescriptorSets(uint32_t frameIndex);
	void FillSamplerDescriptorSets(uint32_t frameIndex);
	void FillTexture2dDescriptorSets(uint32_t frameIndex);
};



#endif // __INCLUDE_GUARD_material_h__