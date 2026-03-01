#include "bufferMember.h"
#include <cassert>
#include <sstream>



namespace emberBufferLayout
{
	// Public methods:
	// Constructor:
	BufferMember::BufferMember(const std::string& name, uint32_t offset, uint32_t size)
		: m_name(name), m_offset(offset), m_size(size)
	{

	}



	// Getters:
	const std::string& BufferMember::GetName() const
	{
		return m_name;
	}
	uint32_t BufferMember::GetOffset() const
	{
		return m_offset;
	}
	uint32_t BufferMember::GetSize() const
	{
		return m_size;
	}
	const BufferMember* BufferMember::GetSubMember(uint32_t index) const
	{
		return index < m_subMembers.size() ? &m_subMembers[index] : nullptr;
	}
	const BufferMember* BufferMember::GetSubMember(const std::string& name) const
	{
		auto it = m_indexMap.find(name);
		return it == m_indexMap.end() ? nullptr : &m_subMembers[it->second];
	}
	bool BufferMember::IsLayoutCompatible(const BufferMember& other) const
	{
		// Basics:
		//if (m_name != other.m_name) return false; // don't care about name, just layout.
		if (m_offset != other.m_offset) return false;
		if (m_size != other.m_size) return false;
		
		// Sub-members:
		if (m_subMembers.size() != other.m_subMembers.size()) return false;
		for (size_t i = 0; i < m_subMembers.size(); i++)
		{
			const BufferMember* otherSubMember = other.GetSubMember(i);
			if (!otherSubMember)
				return false;
			if (!m_subMembers[i].IsLayoutCompatible(*otherSubMember))
				return false;
		}

		// Fully compatible:
		return true;
	}



	// Setters:
	void BufferMember::AddSubMember(const BufferMember& subMember)
	{
		if (m_indexMap.contains(subMember.GetName()))
			throw std::runtime_error("BufferMember::AddSubMember: Duplicate subMember name.");

		uint32_t subMemberEnd = subMember.GetOffset() + subMember.GetSize();
		m_subMembers.push_back(subMember);
		m_indexMap.emplace(subMember.GetName(), m_subMembers.size() - 1);
	}



	// Debugging:
	std::string BufferMember::ToString(int indent) const
	{
		std::ostringstream ss;
		std::string indentStr(indent, ' ');
		ss << indentStr << m_name << ": offset=" << m_offset << ", size=" << m_size;
		for (size_t i = 0; i < m_subMembers.size(); i++)
			ss << "\n" << m_subMembers[i].ToString(indent + 2);
		return ss.str();
	}
}