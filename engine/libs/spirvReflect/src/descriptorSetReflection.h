#pragma once
#include "descriptorReflection.h"
#include <vector>
#include <string>



namespace emberSpirvReflect
{
    class DescriptorSetReflection
    {
    private: // Members:
        uint32_t m_set;
        std::vector<DescriptorReflection> m_descriptorReflections;

    public: // Methods:
        // Constructor/Destructor:
        DescriptorSetReflection();
        DescriptorSetReflection(uint32_t set);
        ~DescriptorSetReflection();

        // Main functionality:
        void AddDescriptorReflection(const DescriptorReflection& descriptorReflection);

        // Getters:
        uint32_t GetSet() const;
        const std::vector<DescriptorReflection>& GetDescriptorReflections() const;

        // Debugging:
        std::string ToString(int indent = 0) const;
    };
}