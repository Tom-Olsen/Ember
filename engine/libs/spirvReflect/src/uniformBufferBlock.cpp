#include "uniformBufferBlock.h"
#include <sstream>
#include <spirv_reflect.h>



namespace emberSpirvReflect
{
	// Public methods:
	// Constructor:
	UniformBufferBlock::UniformBufferBlock(SpvReflectBlockVariable* blockReflection, uint32_t setIndex, uint32_t bindingIndex)
	{
        // Base block:
        this->name = blockReflection->name;
        this->size = blockReflection->size;
        this->setIndex = setIndex;
        this->bindingIndex = bindingIndex;

        // Members:
        for (uint32_t memberIndex = 0; memberIndex < blockReflection->member_count; memberIndex++)
        {
            SpvReflectBlockVariable& memberReflection = blockReflection->members[memberIndex];
            UniformBufferMember member(memberReflection);
            AddMember(member);
        }
	}
    void UniformBufferBlock::AddMember(UniformBufferMember member)
    {
        m_members.emplace(member.name, std::move(member));
        // Size of the block is offset+size of last member:
        uint32_t newSize = member.offset + member.size;
        if (newSize > size)
            size = newSize;
    }
    const UniformBufferMember* UniformBufferBlock::GetMember(const std::string& name) const
    {
        auto it = m_members.find(name);
        return it == m_members.end() ? nullptr : &it->second;
    }
    std::string UniformBufferBlock::ToString(int indent) const
    {
        std::ostringstream ss;
        std::string indentStr(indent, ' ');
        ss << indentStr << name << "(binding=" << bindingIndex << ", size=" << size << "):\n";
        for (const auto& [memberName, member] : m_members)
            ss << member.ToString(memberName, indent + 2);
        return ss.str();
    }
}