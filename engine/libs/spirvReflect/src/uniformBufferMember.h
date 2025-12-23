#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>



// Forward declerations:
struct SpvReflectBlockVariable;



namespace emberSpirvReflect
{
	// Represents a member of a UniformBufferBlock, which may have sub-members (structs/arrays):
	struct UniformBufferMember
	{
	public: // Members:
		std::string name;
		uint32_t offset;	// in bytes.
		uint32_t size;		// in bytes.

	private: // Members:
		std::unordered_map<std::string, std::unique_ptr<UniformBufferMember>> m_subMembers;

	public: // Methods:
		// Constructor:
		// -offset for recursive struct/array members.
		// -arrayIndex for array elements.
		UniformBufferMember(const SpvReflectBlockVariable& memberReflection, uint32_t offset = 0, uint32_t arrayIndex = -1);

		// Getters:
		const UniformBufferMember* GetSubMember(const std::string& name) const;

		// Debugging:
		std::string ToString(const std::string& name, int indent = 0) const;

	private: // Methods:
		bool IsStruct(const SpvReflectBlockVariable& memberReflection) const;
		bool IsArray(const SpvReflectBlockVariable& memberReflection) const;
	};
}