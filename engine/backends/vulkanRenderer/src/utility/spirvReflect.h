#ifndef __INCLUDE_GUARD_vulkanRendererBackend_spirvReflect_h__
#define __INCLUDE_GUARD_vulkanRendererBackend_spirvReflect_h__
#include "iMath.h"
#include <spirv_reflect.h>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	/// <summary>
	/// Member variable spirv-reflection of shader uniform buffer (cbuffer) entry. <para/>
	/// If the member is a struct the subMembers are the fields of the struct. <para/>
	/// If the member is an array the subMembers are the entries of the array. <para/>
	/// Recusive nesting of structs/arrays is possible. <para/>
	/// Names of the member variables are not saved as they are implicitly stored as keys in the unordered map. <para/>
	/// For example see UniformBufferBlock.
	/// </summary>
	struct UniformBufferMember
	{
	public: // Members:
		uint32_t offset;	// in bytes.
		uint32_t size;		// in bytes.

	private: // Members:
		std::unordered_map<std::string, UniformBufferMember*> m_subMembers;

	public: // Methods:
		UniformBufferMember();
		~UniformBufferMember();
		void AddSubMember(std::string name, UniformBufferMember* pSubMember);
		UniformBufferMember* GetSubMember(const std::string& name) const;

		// Debugging:
		std::string ToString(const std::string& name, int indent = 0) const;
	};



	/// <summary>
	/// Shader uniform buffer (cbuffer) spirv-reflection. <para/>
	/// Names of the member variables are implicitly stored as the keys of the unordered map. <para/>
	/// Example: <para/>
	/// cbuffer Foo : register(b1) <para/>
	/// { float2 values[2]; } <para/>
	/// => name = Foo, size = 32, setIndex = 0, bindingIndex = 1, members: <para/>
	/// values: offset=0, size=32 <para/>
	/// values[0]: offset = 0, size = 16 <para/>
	/// values[1] : offset = 16, size = 16
	/// </summary>
	struct UniformBufferBlock
	{
	public: // Members:
		std::string name;
		uint32_t size;			// in bytes.
		uint32_t setIndex;		// always 0 for HLSL shaders.
		uint32_t bindingIndex;	// bX, where X is the index. 

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
		std::string ToString() const;
	};



	/// <summary>
	/// Container for all shader bound resources, e.g: <para/>
	/// cbuffer, SamplerState, Texture2D, etc.
	/// For each uniform buffer (cbuffer) in the shader, a UniformBufferBlock is created.
	/// </summary>
	struct DescriptorBoundResources
	{
		uint32_t bindingCount = 0; // number of descriptorSetBindingNames and descriptorSetLayoutBindings
		std::vector<std::string> descriptorSetBindingNames;
		std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;
		// One UniformBufferBlock* for each descriptorSetLayoutBindings entry with VkDescriptorType=VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER.
		std::unordered_map<std::string, UniformBufferBlock*> uniformBufferBlockMap;
		// One VkImageViewType for each descriptorSetLayoutBindings entry with VkDescriptorType=VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
		std::unordered_map<std::string, VkImageViewType> sampleViewTypeMap;
		// One VkImageViewType for each descriptorSetLayoutBindings entry with VkDescriptorType=VK_DESCRIPTOR_TYPE_STORAGE_IMAGE.
		std::unordered_map<std::string, VkImageViewType> storageViewTypeMap;
		std::string ToString() const;
	};



	class SpirvReflect
	{
	private: // Members:
		SpvReflectShaderModule m_module;

	public: // Methods:
		SpirvReflect(const std::vector<char>& code);
		~SpirvReflect();
		VertexInputDescriptions* GetVertexInputDescriptions() const;
		// Instead of returning pointer, this function adds to one, as it is used for multiple pipeline stages, e.g. vertex + fragment.
		void AddDescriptorBoundResources(DescriptorBoundResources* const descriptorBoundResources) const;
		iMath::Uint3 GetBlockSize() const;

	private: // Methods:
		std::vector<SpvReflectInterfaceVariable*> GetInputVariablesReflection() const;
		std::vector<SpvReflectDescriptorSet*> GetDescriptorSetsReflection() const;
		UniformBufferBlock* GetUniformBufferBlock(const SpvReflectBlockVariable& typeDescription, uint32_t setIndex, uint32_t bindingIndex) const;
		bool IsStruct(const SpvReflectBlockVariable& memberReflection) const;
		bool IsArray(const SpvReflectBlockVariable& memberReflection) const;
		void StructReflection(const SpvReflectBlockVariable& blockReflection, UniformBufferMember* pUniformBufferMember) const;
		void ArrayReflection(const std::string& arrayName, const SpvReflectBlockVariable& arrayReflection, UniformBufferMember* pMember) const;
	};
}



#endif // __INCLUDE_GUARD_vulkanRendererBackend_spirvReflect_h__