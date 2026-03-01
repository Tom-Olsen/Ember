#pragma once
#include "bufferLayout.h"
#include <spirv_reflect.h>
#include <string>
#include <variant>
#include <vulkan/vulkan.h>



namespace emberSpirvReflect
{
    // DescriptorResource variant:
    struct ImageDescriptor
    {
        VkImageViewType vkImageViewType;
    };
    struct UniformBufferDescriptor
    {
        uint32_t set;
        uint32_t binding;
        emberBufferLayout::BufferLayout bufferLayout;
    };
    using DescriptorResource = std::variant<std::monostate, ImageDescriptor, UniformBufferDescriptor>;



    class DescriptorReflection
    {
    private: // Members:
        std::string m_name;
        uint32_t m_set;
        VkDescriptorType m_vkDescriptorType;
        DescriptorResource m_descriptorResource;
        VkDescriptorSetLayoutBinding m_vkDescriptorSetLayoutBinding;

    public: // Methods:
        // Constructor:
        DescriptorReflection(const SpvReflectDescriptorBinding* const pBinding, VkShaderStageFlags shaderStage);

        // Copyable:
        DescriptorReflection(const DescriptorReflection& other) = default;
        DescriptorReflection& operator=(const DescriptorReflection& other) = default;

        // Movable:
        DescriptorReflection(DescriptorReflection&& other) noexcept = default;
        DescriptorReflection& operator=(DescriptorReflection&& other) noexcept = default;

        // Getters:
        const std::string& GetName() const;
        uint32_t GetSet() const;
        VkDescriptorType GetVkDescriptorType() const;
        const ImageDescriptor* GetImageDescriptor() const;
        const UniformBufferDescriptor* GetUniformBufferDescriptor() const;
        VkDescriptorSetLayoutBinding GetVkDescriptorSetLayoutBinding() const;

        // Setters:
        void AddShaderStage(VkShaderStageFlags vkShaderStageFlags);

        // Comparison:
        bool IsEqual(const DescriptorReflection& other);

        // Debugging:
        std::string ToString(int indent = 0) const;

    private: // Methods:
        ImageDescriptor ExtractImageDescriptor(const SpvReflectDescriptorBinding* const pBinding);
        UniformBufferDescriptor ExtractBufferLayout(const SpvReflectDescriptorBinding* const pBinding);
        emberBufferLayout::BufferMember ExtractBufferMember(const SpvReflectBlockVariable& memberReflection, uint32_t offset = 0, uint32_t arrayIndex = UINT32_MAX);
        bool IsStruct(const SpvReflectBlockVariable& memberReflection) const;
        bool IsArray(const SpvReflectBlockVariable& memberReflection) const;
    };
}