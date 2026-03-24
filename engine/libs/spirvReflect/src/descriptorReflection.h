#pragma once
#include "bufferLayout.h"
#include <spirv_reflect.h>
#include <string>
#include <variant>



namespace emberSpirvReflect
{
    // DescriptorResource variant:
    struct ImageDescriptor
    {
        uint32_t imageViewType;
    };
    struct UniformBufferDescriptor
    {
        emberBufferLayout::BufferLayout bufferLayout;
    };
    using DescriptorResource = std::variant<std::monostate, ImageDescriptor, UniformBufferDescriptor>;



    class DescriptorReflection
    {
    private: // Members:
        std::string m_name;
        uint32_t m_set;
        uint32_t m_binding;
        uint32_t m_descriptorCount;
        uint32_t m_descriptorType;
        uint32_t m_shaderStage;
        DescriptorResource m_descriptorResource;

    public: // Methods:
        // Constructor/Destructor:
        DescriptorReflection(const SpvReflectDescriptorBinding* const pBinding, uint32_t shaderStage);
        ~DescriptorReflection();

        // Copyable:
        DescriptorReflection(const DescriptorReflection& other) = default;
        DescriptorReflection& operator=(const DescriptorReflection& other) = default;

        // Movable:
        DescriptorReflection(DescriptorReflection&& other) noexcept = default;
        DescriptorReflection& operator=(DescriptorReflection&& other) noexcept = default;

        // Getters:
        const std::string& GetName() const;
        uint32_t GetSet() const;
        uint32_t GetBinding() const;
        uint32_t GetDescriptorCount() const;
        uint32_t GetDescriptorType() const;
        uint32_t GetShaderStage() const;
        const ImageDescriptor* GetImageDescriptor() const;
        const UniformBufferDescriptor* GetUniformBufferDescriptor() const;

        // Setters:
        void AddShaderStage(uint32_t shaderStage);

        // Comparison:
        bool IsEqual(const DescriptorReflection& other) const;

        // Debugging:
        std::string ToString(int indent = 0) const;

    private: // Methods:
        ImageDescriptor ExtractImageDescriptor(const SpvReflectDescriptorBinding* const pBinding);
        UniformBufferDescriptor ExtractUniformBufferDescriptor(const SpvReflectDescriptorBinding* const pBinding);
        emberBufferLayout::BufferMember ExtractBufferMember(const SpvReflectBlockVariable& memberReflection, uint32_t offset = 0, uint32_t arrayIndex = UINT32_MAX);
        bool IsStruct(const SpvReflectBlockVariable& memberReflection) const;
        bool IsArray(const SpvReflectBlockVariable& memberReflection) const;
    };
}