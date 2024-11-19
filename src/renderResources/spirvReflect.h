#pragma once
#ifndef __INCLUDE_GUARD_spirvReflect_h__
#define __INCLUDE_GUARD_spirvReflect_h__
#include <spirv_reflect.h>
#include <vulkan/vulkan.h>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "mathf.h"



/// <summary>
/// Single member variable reconstruction of shader uniform buffer block via spirv-reflect.
/// </summary>
struct UniformBufferMember
{
	uint32_t offset;
	uint32_t size;
	std::unordered_map<std::string, UniformBufferMember*> subMembers;

	// Debugging:
	std::string ToString() const;
};



/// <summary>
/// Reconstruction of shader uniform buffer block via spirv-reflect.
/// </summary>
struct UniformBufferBlock
{
public: // Members:
	std::string name;
	uint32_t setIndex;		// always 0 for HLSL shaders
	uint32_t bindingIndex;
	uint32_t size;

private: // Members:
	std::unordered_map<std::string, UniformBufferMember*> members;

public: // Methods:
	UniformBufferBlock(const std::string& name, uint32_t setIndex, uint32_t bindingIndex);
	~UniformBufferBlock();

	void AddMember(std::string name, UniformBufferMember* member);
	UniformBufferMember* GetMember(const std::string& name) const;
	std::string ToString() const;
};



class SpirvReflect
{
public: // Members:
	SpvReflectShaderModule module;

private: // Members:
	std::vector<char> code;

public: // Methods:
	SpirvReflect(std::vector<char> code);
	~SpirvReflect();
	std::vector<SpvReflectDescriptorSet*> GetDescriptorSetsReflection() const;
	UniformBufferBlock* GetUniformBufferBlock(const SpvReflectBlockVariable& typeDescription, uint32_t setIndex, uint32_t bindingIndex) const;

private: // Methods:
	bool IsStruct(const SpvReflectBlockVariable& memberReflection) const;
	bool IsArray(const SpvReflectBlockVariable& memberReflection) const;
	static std::string GetSpvReflectDescriptorTypeName(SpvReflectDescriptorType spvReflectDescriptorType);
	static std::string GetSpvStorageClassName(SpvStorageClass spvStorageClass);
	static std::string GetSpvBuiltInName(SpvBuiltIn spvBuiltIn);
};



#endif // __INCLUDE_GUARD_spirvReflect_h__