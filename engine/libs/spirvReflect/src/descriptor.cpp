#include "descriptor.h"
#include "spirvReflectToString.h"
#include "vulkanDescriptorSetLayoutBindingToString.h"
#include "vulkanImageViewTypeToString.h"
#include "vulkanShaderStageFlagsToString.h"
#include <sstream>



namespace emberSpirvReflect
{
    // Public methods:
    // Constructor:
    Descriptor::Descriptor(const SpvReflectDescriptorBinding* const pBinding, VkShaderStageFlags shaderStage)
        : uniformBufferBlock(), vkImageViewType(VK_IMAGE_VIEW_TYPE_MAX_ENUM)
    {
        name = pBinding->name;
        set = pBinding->set;
        vkDescriptorSetLayoutBinding.binding = pBinding->binding;
        vkDescriptorSetLayoutBinding.descriptorType = VkDescriptorType(pBinding->descriptor_type);
        vkDescriptorSetLayoutBinding.descriptorCount = pBinding->count;
        vkDescriptorSetLayoutBinding.stageFlags = shaderStage;
        vkDescriptorSetLayoutBinding.pImmutableSamplers = nullptr;  // not in use yet.

        switch (vkDescriptorSetLayoutBinding.descriptorType)
        {
            case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
                uniformBufferBlock = UniformBufferBlock(pBinding->block, set, vkDescriptorSetLayoutBinding.binding);
                break;
            case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
                vkImageViewType = ExtractVkImageViewType(pBinding);
                break;
            case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
                vkImageViewType = ExtractVkImageViewType(pBinding);
                break;
        }
    }



    // Comparison:
    bool Descriptor::IsEqual(const Descriptor& other)
    {
        // This is intendet to detect same descriptor in different stages:
        // => No comparison between shaderStages or names.

        // Basics:
        if (set != other.set) return false;
        if (vkDescriptorSetLayoutBinding.binding != other.vkDescriptorSetLayoutBinding.binding) return false;
        if (vkDescriptorSetLayoutBinding.descriptorCount != other.vkDescriptorSetLayoutBinding.descriptorCount) return false;
        if (vkDescriptorSetLayoutBinding.descriptorType != other.vkDescriptorSetLayoutBinding.descriptorType) return false;

        // Uniform buffers:
        if (vkDescriptorSetLayoutBinding.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
        {
            if (!uniformBufferBlock.IsLayoutCompatible(other.uniformBufferBlock))
                return false;
        }

        // Images:
        if (vkDescriptorSetLayoutBinding.descriptorType == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE || vkDescriptorSetLayoutBinding.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_IMAGE)
        {
            if (vkImageViewType != other.vkImageViewType)
                return false;
        }

        // Descriptors match:
        return true;
    }



    // Debugging:
    std::string Descriptor::ToString(int indent) const
    {
        std::ostringstream ss;
        std::string indentStr = std::string(indent, ' ');
        ss << indentStr;
        ss << "name: "<< name << ", ";
        ss << "set: " << set << ", ";
        ss << "VkDescriptorSetLayoutBinding{" << emberVulkanUtility::ToString(vkDescriptorSetLayoutBinding) << "}";
        switch (vkDescriptorSetLayoutBinding.descriptorType)
        {
        case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
            ss << "\n" << uniformBufferBlock.ToString(indent + 2);
            break;
        case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
            ss << ", vkImageViewType(sampled): " << emberVulkanUtility::ToString(vkImageViewType);
            break;
        case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
            ss << ", vkImageViewType(storage): " << emberVulkanUtility::ToString(vkImageViewType);
            break;
        }
        return ss.str();
    }



    // Private methods:
    VkImageViewType Descriptor::ExtractVkImageViewType(const SpvReflectDescriptorBinding* const pBinding)
    {
        switch (pBinding->image.dim)
        {
            case SpvDim1D:
                return pBinding->image.arrayed ? VK_IMAGE_VIEW_TYPE_1D_ARRAY : VK_IMAGE_VIEW_TYPE_1D;
            case SpvDim2D:
                return pBinding->image.arrayed ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D;
            case SpvDim3D:
                return VK_IMAGE_VIEW_TYPE_3D; // VK_IMAGE_VIEW_TYPE_3D_ARRAY does not exist in HLSL.
            case SpvDimCube:
                return pBinding->image.arrayed ? VK_IMAGE_VIEW_TYPE_CUBE_ARRAY : VK_IMAGE_VIEW_TYPE_CUBE;
            default:
                return VK_IMAGE_VIEW_TYPE_MAX_ENUM;
        }
    }
}