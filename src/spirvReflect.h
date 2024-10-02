#pragma once
#ifndef __INCLUDE_GUARD_spirvReflect_h__
#define __INCLUDE_GUARD_spirvReflect_h__
#include <spirv_reflect.h>
#include <vulkan/vulkan.h>
#include <string>
#include <vector>



class SpirvReflect
{
public: // Members:
	SpvReflectShaderModule module;

private: // Members:
	std::vector<char> code;

public: // Methods:
	SpirvReflect(std::vector<char> code);
	~SpirvReflect();
	void PrintInputVariablesInfo() const;
	void PrintDescriptorSetsInfo() const;
	std::vector<SpvReflectInterfaceVariable*> GetInputVariablesReflection() const;
	std::vector<SpvReflectDescriptorSet*> GetDescriptorSetsReflection() const;
	static std::vector<VkDescriptorSetLayoutBinding> GetDescriptorSetLayoutBindings(const SpirvReflect& vertexShaderReflect, const SpirvReflect& fragmentShaderFilename);
	static std::vector<VkWriteDescriptorSet> GetWriteDescriptorSets(const SpirvReflect& vertexShaderReflect, const SpirvReflect& fragmentShaderFilename);

private: // Methods:
	static void FixFragmentShaderDescriptorSetsReflection(std::vector<SpvReflectDescriptorSet*>& fragmentDescriptorSets);
	static std::string GetSpvReflectDescriptorTypeName(SpvReflectDescriptorType spvReflectDescriptorType);
	static std::string GetSpvStorageClassName(SpvStorageClass spvStorageClass);
	static std::string GetSpvBuiltInName(SpvBuiltIn spvBuiltIn);
};



#endif // __INCLUDE_GUARD_spirvReflect_h__