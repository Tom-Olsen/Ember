#pragma once
#include "descriptor.h"
#include <unordered_map>
#include <string>
#include <vulkan\vulkan.h>



namespace emberSpirvReflect
{
    class DescriptorSet
    {
    private: // Members:
        uint32_t m_set;
        bool m_layoutCreated;
        std::unordered_map<uint32_t, Descriptor> m_descriptors;
        std::vector<VkDescriptorSetLayoutBinding> m_vkDescriptorSetLayoutBinding;

    public: // Methods:
        // Constructor:
        DescriptorSet();
        DescriptorSet(uint32_t set);
        ~DescriptorSet();

        // Main functionality:
        void AddDescriptor(const Descriptor& descriptor);
        void CreateLayoutBindings();

        // Getters:
        uint32_t GetSet() const;
        const std::vector<VkDescriptorSetLayoutBinding>& GetVkDescriptorSetLayoutBindings() const;

        // Debugging:
        std::string ToString(int indent = 0) const;
    };
}