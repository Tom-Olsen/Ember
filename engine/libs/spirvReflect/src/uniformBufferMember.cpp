#include "uniformBufferMember.h"
#include "spirvReflectToString.h"
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
		bool isArrayMember = false;
		if (arrayIndex != UINT32_MAX)
		{
			name += "[" + std::to_string(arrayIndex) + "]";		// add array index to name.
			offset += memberReflection.array.stride * arrayIndex;	// offset relative to 0th element.
			size = memberReflection.array.stride;					// size of array element.
			isArrayMember = true;
		}

		// Submember struct reflection:
		if (IsStruct(memberReflection) && !IsArray(memberReflection))
		{
			for (uint32_t subMemberIndex = 0; subMemberIndex < memberReflection.member_count; subMemberIndex++)
			{
				SpvReflectBlockVariable& subMemberReflection = memberReflection.members[subMemberIndex];
				std::unique_ptr<UniformBufferMember> subMember = std::make_unique<UniformBufferMember>(subMemberReflection, offset);
				m_subMembers.emplace(subMember->name, std::move(subMember));
			}
		}

		// Submember array reflection: (array of array not possible).
		if (IsArray(memberReflection) && !isArrayMember)
		{
			for (uint32_t arrayIndex = 0; arrayIndex < memberReflection.array.dims[0]; arrayIndex++)
			{
				std::unique_ptr<UniformBufferMember> element = std::make_unique<UniformBufferMember>(memberReflection, offset, arrayIndex);
				m_subMembers.emplace(element->name, std::move(element));
			}
		}
	}



	// Copyable:
	UniformBufferMember::UniformBufferMember(const UniformBufferMember& other)
	{
	    this->name = other.name;
	    this->offset = other.offset;
	    this->size = other.size;
		this->m_subMembers.reserve(other.m_subMembers.size());
		for (const auto& [key, value] : other.m_subMembers)
		{
			if (value)
				m_subMembers.emplace(key, std::make_unique<UniformBufferMember>(*value));
			else
				m_subMembers.emplace(key, nullptr);
		}
	}
	UniformBufferMember& UniformBufferMember::operator=(const UniformBufferMember& other)
	{
	    if (this == &other)
	        return *this;

		this->name = other.name;
		this->offset = other.offset;
		this->size = other.size;
		this->m_subMembers.reserve(other.m_subMembers.size());
		for (const auto& [key, value] : other.m_subMembers)
		{
			if (value)
				m_subMembers.emplace(key, std::make_unique<UniformBufferMember>(*value));
			else
				m_subMembers.emplace(key, nullptr);
		}
	
	    return *this;
	}



	// Getters:
	const UniformBufferMember* UniformBufferMember::GetSubMember(const std::string& name) const
	{
		auto it = m_subMembers.find(name);
		return it == m_subMembers.end() ? nullptr  : it->second.get();
	}
	bool UniformBufferMember::IsLayoutCompatible(const UniformBufferMember& other) const
	{
		// Basics:
		if (offset != other.offset) return false;
		if (size != other.size) return false;
		
		// Sub-members:
		if (m_subMembers.size() != other.m_subMembers.size()) return false;
		for (const auto& [name, subMember] : m_subMembers)
		{
			const UniformBufferMember* otherSub = other.GetSubMember(name);
			if (!otherSub) return false;
			if (!subMember->IsLayoutCompatible(*otherSub))
				return false;
		}

		// Fully compatible:
		return true;
	}


	// Debugging:
	std::string UniformBufferMember::ToString(const std::string& name, int indent) const
	{
		std::ostringstream ss;
		std::string indentStr(indent, ' ');
		ss << indentStr << name << ": offset=" << offset << ", size=" << size;
		for (const auto& [subMemberName, subMember] : m_subMembers)
			ss << "\n" << subMember->ToString(subMemberName, indent + 2);
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