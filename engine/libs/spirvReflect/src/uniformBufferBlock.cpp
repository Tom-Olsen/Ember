#include "uniformBufferBlock.h"
#include <sstream>
#include <spirv_reflect.h>



namespace emberSpirvReflect
{
	// Public methods:
	// Constructor:
    UniformBufferBlock::UniformBufferBlock()
    {
        // Base block:
        this->name = "invalid";
        this->size = -1;
        this->setIndex = -1;
        this->bindingIndex = -1;
    }
	UniformBufferBlock::UniformBufferBlock(const SpvReflectBlockVariable& blockReflection, uint32_t setIndex, uint32_t bindingIndex)
	{
        // Base block:
        this->name = blockReflection.name;
        this->size = blockReflection.size;
        this->setIndex = setIndex;
        this->bindingIndex = bindingIndex;

        // Members:
        for (uint32_t memberIndex = 0; memberIndex < blockReflection.member_count; memberIndex++)
        {
            SpvReflectBlockVariable& memberReflection = blockReflection.members[memberIndex];
            UniformBufferMember member = UniformBufferMember(memberReflection);
            uint32_t memberEnd = member.offset + member.size;
            if (memberEnd > size)
                size = memberEnd; // size of the block is end of last member.
            m_members.emplace(member.name, std::move(member));
        }
	}



    // Getters:
    const UniformBufferMember* UniformBufferBlock::GetMember(const std::string& name) const
    {
        auto it = m_members.find(name);
        return it == m_members.end() ? nullptr : &it->second;
    }
    bool UniformBufferBlock::IsLayoutCompatible(const UniformBufferBlock& other) const
    {
        // Basics;
        if (size != other.size) return false;
        if (m_members.size() != other.m_members.size()) return false;
        
        // Members:
        for (const auto& [name, member] : m_members)
        {
            const UniformBufferMember* otherMember = other.GetMember(name);
            if (!otherMember) return false;
            if (!member.IsLayoutCompatible(*otherMember))
                return false;
        }

        // Fully compatible:
        return true;
    }



    // Debugging:
    std::string UniformBufferBlock::ToString(int indent) const
    {
        std::ostringstream ss;
        std::string indentStr(indent, ' ');
        ss << indentStr << name << "(set=" << setIndex<< ", binding=" << bindingIndex << ", size = " << size << "):";
        for (const auto& [memberName, member] : m_members)
            ss << "\n" << member.ToString(memberName, indent + 2);
        return ss.str();
    }
}