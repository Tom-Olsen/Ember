#pragma once
#ifndef __INCLUDE_GUARD_material_h__
#define __INCLUDE_GUARD_material_h__
#include <memory>
#include <filesystem>
#include <vector>
#include <unordered_map>
#include "vulkanContext.h"
#include "forwardPipeline.h"
#include "shadowPipeline.h"
#include "spirvReflect.h"
#include "materialProperties.h"



/// <summary>
/// Material creation is expensive.
/// It is recommended to create all materials at the start of the application.
/// Create a Material pointer and store it in the static MaterialManager class, making it globally accessible.
/// </summary>
class Material
{
public: // Enums:
	enum class Type
	{
		shadow,
		forward
	};

public: // Members:
	std::string name;
	std::unique_ptr<Pipeline> pipeline;

private: // Members:
	VulkanContext* context;
	uint32_t frameIndex;
	Type type;
	std::vector<VkDescriptorSetLayoutBinding> bindings;
	std::vector<std::string> bindingNames;

public: // Methods:
	// Constructors/Destructor:
	Material(VulkanContext* context, Type type, const std::string& name, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv = "");
	Material(const Material& other) = default;
	Material& operator=(const Material& other) = default;
	Material(Material&& other) noexcept = default;
	Material& operator=(Material&& other) noexcept = default;
	~Material();

	// Getters:
	MaterialProperties* GetNewMaterialProperties();

	// Debugging:
	void PrintBindings() const;

private: // Methods:
	static std::vector<char> ReadShaderCode(const std::filesystem::path& spvFile);
	void GetDescriptorSetLayoutBindings(const SpirvReflect& shaderReflect, VkShaderStageFlagBits shaderStage);
};



#endif // __INCLUDE_GUARD_material_h__