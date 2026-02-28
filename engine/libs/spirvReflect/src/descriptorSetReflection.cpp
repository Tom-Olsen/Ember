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
    void DescriptorSetReflection::AddDescriptor(const Descriptor& descriptor)
    {
        assert((m_set < UINT32_MAX) && "DescriptorSetReflection::AddDescriptor(const Descriptor&): forgot to set m_set by calling proper constructor (not default).");
        assert(!m_layoutCreated && "DescriptorSetReflection::AddDescriptor(const Descriptor&) called after layout bindings were finalized.");

        auto it = m_descriptors.find(descriptor.vkDescriptorSetLayoutBinding.binding);
        if (it != m_descriptors.end())
        {
            if(!it->second.IsEqual(descriptor))
                throw std::runtime_error("Found unequal descriptors with same set+binding:\n prev: " + it->second.ToString() + "\n  new: " + descriptor.ToString());
            it->second.vkDescriptorSetLayoutBinding.stageFlags |= descriptor.vkDescriptorSetLayoutBinding.stageFlags;
        }
        else
            m_descriptors.emplace(descriptor.vkDescriptorSetLayoutBinding.binding, descriptor);
    }
    void DescriptorSetReflection::CreateLayoutBindings()
    {
        assert((m_set < UINT32_MAX) && "DescriptorSetReflection::CreateLayoutBindings(): forgot to set m_set by calling proper constructor (not default).");
        assert(!m_layoutCreated && "DescriptorSetReflection::CreateLayoutBindings() called twice.");

        m_descriptorNames.reserve(m_descriptors.size());
        m_vkDescriptorSetLayoutBinding.reserve(m_descriptors.size());   
        for (const auto& [_, descriptor] : m_descriptors)
        {
            m_descriptorNames.push_back(descriptor.name);
            m_vkDescriptorSetLayoutBinding.push_back(descriptor.vkDescriptorSetLayoutBinding);
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
        for (auto [binding, descriptor] : m_descriptors)
            ss << "\n" << descriptor.ToString(indent + 2);
        return ss.str();
    }
}