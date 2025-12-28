#pragma once
#include "uniformBufferBlock.h"
#include <memory>
#include <spirv_reflect.h>
#include <string>
#include <variant>
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
        VkShaderStageFlags shaderStage;
        UniformBufferBlock uniformBufferBlock;  // only valid for descriptorType = SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER.
        VkImageViewType vkImageViewType;        // only valid for descriptorType = SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED/STORAGE_IMAGE.

    public: // Methods:
        // Constructor:
		Descriptor(const SpvReflectDescriptorBinding* const pBinding, VkShaderStageFlags shaderStage);

        // Copyable:
        Descriptor(const Descriptor& other) = default;
        Descriptor& operator=(const Descriptor& other) = default;

        // Movable:
        Descriptor(Descriptor&& other) noexcept = default;
        Descriptor& operator=(Descriptor&& other) noexcept = default;

        // Comparison:
        bool IsEqual(const Descriptor& other);

        // Debugging:
        std::string ToString(int indent = 0) const;

    private: // Methods:
        VkImageViewType ExtractVkImageViewType(const SpvReflectDescriptorBinding* const pBinding);
    };
}