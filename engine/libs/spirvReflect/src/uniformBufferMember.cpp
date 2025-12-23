#include "uniformBufferMember.h"
#include <spirv_reflect.h>
#include <sstream>



namespace emberSpirvReflect
{
	// Public methods:
	// Constructor:
	UniformBufferMember::UniformBufferMember(const SpvReflectBlockVariable& memberReflection, uint32_t offset, uint32_t arrayIndex)
	{
		// Base member:
		this->name = memberReflection.name;
		this->offset = memberReflection.offset + offset;	// offset != 0: convert offset relative to parent member to absolute offset.
		this->size = memberReflection.size;

		// Array member:
		if (arrayIndex != uint32_t(-1))
		{
			name += "[" + std::to_string(arrayIndex) + "]";		// add array index to name.
			offset += memberReflection.array.stride * arrayIndex;	// offset relative to 0th element.
			size = memberReflection.array.stride;					// size of array element.
		}

		// Submember struct reflection:
		if (IsStruct(memberReflection) > 0 && !IsArray(memberReflection))
		{
			for (uint32_t subMemberIndex = 0; subMemberIndex < memberReflection.member_count; subMemberIndex++)
			{
				SpvReflectBlockVariable& subMemberReflection = memberReflection.members[subMemberIndex];
				std::unique_ptr<UniformBufferMember> subMember = std::make_unique<UniformBufferMember>(subMemberReflection, offset);
				m_subMembers.emplace(subMember->name, std::move(subMember));
			}
		}

		// Submember array reflection:
		if (IsArray(memberReflection))
		{
			for (uint32_t arrayIndex = 0; arrayIndex < memberReflection.array.dims[0]; arrayIndex++)
			{
				std::unique_ptr<UniformBufferMember> element = std::make_unique<UniformBufferMember>(memberReflection, offset, arrayIndex);
				m_subMembers.emplace(element->name, std::move(element));
			}
		}
	}



	// Getters:
	const UniformBufferMember* UniformBufferMember::GetSubMember(const std::string& name) const
	{
		auto it = m_subMembers.find(name);
		return it == m_subMembers.end() ? nullptr  : it->second.get();
	}



	// Debugging:
	std::string UniformBufferMember::ToString(const std::string& name, int indent) const
	{
		std::ostringstream ss;
		std::string indentStr(indent, ' ');
		ss << indentStr << name << ": offset=" << offset << ", size=" << size << "\n";
		for (const auto& [subMemberName, subMember] : m_subMembers)
			ss << subMember->ToString(subMemberName, indent + 2);
		return ss.str();
	}



	// Private methods:
	bool UniformBufferMember::IsStruct(const SpvReflectBlockVariable& memberReflection) const
	{
		return memberReflection.member_count > 0;
	}
	bool UniformBufferMember::IsArray(const SpvReflectBlockVariable& memberReflection) const
	{
		return memberReflection.array.dims_count > 0;
	}
}