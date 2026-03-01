#include "descriptorReflection.h"
#include "spirvReflectToString.h"
#include "vulkanDescriptorSetLayoutBindingToString.h"
#include "vulkanImageViewTypeToString.h"
#include "vulkanShaderStageFlagsToString.h"
#include <sstream>



namespace emberSpirvReflect
{
    // Public methods:
    // Constructor:
    DescriptorReflection::DescriptorReflection(const SpvReflectDescriptorBinding* const pBinding, VkShaderStageFlags shaderStage)
        : m_name(pBinding->name), m_set(pBinding->set), m_vkDescriptorType(VkDescriptorType(pBinding->descriptor_type))
    {
        switch (m_vkDescriptorType)
        {
        case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
            m_descriptorResource = ExtractImageDescriptor(pBinding);
            break;
        case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
            m_descriptorResource = ExtractImageDescriptor(pBinding);
            break;
        case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
            m_descriptorResource = ExtractBufferLayout(pBinding);
            break;
        }

        m_vkDescriptorSetLayoutBinding.binding = pBinding->binding;
        m_vkDescriptorSetLayoutBinding.descriptorType = m_vkDescriptorType;
        m_vkDescriptorSetLayoutBinding.descriptorCount = pBinding->count;
        m_vkDescriptorSetLayoutBinding.stageFlags = shaderStage;
        m_vkDescriptorSetLayoutBinding.pImmutableSamplers = nullptr;
    }



    // Getters:
    const std::string& DescriptorReflection::GetName() const
    {
        return m_name;
    }
    uint32_t DescriptorReflection::GetSet() const
    {
        return m_set;
    }
    VkDescriptorType DescriptorReflection::GetVkDescriptorType() const
    {
        return m_vkDescriptorType;
    }
    const ImageDescriptor* DescriptorReflection::GetImageDescriptor() const
    {
        return std::get_if<ImageDescriptor>(&m_descriptorResource);
    }
    const UniformBufferDescriptor* DescriptorReflection::GetUniformBufferDescriptor() const
    {
        return std::get_if<UniformBufferDescriptor>(&m_descriptorResource);
    }
    VkDescriptorSetLayoutBinding DescriptorReflection::GetVkDescriptorSetLayoutBinding() const
    {
        return m_vkDescriptorSetLayoutBinding;
    }



    // Setters:
    void DescriptorReflection::AddShaderStage(VkShaderStageFlags vkShaderStageFlags)
    {
        m_vkDescriptorSetLayoutBinding.stageFlags |= vkShaderStageFlags;
    }




