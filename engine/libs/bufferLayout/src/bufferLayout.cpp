#include "bufferLayout.h"
#include <sstream>



namespace emberBufferLayout
{
	// Public methods:
	// Constructor:
    BufferLayout::BufferLayout(const std::string& name)
        : m_name(name), m_size(0)
	{

	}



    // Getters:
    uint32_t BufferLayout::GetSize() const
    {
        return m_size;
    }
    const BufferMember* BufferLayout::GetMember(uint32_t index) const
    {
        return index < m_members.size() ? &m_members[index] : nullptr;
    }
    const BufferMember* BufferLayout::GetMember(const std::string& name) const
    {
        auto it = m_indexMap.find(name);
        return it == m_indexMap.end() ? nullptr : &m_members[it->second];
    }
    bool BufferLayout::IsLayoutCompatible(const BufferLayout& other) const
    {
        // Basics:
        //if (m_name != other.m_name) return false; // don't care about name, just layout.
        if (m_size != other.m_size) return false;
        if (m_members.size() != other.m_members.size()) return false;
        
        // Members:
        for (size_t i = 0; i < m_members.size(); i++)
        {
            const BufferMember* otherMember = other.GetMember(i);
            if (!otherMember)
                return false;
            if (!m_members[i].IsLayoutCompatible(*otherMember))
                return false;
        }

        // Fully compatible:
        return true;
    }



    // Setters:
    void BufferLayout::AddMember(const BufferMember& member)
    {
        if (m_indexMap.contains(member.GetName()))
            throw std::runtime_error("BufferLayout::AddMember: Duplicate member name in BufferLayout members.");

        uint32_t memberEnd = member.GetOffset() + member.GetSize();
        if (memberEnd > m_size)
            m_size = memberEnd; // size of the buffer is end of last member.
        m_members.push_back(member);
        m_indexMap.emplace(member.GetName(), m_members.size() - 1);
    }
    void BufferLayout::ReserveMembers(size_t count)
    {
        m_members.reserve(count);
        m_indexMap.reserve(count);
    }



    // Debugging:
    std::string BufferLayout::ToString(int indent) const
    {
        std::ostringstream ss;
        std::string indentStr(indent, ' ');
        ss << indentStr << m_name << ": size=" << m_size;
        for (size_t i = 0; i < m_members.size(); i++)
            ss << "\n" << m_members[i].ToString(indent + 2);
        return ss.str();
    }
}