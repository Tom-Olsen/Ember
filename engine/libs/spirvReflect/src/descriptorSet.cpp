#include "descriptorSet.h"
#include <algorithm>
#include <assert.h>
#include <sstream>



namespace emberSpirvReflect
{
    // Public methods:
    // Constructor:
    DescriptorSet::DescriptorSet()
    {
        m_set = UINT32_MAX;
        m_layoutCreated = false;
    }
    DescriptorSet::DescriptorSet(uint32_t set)
    {
        m_set = set;
        m_layoutCreated = false;
    }
    DescriptorSet::~DescriptorSet()
    {

    }



    // Main functionality:
    void DescriptorSet::AddDescriptor(const Descriptor& descriptor)
    {
        assert(!m_layoutCreated && "DescriptorSet::AddDescriptor(const Descriptor&) called after layout bindings were finalized.");

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
    void DescriptorSet::CreateLayoutBindings()
    {
        assert(!m_layoutCreated && "DescriptorSet::CreateLayout() called twice.");

        m_vkDescriptorSetLayoutBinding.reserve(m_descriptors.size());   
        for (const auto& [_, descriptor] : m_descriptors)
            m_vkDescriptorSetLayoutBinding.push_back(descriptor.vkDescriptorSetLayoutBinding);

        // Sorting for better debug output.
        std::sort(m_vkDescriptorSetLayoutBinding.begin(), m_vkDescriptorSetLayoutBinding.end(),
            [](const VkDescriptorSetLayoutBinding& a, const VkDescriptorSetLayoutBinding& b)
        { return a.binding < b.binding; });

        m_layoutCreated = true; m_vkDescriptorSetLayoutBinding
    }



    // Getters:
    uint32_t DescriptorSet::GetSet() const
    {
        return m_set;
    }
    const std::vector<VkDescriptorSetLayoutBinding>& DescriptorSet::GetVkDescriptorSetLayoutBindings() const
    {
        assert(m_layoutCreated && "Must call DescriptorSet::CreateLayout() before DescriptorSet::GetVkDescriptorSetLayoutBindings().");
        return m_vkDescriptorSetLayoutBinding;
    }



    // Debugging:
    std::string DescriptorSet::ToString(int indent) const
    {
        std::ostringstream ss;
        std::string indentStr = std::string(indent, ' ');
        ss << indentStr << "DescriptorSet: " << m_set;
        for (auto [binding, descriptor] : m_descriptors)
            ss << "\n" << descriptor.ToString(indent + 2);
        return ss.str();
    }
}