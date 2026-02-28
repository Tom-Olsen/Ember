#pragma once
#include "uniformBufferBlock.h"
#include <spirv_reflect.h>
#include <string>
#include <variant>
#include <vulkan/vulkan.h>



namespace emberSpirvReflect
{
    //// DescriptorResource variant:
    //struct UniformBufferDescriptor
    //{
    //    BufferLayout layout;
    //};
    //struct ImageDescriptor
    //{
    //    VkImageViewType viewType;
    //};
    //using DescriptorResource = std::variant<std::monostate, UniformBufferDescriptor, ImageDescriptor>;



    struct Descriptor
    {
    public: // Members:
        std::string name;
        uint32_t set;
        VkDescriptorSetLayoutBinding vkDescriptorSetLayoutBinding;
        // Ember::ToDo: use variant here see above example code.
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