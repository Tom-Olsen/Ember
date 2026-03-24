#include "descriptorSetReflection.h"
#include <algorithm>
#include <assert.h>
#include <sstream>



namespace emberSpirvReflect
{
    // Public methods:
    // Constructor:
    DescriptorSetReflection::DescriptorSetReflection()
    {
        m_set = UINT32_MAX;
    }
    DescriptorSetReflection::DescriptorSetReflection(uint32_t set)
    {
        m_set = set;
    }
    DescriptorSetReflection::~DescriptorSetReflection()
    {

    }



    // Main functionality:
    void DescriptorSetReflection::AddDescriptorReflection(const DescriptorReflection& descriptorReflection)
    {
        assert((m_set < UINT32_MAX) && "DescriptorSetReflection::AddDescriptor: Forgot to set m_set by calling proper constructor (not default).");

        for (DescriptorReflection& existing : m_descriptorReflections)
        {
            if (existing.GetBinding() == descriptorReflection.GetBinding())
            {
                if (!existing.IsEqual(descriptorReflection))
                    throw std::runtime_error("DescriptorSetReflection::AddDescriptor: Found unequal descriptors with same set+binding:\n prev: " + existing.ToString() + "\n  new: " + descriptorReflection.ToString());

                existing.AddShaderStage(descriptorReflection.GetShaderStage());
                return;
            }
        }

        m_descriptorReflections.push_back(descriptorReflection);
    }



    // Getters:
    uint32_t DescriptorSetReflection::GetSet() const
    {
        return m_set;
    }
    const std::vector<DescriptorReflection>& DescriptorSetReflection::GetDescriptorReflections() const
    {
        return m_descriptorReflections;
    }
    //const std::vector<std::string>& DescriptorSetReflection::GetDescriptorNames() const
    //{
    //    assert(m_layoutCreated && "Must call DescriptorSetReflection::CreateLayoutBindings() before DescriptorSetReflection::GetVkDescriptorSetLayoutBindings().");
    //    return m_descriptorNames;
    //}
    //const std::vector<VkDescriptorSetLayoutBinding>& DescriptorSetReflection::GetVkDescriptorSetLayoutBindings() const
    //{
    //    assert(m_layoutCreated && "Must call DescriptorSetReflection::CreateLayoutBindings() before DescriptorSetReflection::GetVkDescriptorSetLayoutBindings().");
    //    return m_vkDescriptorSetLayoutBinding;
    //}



    // Debugging:
    std::string DescriptorSetReflection::ToString(int indent) const
    {
        std::ostringstream ss;
        std::string indentStr = std::string(indent, ' ');
        ss << indentStr << "DescriptorSetReflection: " << m_set;
        for (const DescriptorReflection& descriptorReflection : m_descriptorReflections)
            ss << "\n" << descriptorReflection.ToString(indent + 2);
        return ss.str();
    }
}