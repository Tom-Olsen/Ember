#pragma once
#include <spirv_reflect.h>
#include <string>
#include <vulkan/vulkan.h>



namespace emberSpirvReflect
{
    struct Descriptor
    {
    public: // Members:
        std::string name;
        uint32_t set;
        uint32_t binding;
        uint32_t descriptorCount; // element count for array bindings. 1 otherwise. 0 = special handling required
        SpvReflectDescriptorType descriptorType;
        VkShaderStageFlagBits shaderStage;

    public: // Methods:
        // Constructor:
		Descriptor(const SpvReflectDescriptorBinding* const pBinding, VkShaderStageFlagBits shaderStage);

        bool IsEqual(const Descriptor& other);

        // Debugging:
        std::string ToString();
    };
}