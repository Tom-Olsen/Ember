#include "descriptor.h"
#include "spirvReflectToString.h"
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
        binding = pBinding->binding;
        descriptorCount = pBinding->count;
        descriptorType = pBinding->descriptor_type;
        this->shaderStage = shaderStage;

        switch (descriptorType)
        {
            case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
                uniformBufferBlock = UniformBufferBlock(pBinding->block, set, binding);
                break;
            case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
                vkImageViewType = ExtractVkImageViewType(pBinding);
                break;
            case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE:
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
        if (binding != other.binding) return false;
        if (descriptorCount != other.descriptorCount) return false;
        if (descriptorType != other.descriptorType) return false;

        // Uniform buffers:
        if (!uniformBufferBlock.IsLayoutCompatible(other.uniformBufferBlock))
            return false;

        // Images:
        if (vkImageViewType != other.vkImageViewType)
            return false;

        // Descriptors match:
        return true;
    }



    // Debugging:
    std::string Descriptor::ToString(int indent) const
    {
        std::ostringstream ss;
        ss << std::string(indent, ' ');
        ss << "name: "<< name << ", ";
        ss << "set: " << set << ", ";
        ss << "binding: " << binding << ", ";
        ss << "descriptorCount: " << descriptorCount << ", ";
        ss << "descriptorType: " << emberSpirvReflect::ToString(descriptorType) << ", ";
        ss << "shaderStage: " << emberVulkanUtility::ToString_VkShaderStageFlags(shaderStage);
        switch (descriptorType)
        {
        case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
            ss << "\n" << uniformBufferBlock.ToString(indent + 2);
            break;
        case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
            ss << ", vkImageViewType(sampled): " << emberVulkanUtility::ToString(vkImageViewType);
            break;
        case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE:
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