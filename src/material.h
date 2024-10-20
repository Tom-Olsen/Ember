#pragma once
#ifndef __INCLUDE_GUARD_material_h__
#define __INCLUDE_GUARD_material_h__
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include "vulkanContext.h"
#include "vulkanPipeline.h"
#include "spirvReflect.h"
#include "materialProperties.h"



/// <summary>
/// Material creation is expensive.
/// It is recommended to create all materials at the start of the application.
/// Create a Material pointer and store it in the static MaterialManager class, making it globally accessible.
/// </summary>
class Material
{
public: // Members:
	std::string name;
	std::unique_ptr<VulkanPipeline> pipeline;

private: // Members:
	// Internal:
	VulkanContext* context;
	uint32_t frameIndex;
	std::vector<VkDescriptorSetLayoutBinding> bindings;
	std::unique_ptr<MaterialProperties> materialProperties;

	// Default resources:
	UniformObject defaultUniformObject;
	std::unique_ptr<VulkanSampler> defaultSampler;
	std::unique_ptr<Texture2d> defaultTexture2d;

public: // Methods:
	Material(VulkanContext* context, const std::string& vertexSpv, const std::string& fragmentSpv, std::string name);
	Material(const Material& other) = default;
	Material& operator=(const Material& other) = default;
	Material(Material&& other) noexcept = default;
	Material& operator=(Material&& other) noexcept = default;
	~Material();

	MaterialProperties* GetMaterialPropertiesCopy();

private: // Methods:
	std::vector<char> ReadShaderCode(const std::string& spvFile);
	void GetDescriptorSetLayoutBindings(const SpirvReflect& shaderReflect, VkShaderStageFlagBits shaderStage);
};



#endif // __INCLUDE_GUARD_material_h__