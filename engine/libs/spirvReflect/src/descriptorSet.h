#pragma once
#include "descriptor.h"
#include <unordered_map>
#include <string>



namespace emberSpirvReflect
{
    class DescriptorSet
    {
    private: // Members:
        uint32_t m_set;
        std::unordered_map<uint32_t, Descriptor> m_descriptors;
        std::vector<VkDescriptorSetLayoutBinding> m_vkDescriptorSetLayoutBinding;

    public: // Methods:
        // Constructor:
        DescriptorSet();
        DescriptorSet(uint32_t set);

        void AddDescriptor(const Descriptor& descriptor);

        // Getters:
        uint32_t GetSet() const;
        const std::vector<VkDescriptorSetLayoutBinding>& GetVkDescriptorSetLayoutBindings();

        // Debugging:
        std::string ToString(int indent = 0) const;
    };
}