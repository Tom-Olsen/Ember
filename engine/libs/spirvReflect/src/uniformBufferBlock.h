#pragma once
#include "uniformBufferMember.h"
#include <cstdint>
#include <string>
#include <unordered_map>



// Forward declerations:
struct SpvReflectBlockVariable;



namespace emberSpirvReflect
{
	// Represents a bX binding.
	struct UniformBufferBlock
	{
	public: // Members:
		std::string name;
		uint32_t size;			// in bytes.
		uint32_t setIndex;		// always 0 for HLSL shaders.
		uint32_t bindingIndex;	// bX, where X is the index. 

	private: // Members:
		std::unordered_map<std::string, UniformBufferMember> m_members;

	public: // Methods:
		// Constructor:
		UniformBufferBlock(SpvReflectBlockVariable* blockReflection, uint32_t setIndex, uint32_t bindingIndex);

		// Members:
		void AddMember(UniformBufferMember member);
		const UniformBufferMember* GetMember(const std::string& name) const;

		// Debugging:
		std::string ToString(int indent = 0) const;
	};

	// Example:
	// struct Foo { int a; float b; };
	// cbuffer DataBuffer : register(b1)
	// {
	//     Foo foo;
	//     float4 c;
	//     float4 d[2];
	// };
	// UniformBufferBlock dataBuffer(...);
	// dataBuffer.m_members = {foo, c, d}
	// foo.m_submemers = {a,b}
	// d.m_submemers = {d0,d1}
}