#pragma once
#include "descriptorReflection.h"
#include <unordered_map>
#include <string>
#include <vulkan\vulkan.h>



namespace emberSpirvReflect
{
    class DescriptorSetReflection
    {
    private: // Members:
        uint32_t m_set;
        bool m_layoutCreated;
        std::unordered_map<uint32_t, DescriptorReflection> m_descriptorReflections;
        std::vector<std::string> m_descriptorNames;
        std::vector<VkDescriptorSetLayoutBinding> m_vkDescriptorSetLayoutBinding;

    public: // Methods:
        // Constructor:
        DescriptorSetReflection();
        DescriptorSetReflection(uint32_t set);
        ~DescriptorSetReflection();

        // Main functionality:
        void AddDescriptorReflection(const DescriptorReflection& descriptorReflection);
        void CreateLayoutBindings();

        // Getters:
        uint32_t GetSet() const;
        const std::vector<std::string>& GetDescriptorNames() const;
        const std::vector<VkDescriptorSetLayoutBinding>& GetVkDescriptorSetLayoutBindings() const;

        // Debugging:
        std::string ToString(int indent = 0) const;
    };
}