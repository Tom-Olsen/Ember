#pragma once
#ifndef __INCLUDE_GUARD_material_h__
#define __INCLUDE_GUARD_material_h__
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include "spirvReflect.h"
#include "vulkanContext.h"
#include "vulkanPipeline.h"
#include "vulkanDescriptorPool.h"
#include "vulkanRenderpass.h"
#include "vulkanSampler.h"
#include "vulkanUniformBuffer.h"
#include "texture2d.h"
#include "materialProperties.h"
#include "mesh.h"



class Material
{
public: // Members:
	std::unique_ptr<VulkanPipeline> pipeline;

private: // Members:
	// Internal:
	VulkanContext* context;
	uint32_t frameIndex;
	std::vector<VkDescriptorSetLayoutBinding> bindings;
	MaterialProperties materialProperties;

	// Default resources:
	UniformObject defaultUniformObject;
	std::unique_ptr<VulkanSampler> defaultSampler;
	std::unique_ptr<Texture2d> defaultTexture2d;

public: // Methods:
	Material(VulkanContext* context, VulkanRenderpass* renderpass, const std::string& vertexSpv, const std::string& fragmentSpv);
	~Material();
	MaterialProperties GetMaterialProperties();

private: // Methods:
	std::vector<char> ReadShaderCode(const std::string& spvFile);
	void GetDescriptorSetLayoutBindings(const SpirvReflect& shaderReflect, VkShaderStageFlagBits shaderStage);
};



#endif // __INCLUDE_GUARD_material_h__