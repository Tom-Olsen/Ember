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
        m_layoutCreated = false;
    }
    DescriptorSetReflection::DescriptorSetReflection(uint32_t set)
    {
        m_set = set;
        m_layoutCreated = false;
    }
    DescriptorSetReflection::~DescriptorSetReflection()
    {

    }



    // Main functionality:
    void DescriptorSetReflection::AddDescriptorReflection(const DescriptorReflection& descriptorReflection)
    {
        assert((m_set < UINT32_MAX) && "DescriptorSetReflection::AddDescriptor: Forgot to set m_set by calling proper constructor (not default).");
        assert(!m_layoutCreated && "DescriptorSetReflection::AddDescriptor: Called after layout bindings were finalized.");

        auto it = m_descriptorReflections.find(descriptorReflection.GetVkDescriptorSetLayoutBinding().binding);
        if (it != m_descriptorReflections.end())
        {
            if(!it->second.IsEqual(descriptorReflection))
                throw std::runtime_error("DescriptorSetReflection::AddDescriptor: Found unequal descriptors with same set+binding:\n prev: " + it->second.ToString() + "\n  new: " + descriptorReflection.ToString());
            it->second.AddShaderStage(descriptorReflection.GetVkDescriptorSetLayoutBinding().stageFlags);
        }
        else
            m_descriptorReflections.emplace(descriptorReflection.GetVkDescriptorSetLayoutBinding().binding, descriptorReflection);
    }
    void DescriptorSetReflection::CreateLayoutBindings()
    {
        assert((m_set < UINT32_MAX) && "DescriptorSetReflection::CreateLayoutBindings(): forgot to set m_set by calling proper constructor (not default).");
        assert(!m_layoutCreated && "DescriptorSetReflection::CreateLayoutBindings() called twice.");

        m_descriptorNames.reserve(m_descriptorReflections.size());
        m_vkDescriptorSetLayoutBinding.reserve(m_descriptorReflections.size());
        for (const auto& [_, descriptorReflection] : m_descriptorReflections)
        {
            m_descriptorNames.push_back(descriptorReflection.GetName());
            m_vkDescriptorSetLayoutBinding.push_back(descriptorReflection.GetVkDescriptorSetLayoutBinding());
        }

        // Sorting:
        std::sort(m_vkDescriptorSetLayoutBinding.begin(), m_vkDescriptorSetLayoutBinding.end(),
            [](const VkDescriptorSetLayoutBinding& a, const VkDescriptorSetLayoutBinding& b)
        { return a.binding < b.binding; });

        m_layoutCreated = true;
    }



    // Getters:
    uint32_t DescriptorSetReflection::GetSet() const
    {
        return m_set;
    }
    const std::vector<std::string>& DescriptorSetReflection::GetDescriptorNames() const
    {
        assert(m_layoutCreated && "Must call DescriptorSetReflection::CreateLayoutBindings() before DescriptorSetReflection::GetVkDescriptorSetLayoutBindings().");
        return m_descriptorNames;
    }
    const std::vector<VkDescriptorSetLayoutBinding>& DescriptorSetReflection::GetVkDescriptorSetLayoutBindings() const
    {
        assert(m_layoutCreated && "Must call DescriptorSetReflection::CreateLayoutBindings() before DescriptorSetReflection::GetVkDescriptorSetLayoutBindings().");
        return m_vkDescriptorSetLayoutBinding;
    }



    // Debugging:
    std::string DescriptorSetReflection::ToString(int indent) const
    {
        std::ostringstream ss;
        std::string indentStr = std::string(indent, ' ');
        ss << indentStr << "DescriptorSetReflection: " << m_set;
        for (auto [binding, descriptorReflection] : m_descriptorReflections)
            ss << "\n" << descriptorReflection.ToString(indent + 2);
        return ss.str();
    }
}