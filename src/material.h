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
	VulkanContext* context;
	std::unique_ptr<Pipeline> pipeline;

private: // Members:
	uint32_t frameIndex;
	Type type;
	std::vector<VkDescriptorSetLayoutBinding> bindings;
	std::vector<std::string> bindingNames;
	std::unordered_map<std::string, UniformBufferBlock*> uniformBufferBlockMap;

public: // Methods:
	// Constructors/Destructor:
	Material(VulkanContext* context, Type type, const std::string& name, const std::filesystem::path& vertexSpv, const std::filesystem::path& fragmentSpv = "");
	~Material();

	// Getters:
	uint32_t GetBindingCount() const;
	uint32_t GetBindingIndex(uint32_t i) const;
	VkDescriptorType GetBindingType(uint32_t i) const;
	std::string GetBindingName(uint32_t i) const;
	UniformBufferBlock* GetUniformBufferBlock(const std::string& name) const;

	// Debugging:
	void PrintBindings() const;
	void PrintUniformBuffers() const;

private: // Methods:
	static std::vector<char> ReadShaderCode(const std::filesystem::path& spvFile);
	void GetDescriptorSetLayoutBindings(const SpirvReflect& shaderReflect, VkShaderStageFlagBits shaderStage);
};



#endif // __INCLUDE_GUARD_material_h__