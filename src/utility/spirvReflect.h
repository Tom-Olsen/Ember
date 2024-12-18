#ifndef __INCLUDE_GUARD_spirvReflect_h__
#define __INCLUDE_GUARD_spirvReflect_h__
#include "mathf.h"
#include <spirv_reflect.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan.h>



/// <summary>
/// Single member variable reconstruction of shader uniform buffer block via spirv-reflect.
/// </summary>
struct UniformBufferMember
{
public: // Members:
	uint32_t offset;
	uint32_t size;

private: // Members:
	std::unordered_map<std::string, UniformBufferMember*> m_subMembers;

public: // Methods:
	void AddSubMember(std::string name, UniformBufferMember* pSubMember);
	UniformBufferMember* GetSubMember(const std::string& name) const;

	// Debugging:
	std::string ToString(const std::string& name, int indent = 0) const;
};



/// <summary>
/// Reconstruction of shader uniform buffer block via spirv-reflect.
/// </summary>
struct UniformBufferBlock
{
public: // Members:
	std::string name;
	uint32_t size;			// in bytes
	uint32_t setIndex;		// always 0 for HLSL shaders
	uint32_t bindingIndex;

private: // Members:
	std::unordered_map<std::string, UniformBufferMember*> members;

public: // Methods:
	UniformBufferBlock(const std::string& name, uint32_t size, uint32_t setIndex, uint32_t bindingIndex);
	~UniformBufferBlock();

	void AddMember(std::string name, UniformBufferMember* pMember);
	UniformBufferMember* GetMember(const std::string& name) const;
	std::string ToString(int indent = 0) const;
};



/// <summary>
/// Container for all vertex input descriptions, e.g: <para/>
/// struct VertexInput <para/>
/// { <para/>
///     float3 position : POSITION; <para/>
///     float4 uv : TEXCOORD0; <para/>
/// }; <para/>
/// => size = 2 <para/>
/// => semantics = {"POSITION", "TEXCOORD0"} <para/>
/// => bindings = {{binding=0, stride=3,...}, {binding=0, stride=4,...}} <para/>
/// => attributes = {{format=VK_FORMAT_R32G32B32_SFLOAT,...}, {format=VK_FORMAT_R32G32B32A32_SFLOAT,...}}
/// </summary>
struct VertexInputDescriptions
{
	uint32_t size = 0;
	std::vector<std::string> semantics;
	std::vector<VkVertexInputBindingDescription> bindings;
	std::vector<VkVertexInputAttributeDescription> attributes;
};



/// <summary>
/// Container for all shader bound resources, e.g: <para/>
/// cbuffer, SamplerState, Texture2D, etc.
/// For each uniform buffer (cbuffer) in the shader, a UniformBufferBlock is created.
/// </summary>
struct DescriptorBoundResources
{
	uint32_t size = 0;
	std::vector<std::string> descriptorSetBindingNames;
	std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;
	std::unordered_map<std::string, UniformBufferBlock*> uniformBufferBlockMap;
};



class SpirvReflect
{
private: // Members:
	SpvReflectShaderModule m_module;

public: // Methods:
	SpirvReflect(const std::vector<char>& code);
	~SpirvReflect();
	VertexInputDescriptions* GetVertexInputDescriptions() const;
	void AddDescriptorBoundResources(DescriptorBoundResources* const descriptorBoundResources) const;

private: // Methods:
	std::vector<SpvReflectInterfaceVariable*> GetInputVariablesReflection() const;
	std::vector<SpvReflectDescriptorSet*> GetDescriptorSetsReflection() const;
	UniformBufferBlock* GetUniformBufferBlock(const SpvReflectBlockVariable& typeDescription, uint32_t setIndex, uint32_t bindingIndex) const;
	bool IsStruct(const SpvReflectBlockVariable& memberReflection) const;
	bool IsArray(const SpvReflectBlockVariable& memberReflection) const;
	void StructReflection(const SpvReflectBlockVariable& blockReflection, UniformBufferMember* pUniformBufferMember) const;
	void ArrayReflection(const std::string& arrayName, const SpvReflectBlockVariable& arrayReflection, UniformBufferMember* pMember) const;
};



#endif // __INCLUDE_GUARD_spirvReflect_h__