    // Comparison:
    bool DescriptorReflection::IsEqual(const DescriptorReflection& other)
    {
        // This is intendet to detect same descriptor in different stages:
        // => No comparison between shaderStages or names.

        // Set and descriptorType:
        if (m_set != other.m_set) return false;
        if (m_vkDescriptorType != other.m_vkDescriptorType) return false;

        // Image descriptor:
        if (m_vkDescriptorType == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE || m_vkDescriptorType == VK_DESCRIPTOR_TYPE_STORAGE_IMAGE)
            if (GetImageDescriptor()->vkImageViewType != other.GetImageDescriptor()->vkImageViewType)
                return false;

        // Uniform buffer descriptor:
        if (m_vkDescriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
            if (!GetUniformBufferDescriptor()->bufferLayout.IsLayoutCompatible(other.GetUniformBufferDescriptor()->bufferLayout))
                return false;

        if (m_vkDescriptorSetLayoutBinding.binding != other.m_vkDescriptorSetLayoutBinding.binding) return false;
        if (m_vkDescriptorSetLayoutBinding.descriptorCount != other.m_vkDescriptorSetLayoutBinding.descriptorCount) return false;

        // Descriptors match:
        return true;
    }



    // Debugging:
    std::string DescriptorReflection::ToString(int indent) const
    {
        std::ostringstream ss;
        std::string indentStr = std::string(indent, ' ');
        ss << indentStr;
        ss << "name: " << m_name << ", ";
        ss << "set: " << m_set << ", ";
        ss << "VkDescriptorSetLayoutBinding{" << emberVulkanUtility::ToString(m_vkDescriptorSetLayoutBinding) << "}";
        switch (m_vkDescriptorType)
        {
            case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
                ss << "\n" << GetUniformBufferDescriptor()->bufferLayout.ToString(indent + 2);
                break;
            case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
                ss << ", vkImageViewType(sampled): " << emberVulkanUtility::ToString(GetImageDescriptor()->vkImageViewType);
                break;
            case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
                ss << ", vkImageViewType(storage): " << emberVulkanUtility::ToString(GetImageDescriptor()->vkImageViewType);
                break;
        }
        return ss.str();
    }



    // Private methods:
    ImageDescriptor DescriptorReflection::ExtractImageDescriptor(const SpvReflectDescriptorBinding* const pBinding)
    {
        ImageDescriptor imageDescriptor;
        switch (pBinding->image.dim)
        {
            case SpvDim1D:
                imageDescriptor.vkImageViewType = pBinding->image.arrayed ? VK_IMAGE_VIEW_TYPE_1D_ARRAY : VK_IMAGE_VIEW_TYPE_1D;
                break;
            case SpvDim2D:
                imageDescriptor.vkImageViewType = pBinding->image.arrayed ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D;
                break;
            case SpvDim3D:
                imageDescriptor.vkImageViewType = VK_IMAGE_VIEW_TYPE_3D; // VK_IMAGE_VIEW_TYPE_3D_ARRAY does not exist in HLSL.
                break;
            case SpvDimCube:
                imageDescriptor.vkImageViewType = pBinding->image.arrayed ? VK_IMAGE_VIEW_TYPE_CUBE_ARRAY : VK_IMAGE_VIEW_TYPE_CUBE;
                break;
            default:
                imageDescriptor.vkImageViewType = VK_IMAGE_VIEW_TYPE_MAX_ENUM;
                break;
        }
        return imageDescriptor;
    }
    UniformBufferDescriptor DescriptorReflection::ExtractBufferLayout(const SpvReflectDescriptorBinding* const pBinding)
    {
        const SpvReflectBlockVariable& blockReflection = pBinding->block;
        emberBufferLayout::BufferLayout bufferLayout(blockReflection.name);
        // Add members:
        for (uint32_t memberIndex = 0; memberIndex < blockReflection.member_count; memberIndex++)
        {
            SpvReflectBlockVariable& memberReflection = blockReflection.members[memberIndex];
            emberBufferLayout::BufferMember member = ExtractBufferMember(memberReflection, memberReflection.offset);
            bufferLayout.AddMember(std::move(member));
        }

        return UniformBufferDescriptor{ pBinding->set, pBinding->binding, bufferLayout };
    }
    emberBufferLayout::BufferMember DescriptorReflection::ExtractBufferMember(const SpvReflectBlockVariable& memberReflection, uint32_t baseOffset, uint32_t arrayIndex)
    {
        // Compute name, offset, size depending on array member or not:
        bool isArrayMember = (arrayIndex != UINT32_MAX);
        std::string name = memberReflection.name + (isArrayMember ? "[" + std::to_string(arrayIndex) + "]" : "");
        uint32_t offset = baseOffset + (isArrayMember ? memberReflection.array.stride * arrayIndex : 0);
        uint32_t size = (isArrayMember ? memberReflection.array.stride : memberReflection.size);

        // Create member:
        emberBufferLayout::BufferMember member(name, offset, size);

        // Recursively add sub members:
        if (IsStruct(memberReflection) && !IsArray(memberReflection))
        { // Submember struct reflection:
            for (uint32_t subMemberIndex = 0; subMemberIndex < memberReflection.member_count; subMemberIndex++)
            {
                SpvReflectBlockVariable& subMemberReflection = memberReflection.members[subMemberIndex];
                emberBufferLayout::BufferMember subMember = ExtractBufferMember(subMemberReflection, offset);
                member.AddSubMember(subMember);
            }
        }
        if (IsArray(memberReflection) && !isArrayMember)
        { // Submember array reflection: (array of array not possible).
            for (uint32_t arrayIndex = 0; arrayIndex < memberReflection.array.dims[0]; arrayIndex++)
            {
                emberBufferLayout::BufferMember subElement = ExtractBufferMember(memberReflection, offset, arrayIndex);
                member.AddSubMember(subElement);
            }
        }

        return member;
    }
    bool DescriptorReflection::IsStruct(const SpvReflectBlockVariable& memberReflection) const
    {
        return memberReflection.member_count > 0;
    }
    bool DescriptorReflection::IsArray(const SpvReflectBlockVariable& memberReflection) const
    {
        return memberReflection.array.dims_count > 0;
    }
}