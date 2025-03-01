#include "vulkanUtility.h"
#include <vector>



namespace emberEngine
{
    namespace vulkanObjToString
    {
        std::string VkVertexInputAttributeDescriptionToString(VkVertexInputAttributeDescription vkVertexInputAttributeDescription)
        {
            std::string output = "";
            output += "location: " + std::to_string(vkVertexInputAttributeDescription.location);
            output += ", binding: " + std::to_string(vkVertexInputAttributeDescription.binding);
            output += ", format: " + VkFormatToString(vkVertexInputAttributeDescription.format);
            output += ", offset: " + std::to_string(vkVertexInputAttributeDescription.offset);
            return output;
        }
        std::string VkImageViewTypeToString(VkImageViewType vkImageViewType)
        {
            switch (vkImageViewType)
            {
            case VK_IMAGE_VIEW_TYPE_1D: return std::string("VK_IMAGE_VIEW_TYPE_1D");
            case VK_IMAGE_VIEW_TYPE_2D: return std::string("VK_IMAGE_VIEW_TYPE_2D");
            case VK_IMAGE_VIEW_TYPE_3D: return std::string("VK_IMAGE_VIEW_TYPE_3D");
            case VK_IMAGE_VIEW_TYPE_CUBE: return std::string("VK_IMAGE_VIEW_TYPE_CUBE");
            case VK_IMAGE_VIEW_TYPE_1D_ARRAY: return std::string("VK_IMAGE_VIEW_TYPE_1D_ARRAY");
            case VK_IMAGE_VIEW_TYPE_2D_ARRAY: return std::string("VK_IMAGE_VIEW_TYPE_2D_ARRAY");
            case VK_IMAGE_VIEW_TYPE_CUBE_ARRAY: return std::string("VK_IMAGE_VIEW_TYPE_CUBE_ARRAY");
            case VK_IMAGE_VIEW_TYPE_MAX_ENUM: return std::string("VK_IMAGE_VIEW_TYPE_MAX_ENUM");
            default: return "UNKNOWN_VK_IMAGE_VIEW_TYPE (maybe vulkan backend has been updated)";
            }
        }
        std::string VkImageLayoutToString(VkImageLayout vkImageView)
        {
            switch (vkImageView)
            {
            case VK_IMAGE_LAYOUT_UNDEFINED: return std::string("VK_IMAGE_LAYOUT_UNDEFINED");
            case VK_IMAGE_LAYOUT_GENERAL: return std::string("VK_IMAGE_LAYOUT_GENERAL");
            case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL: return std::string("VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL");
            case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL: return std::string("VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL");
            case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL: return std::string("VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL");
            case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL: return std::string("VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL");
            case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL: return std::string("VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL");
            case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL: return std::string("VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL");
            case VK_IMAGE_LAYOUT_PREINITIALIZED: return std::string("VK_IMAGE_LAYOUT_PREINITIALIZED");
            case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL: return std::string("VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL");
            case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL: return std::string("VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL");
            case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL: return std::string("VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL");
            case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL: return std::string("VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL");
            case VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL: return std::string("VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL");
            case VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL: return std::string("VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL");
            case VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL: return std::string("VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL");
            case VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL: return std::string("VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL");
            case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR: return std::string("VK_IMAGE_LAYOUT_PRESENT_SRC_KHR");
            case VK_IMAGE_LAYOUT_VIDEO_DECODE_DST_KHR: return std::string("VK_IMAGE_LAYOUT_VIDEO_DECODE_DST_KHR");
            case VK_IMAGE_LAYOUT_VIDEO_DECODE_SRC_KHR: return std::string("VK_IMAGE_LAYOUT_VIDEO_DECODE_SRC_KHR");
            case VK_IMAGE_LAYOUT_VIDEO_DECODE_DPB_KHR: return std::string("VK_IMAGE_LAYOUT_VIDEO_DECODE_DPB_KHR");
            case VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR: return std::string("VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR");
            case VK_IMAGE_LAYOUT_FRAGMENT_DENSITY_MAP_OPTIMAL_EXT: return std::string("VK_IMAGE_LAYOUT_FRAGMENT_DENSITY_MAP_OPTIMAL_EXT");
            case VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR: return std::string("VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR");
            case VK_IMAGE_LAYOUT_RENDERING_LOCAL_READ_KHR: return std::string("VK_IMAGE_LAYOUT_RENDERING_LOCAL_READ_KHR");
            case VK_IMAGE_LAYOUT_VIDEO_ENCODE_DST_KHR: return std::string("VK_IMAGE_LAYOUT_VIDEO_ENCODE_DST_KHR");
            case VK_IMAGE_LAYOUT_VIDEO_ENCODE_SRC_KHR: return std::string("VK_IMAGE_LAYOUT_VIDEO_ENCODE_SRC_KHR");
            case VK_IMAGE_LAYOUT_VIDEO_ENCODE_DPB_KHR: return std::string("VK_IMAGE_LAYOUT_VIDEO_ENCODE_DPB_KHR");
            case VK_IMAGE_LAYOUT_ATTACHMENT_FEEDBACK_LOOP_OPTIMAL_EXT: return std::string("VK_IMAGE_LAYOUT_ATTACHMENT_FEEDBACK_LOOP_OPTIMAL_EXT");
            case VK_IMAGE_LAYOUT_MAX_ENUM: return std::string("VK_IMAGE_LAYOUT_MAX_ENUM");
            default: return "UNKNOWN_VK_IMAGE_LAYOUT (maybe vulkan backend has been updated)";
            }
        }
        std::string VkVertexInputRateToString(VkVertexInputRate vkVertexInputRate)
        {
            switch (vkVertexInputRate)
            {
            case VK_VERTEX_INPUT_RATE_VERTEX: return std::string("VK_VERTEX_INPUT_RATE_VERTEX");
            case VK_VERTEX_INPUT_RATE_INSTANCE: return std::string("VK_VERTEX_INPUT_RATE_INSTANCE");
            case VK_VERTEX_INPUT_RATE_MAX_ENUM: return std::string("VK_VERTEX_INPUT_RATE_MAX_ENUM");
            default: return "UNKNOWN_VK_VERTEX_INPUT_RATE (maybe vulkan backend has been updated)";
            }
        }
        std::string VkDescriptorTypeToString(VkDescriptorType vkDescriptorType)
        {
            switch (vkDescriptorType)
            {
            case VK_DESCRIPTOR_TYPE_SAMPLER: return std::string("VK_DESCRIPTOR_TYPE_SAMPLER");
            case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER: return std::string("VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER");
            case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE: return std::string("VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE");
            case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE: return std::string("VK_DESCRIPTOR_TYPE_STORAGE_IMAGE");
            case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER: return std::string("VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER");
            case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER: return std::string("VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER");
            case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER: return std::string("VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER");
            case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER: return std::string("VK_DESCRIPTOR_TYPE_STORAGE_BUFFER");
            case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC: return std::string("VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC");
            case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC: return std::string("VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC");
            case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT: return std::string("VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT");
            case VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK: return std::string("VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK");
            case VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR: return std::string("VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR");
            case VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV: return std::string("VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV");
            case VK_DESCRIPTOR_TYPE_SAMPLE_WEIGHT_IMAGE_QCOM: return std::string("VK_DESCRIPTOR_TYPE_SAMPLE_WEIGHT_IMAGE_QCOM");
            case VK_DESCRIPTOR_TYPE_BLOCK_MATCH_IMAGE_QCOM: return std::string("VK_DESCRIPTOR_TYPE_BLOCK_MATCH_IMAGE_QCOM");
            case VK_DESCRIPTOR_TYPE_MUTABLE_EXT: return std::string("VK_DESCRIPTOR_TYPE_MUTABLE_EXT");
            case VK_DESCRIPTOR_TYPE_MAX_ENUM: return std::string("VK_DESCRIPTOR_TYPE_MAX_ENUM");
            default: return "UNKNOWN_VK_DESCRIPTOR_TYPE (maybe vulkan backend has been updated)";
            }
        }
        std::string VkFormatToString(VkFormat vkFormat)
        {
            switch (vkFormat)
            {
            case VK_FORMAT_UNDEFINED: return std::string("VK_FORMAT_UNDEFINED");
            case VK_FORMAT_R4G4_UNORM_PACK8: return std::string("VK_FORMAT_R4G4_UNORM_PACK8");
            case VK_FORMAT_R4G4B4A4_UNORM_PACK16: return std::string("VK_FORMAT_R4G4B4A4_UNORM_PACK16");
            case VK_FORMAT_B4G4R4A4_UNORM_PACK16: return std::string("VK_FORMAT_B4G4R4A4_UNORM_PACK16");
            case VK_FORMAT_R5G6B5_UNORM_PACK16: return std::string("VK_FORMAT_R5G6B5_UNORM_PACK16");
            case VK_FORMAT_B5G6R5_UNORM_PACK16: return std::string("VK_FORMAT_B5G6R5_UNORM_PACK16");
            case VK_FORMAT_R5G5B5A1_UNORM_PACK16: return std::string("VK_FORMAT_R5G5B5A1_UNORM_PACK16");
            case VK_FORMAT_B5G5R5A1_UNORM_PACK16: return std::string("VK_FORMAT_B5G5R5A1_UNORM_PACK16");
            case VK_FORMAT_A1R5G5B5_UNORM_PACK16: return std::string("VK_FORMAT_A1R5G5B5_UNORM_PACK16");
            case VK_FORMAT_R8_UNORM: return std::string("VK_FORMAT_R8_UNORM");
            case VK_FORMAT_R8_SNORM: return std::string("VK_FORMAT_R8_SNORM");
            case VK_FORMAT_R8_USCALED: return std::string("VK_FORMAT_R8_USCALED");
            case VK_FORMAT_R8_SSCALED: return std::string("VK_FORMAT_R8_SSCALED");
            case VK_FORMAT_R8_UINT: return std::string("VK_FORMAT_R8_UINT");
            case VK_FORMAT_R8_SINT: return std::string("VK_FORMAT_R8_SINT");
            case VK_FORMAT_R8_SRGB: return std::string("VK_FORMAT_R8_SRGB");
            case VK_FORMAT_R8G8_UNORM: return std::string("VK_FORMAT_R8G8_UNORM");
            case VK_FORMAT_R8G8_SNORM: return std::string("VK_FORMAT_R8G8_SNORM");
            case VK_FORMAT_R8G8_USCALED: return std::string("VK_FORMAT_R8G8_USCALED");
            case VK_FORMAT_R8G8_SSCALED: return std::string("VK_FORMAT_R8G8_SSCALED");
            case VK_FORMAT_R8G8_UINT: return std::string("VK_FORMAT_R8G8_UINT");
            case VK_FORMAT_R8G8_SINT: return std::string("VK_FORMAT_R8G8_SINT");
            case VK_FORMAT_R8G8_SRGB: return std::string("VK_FORMAT_R8G8_SRGB");
            case VK_FORMAT_R8G8B8_UNORM: return std::string("VK_FORMAT_R8G8B8_UNORM");
            case VK_FORMAT_R8G8B8_SNORM: return std::string("VK_FORMAT_R8G8B8_SNORM");
            case VK_FORMAT_R8G8B8_USCALED: return std::string("VK_FORMAT_R8G8B8_USCALED");
            case VK_FORMAT_R8G8B8_SSCALED: return std::string("VK_FORMAT_R8G8B8_SSCALED");
            case VK_FORMAT_R8G8B8_UINT: return std::string("VK_FORMAT_R8G8B8_UINT");
            case VK_FORMAT_R8G8B8_SINT: return std::string("VK_FORMAT_R8G8B8_SINT");
            case VK_FORMAT_R8G8B8_SRGB: return std::string("VK_FORMAT_R8G8B8_SRGB");
            case VK_FORMAT_B8G8R8_UNORM: return std::string("VK_FORMAT_B8G8R8_UNORM");
            case VK_FORMAT_B8G8R8_SNORM: return std::string("VK_FORMAT_B8G8R8_SNORM");
            case VK_FORMAT_B8G8R8_USCALED: return std::string("VK_FORMAT_B8G8R8_USCALED");
            case VK_FORMAT_B8G8R8_SSCALED: return std::string("VK_FORMAT_B8G8R8_SSCALED");
            case VK_FORMAT_B8G8R8_UINT: return std::string("VK_FORMAT_B8G8R8_UINT");
            case VK_FORMAT_B8G8R8_SINT: return std::string("VK_FORMAT_B8G8R8_SINT");
            case VK_FORMAT_B8G8R8_SRGB: return std::string("VK_FORMAT_B8G8R8_SRGB");
            case VK_FORMAT_R8G8B8A8_UNORM: return std::string("VK_FORMAT_R8G8B8A8_UNORM");
            case VK_FORMAT_R8G8B8A8_SNORM: return std::string("VK_FORMAT_R8G8B8A8_SNORM");
            case VK_FORMAT_R8G8B8A8_USCALED: return std::string("VK_FORMAT_R8G8B8A8_USCALED");
            case VK_FORMAT_R8G8B8A8_SSCALED: return std::string("VK_FORMAT_R8G8B8A8_SSCALED");
            case VK_FORMAT_R8G8B8A8_UINT: return std::string("VK_FORMAT_R8G8B8A8_UINT");
            case VK_FORMAT_R8G8B8A8_SINT: return std::string("VK_FORMAT_R8G8B8A8_SINT");
            case VK_FORMAT_R8G8B8A8_SRGB: return std::string("VK_FORMAT_R8G8B8A8_SRGB");
            case VK_FORMAT_B8G8R8A8_UNORM: return std::string("VK_FORMAT_B8G8R8A8_UNORM");
            case VK_FORMAT_B8G8R8A8_SNORM: return std::string("VK_FORMAT_B8G8R8A8_SNORM");
            case VK_FORMAT_B8G8R8A8_USCALED: return std::string("VK_FORMAT_B8G8R8A8_USCALED");
            case VK_FORMAT_B8G8R8A8_SSCALED: return std::string("VK_FORMAT_B8G8R8A8_SSCALED");
            case VK_FORMAT_B8G8R8A8_UINT: return std::string("VK_FORMAT_B8G8R8A8_UINT");
            case VK_FORMAT_B8G8R8A8_SINT: return std::string("VK_FORMAT_B8G8R8A8_SINT");
            case VK_FORMAT_B8G8R8A8_SRGB: return std::string("VK_FORMAT_B8G8R8A8_SRGB");
            case VK_FORMAT_A8B8G8R8_UNORM_PACK32: return std::string("VK_FORMAT_A8B8G8R8_UNORM_PACK32");
            case VK_FORMAT_A8B8G8R8_SNORM_PACK32: return std::string("VK_FORMAT_A8B8G8R8_SNORM_PACK32");
            case VK_FORMAT_A8B8G8R8_USCALED_PACK32: return std::string("VK_FORMAT_A8B8G8R8_USCALED_PACK32");
            case VK_FORMAT_A8B8G8R8_SSCALED_PACK32: return std::string("VK_FORMAT_A8B8G8R8_SSCALED_PACK32");
            case VK_FORMAT_A8B8G8R8_UINT_PACK32: return std::string("VK_FORMAT_A8B8G8R8_UINT_PACK32");
            case VK_FORMAT_A8B8G8R8_SINT_PACK32: return std::string("VK_FORMAT_A8B8G8R8_SINT_PACK32");
            case VK_FORMAT_A8B8G8R8_SRGB_PACK32: return std::string("VK_FORMAT_A8B8G8R8_SRGB_PACK32");
            case VK_FORMAT_A2R10G10B10_UNORM_PACK32: return std::string("VK_FORMAT_A2R10G10B10_UNORM_PACK32");
            case VK_FORMAT_A2R10G10B10_SNORM_PACK32: return std::string("VK_FORMAT_A2R10G10B10_SNORM_PACK32");
            case VK_FORMAT_A2R10G10B10_USCALED_PACK32: return std::string("VK_FORMAT_A2R10G10B10_USCALED_PACK32");
            case VK_FORMAT_A2R10G10B10_SSCALED_PACK32: return std::string("VK_FORMAT_A2R10G10B10_SSCALED_PACK32");
            case VK_FORMAT_A2R10G10B10_UINT_PACK32: return std::string("VK_FORMAT_A2R10G10B10_UINT_PACK32");
            case VK_FORMAT_A2R10G10B10_SINT_PACK32: return std::string("VK_FORMAT_A2R10G10B10_SINT_PACK32");
            case VK_FORMAT_A2B10G10R10_UNORM_PACK32: return std::string("VK_FORMAT_A2B10G10R10_UNORM_PACK32");
            case VK_FORMAT_A2B10G10R10_SNORM_PACK32: return std::string("VK_FORMAT_A2B10G10R10_SNORM_PACK32");
            case VK_FORMAT_A2B10G10R10_USCALED_PACK32: return std::string("VK_FORMAT_A2B10G10R10_USCALED_PACK32");
            case VK_FORMAT_A2B10G10R10_SSCALED_PACK32: return std::string("VK_FORMAT_A2B10G10R10_SSCALED_PACK32");
            case VK_FORMAT_A2B10G10R10_UINT_PACK32: return std::string("VK_FORMAT_A2B10G10R10_UINT_PACK32");
            case VK_FORMAT_A2B10G10R10_SINT_PACK32: return std::string("VK_FORMAT_A2B10G10R10_SINT_PACK32");
            case VK_FORMAT_R16_UNORM: return std::string("VK_FORMAT_R16_UNORM");
            case VK_FORMAT_R16_SNORM: return std::string("VK_FORMAT_R16_SNORM");
            case VK_FORMAT_R16_USCALED: return std::string("VK_FORMAT_R16_USCALED");
            case VK_FORMAT_R16_SSCALED: return std::string("VK_FORMAT_R16_SSCALED");
            case VK_FORMAT_R16_UINT: return std::string("VK_FORMAT_R16_UINT");
            case VK_FORMAT_R16_SINT: return std::string("VK_FORMAT_R16_SINT");
            case VK_FORMAT_R16_SFLOAT: return std::string("VK_FORMAT_R16_SFLOAT");
            case VK_FORMAT_R16G16_UNORM: return std::string("VK_FORMAT_R16G16_UNORM");
            case VK_FORMAT_R16G16_SNORM: return std::string("VK_FORMAT_R16G16_SNORM");
            case VK_FORMAT_R16G16_USCALED: return std::string("VK_FORMAT_R16G16_USCALED");
            case VK_FORMAT_R16G16_SSCALED: return std::string("VK_FORMAT_R16G16_SSCALED");
            case VK_FORMAT_R16G16_UINT: return std::string("VK_FORMAT_R16G16_UINT");
            case VK_FORMAT_R16G16_SINT: return std::string("VK_FORMAT_R16G16_SINT");
            case VK_FORMAT_R16G16_SFLOAT: return std::string("VK_FORMAT_R16G16_SFLOAT");
            case VK_FORMAT_R16G16B16_UNORM: return std::string("VK_FORMAT_R16G16B16_UNORM");
            case VK_FORMAT_R16G16B16_SNORM: return std::string("VK_FORMAT_R16G16B16_SNORM");
            case VK_FORMAT_R16G16B16_USCALED: return std::string("VK_FORMAT_R16G16B16_USCALED");
            case VK_FORMAT_R16G16B16_SSCALED: return std::string("VK_FORMAT_R16G16B16_SSCALED");
            case VK_FORMAT_R16G16B16_UINT: return std::string("VK_FORMAT_R16G16B16_UINT");
            case VK_FORMAT_R16G16B16_SINT: return std::string("VK_FORMAT_R16G16B16_SINT");
            case VK_FORMAT_R16G16B16_SFLOAT: return std::string("VK_FORMAT_R16G16B16_SFLOAT");
            case VK_FORMAT_R16G16B16A16_UNORM: return std::string("VK_FORMAT_R16G16B16A16_UNORM");
            case VK_FORMAT_R16G16B16A16_SNORM: return std::string("VK_FORMAT_R16G16B16A16_SNORM");
            case VK_FORMAT_R16G16B16A16_USCALED: return std::string("VK_FORMAT_R16G16B16A16_USCALED");
            case VK_FORMAT_R16G16B16A16_SSCALED: return std::string("VK_FORMAT_R16G16B16A16_SSCALED");
            case VK_FORMAT_R16G16B16A16_UINT: return std::string("VK_FORMAT_R16G16B16A16_UINT");
            case VK_FORMAT_R16G16B16A16_SINT: return std::string("VK_FORMAT_R16G16B16A16_SINT");
            case VK_FORMAT_R16G16B16A16_SFLOAT: return std::string("VK_FORMAT_R16G16B16A16_SFLOAT");
            case VK_FORMAT_R32_UINT: return std::string("VK_FORMAT_R32_UINT");
            case VK_FORMAT_R32_SINT: return std::string("VK_FORMAT_R32_SINT");
            case VK_FORMAT_R32_SFLOAT: return std::string("VK_FORMAT_R32_SFLOAT");
            case VK_FORMAT_R32G32_UINT: return std::string("VK_FORMAT_R32G32_UINT");
            case VK_FORMAT_R32G32_SINT: return std::string("VK_FORMAT_R32G32_SINT");
            case VK_FORMAT_R32G32_SFLOAT: return std::string("VK_FORMAT_R32G32_SFLOAT");
            case VK_FORMAT_R32G32B32_UINT: return std::string("VK_FORMAT_R32G32B32_UINT");
            case VK_FORMAT_R32G32B32_SINT: return std::string("VK_FORMAT_R32G32B32_SINT");
            case VK_FORMAT_R32G32B32_SFLOAT: return std::string("VK_FORMAT_R32G32B32_SFLOAT");
            case VK_FORMAT_R32G32B32A32_UINT: return std::string("VK_FORMAT_R32G32B32A32_UINT");
            case VK_FORMAT_R32G32B32A32_SINT: return std::string("VK_FORMAT_R32G32B32A32_SINT");
            case VK_FORMAT_R32G32B32A32_SFLOAT: return std::string("VK_FORMAT_R32G32B32A32_SFLOAT");
            case VK_FORMAT_R64_UINT: return std::string("VK_FORMAT_R64_UINT");
            case VK_FORMAT_R64_SINT: return std::string("VK_FORMAT_R64_SINT");
            case VK_FORMAT_R64_SFLOAT: return std::string("VK_FORMAT_R64_SFLOAT");
            case VK_FORMAT_R64G64_UINT: return std::string("VK_FORMAT_R64G64_UINT");
            case VK_FORMAT_R64G64_SINT: return std::string("VK_FORMAT_R64G64_SINT");
            case VK_FORMAT_R64G64_SFLOAT: return std::string("VK_FORMAT_R64G64_SFLOAT");
            case VK_FORMAT_R64G64B64_UINT: return std::string("VK_FORMAT_R64G64B64_UINT");
            case VK_FORMAT_R64G64B64_SINT: return std::string("VK_FORMAT_R64G64B64_SINT");
            case VK_FORMAT_R64G64B64_SFLOAT: return std::string("VK_FORMAT_R64G64B64_SFLOAT");
            case VK_FORMAT_R64G64B64A64_UINT: return std::string("VK_FORMAT_R64G64B64A64_UINT");
            case VK_FORMAT_R64G64B64A64_SINT: return std::string("VK_FORMAT_R64G64B64A64_SINT");
            case VK_FORMAT_R64G64B64A64_SFLOAT: return std::string("VK_FORMAT_R64G64B64A64_SFLOAT");
            case VK_FORMAT_B10G11R11_UFLOAT_PACK32: return std::string("VK_FORMAT_B10G11R11_UFLOAT_PACK32");
            case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32: return std::string("VK_FORMAT_E5B9G9R9_UFLOAT_PACK32");
            case VK_FORMAT_D16_UNORM: return std::string("VK_FORMAT_D16_UNORM");
            case VK_FORMAT_X8_D24_UNORM_PACK32: return std::string("VK_FORMAT_X8_D24_UNORM_PACK32");
            case VK_FORMAT_D32_SFLOAT: return std::string("VK_FORMAT_D32_SFLOAT");
            case VK_FORMAT_S8_UINT: return std::string("VK_FORMAT_S8_UINT");
            case VK_FORMAT_D16_UNORM_S8_UINT: return std::string("VK_FORMAT_D16_UNORM_S8_UINT");
            case VK_FORMAT_D24_UNORM_S8_UINT: return std::string("VK_FORMAT_D24_UNORM_S8_UINT");
            case VK_FORMAT_D32_SFLOAT_S8_UINT: return std::string("VK_FORMAT_D32_SFLOAT_S8_UINT");
            case VK_FORMAT_BC1_RGB_UNORM_BLOCK: return std::string("VK_FORMAT_BC1_RGB_UNORM_BLOCK");
            case VK_FORMAT_BC1_RGB_SRGB_BLOCK: return std::string("VK_FORMAT_BC1_RGB_SRGB_BLOCK");
            case VK_FORMAT_BC1_RGBA_UNORM_BLOCK: return std::string("VK_FORMAT_BC1_RGBA_UNORM_BLOCK");
            case VK_FORMAT_BC1_RGBA_SRGB_BLOCK: return std::string("VK_FORMAT_BC1_RGBA_SRGB_BLOCK");
            case VK_FORMAT_BC2_UNORM_BLOCK: return std::string("VK_FORMAT_BC2_UNORM_BLOCK");
            case VK_FORMAT_BC2_SRGB_BLOCK: return std::string("VK_FORMAT_BC2_SRGB_BLOCK");
            case VK_FORMAT_BC3_UNORM_BLOCK: return std::string("VK_FORMAT_BC3_UNORM_BLOCK");
            case VK_FORMAT_BC3_SRGB_BLOCK: return std::string("VK_FORMAT_BC3_SRGB_BLOCK");
            case VK_FORMAT_BC4_UNORM_BLOCK: return std::string("VK_FORMAT_BC4_UNORM_BLOCK");
            case VK_FORMAT_BC4_SNORM_BLOCK: return std::string("VK_FORMAT_BC4_SNORM_BLOCK");
            case VK_FORMAT_BC5_UNORM_BLOCK: return std::string("VK_FORMAT_BC5_UNORM_BLOCK");
            case VK_FORMAT_BC5_SNORM_BLOCK: return std::string("VK_FORMAT_BC5_SNORM_BLOCK");
            case VK_FORMAT_BC6H_UFLOAT_BLOCK: return std::string("VK_FORMAT_BC6H_UFLOAT_BLOCK");
            case VK_FORMAT_BC6H_SFLOAT_BLOCK: return std::string("VK_FORMAT_BC6H_SFLOAT_BLOCK");
            case VK_FORMAT_BC7_UNORM_BLOCK: return std::string("VK_FORMAT_BC7_UNORM_BLOCK");
            case VK_FORMAT_BC7_SRGB_BLOCK: return std::string("VK_FORMAT_BC7_SRGB_BLOCK");
            case VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK: return std::string("VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK");
            case VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK: return std::string("VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK");
            case VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK: return std::string("VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK");
            case VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK: return std::string("VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK");
            case VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK: return std::string("VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK");
            case VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK: return std::string("VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK");
            case VK_FORMAT_EAC_R11_UNORM_BLOCK: return std::string("VK_FORMAT_EAC_R11_UNORM_BLOCK");
            case VK_FORMAT_EAC_R11_SNORM_BLOCK: return std::string("VK_FORMAT_EAC_R11_SNORM_BLOCK");
            case VK_FORMAT_EAC_R11G11_UNORM_BLOCK: return std::string("VK_FORMAT_EAC_R11G11_UNORM_BLOCK");
            case VK_FORMAT_EAC_R11G11_SNORM_BLOCK: return std::string("VK_FORMAT_EAC_R11G11_SNORM_BLOCK");
            case VK_FORMAT_ASTC_4x4_UNORM_BLOCK: return std::string("VK_FORMAT_ASTC_4x4_UNORM_BLOCK");
            case VK_FORMAT_ASTC_4x4_SRGB_BLOCK: return std::string("VK_FORMAT_ASTC_4x4_SRGB_BLOCK");
            case VK_FORMAT_ASTC_5x4_UNORM_BLOCK: return std::string("VK_FORMAT_ASTC_5x4_UNORM_BLOCK");
            case VK_FORMAT_ASTC_5x4_SRGB_BLOCK: return std::string("VK_FORMAT_ASTC_5x4_SRGB_BLOCK");
            case VK_FORMAT_ASTC_5x5_UNORM_BLOCK: return std::string("VK_FORMAT_ASTC_5x5_UNORM_BLOCK");
            case VK_FORMAT_ASTC_5x5_SRGB_BLOCK: return std::string("VK_FORMAT_ASTC_5x5_SRGB_BLOCK");
            case VK_FORMAT_ASTC_6x5_UNORM_BLOCK: return std::string("VK_FORMAT_ASTC_6x5_UNORM_BLOCK");
            case VK_FORMAT_ASTC_6x5_SRGB_BLOCK: return std::string("VK_FORMAT_ASTC_6x5_SRGB_BLOCK");
            case VK_FORMAT_ASTC_6x6_UNORM_BLOCK: return std::string("VK_FORMAT_ASTC_6x6_UNORM_BLOCK");
            case VK_FORMAT_ASTC_6x6_SRGB_BLOCK: return std::string("VK_FORMAT_ASTC_6x6_SRGB_BLOCK");
            case VK_FORMAT_ASTC_8x5_UNORM_BLOCK: return std::string("VK_FORMAT_ASTC_8x5_UNORM_BLOCK");
            case VK_FORMAT_ASTC_8x5_SRGB_BLOCK: return std::string("VK_FORMAT_ASTC_8x5_SRGB_BLOCK");
            case VK_FORMAT_ASTC_8x6_UNORM_BLOCK: return std::string("VK_FORMAT_ASTC_8x6_UNORM_BLOCK");
            case VK_FORMAT_ASTC_8x6_SRGB_BLOCK: return std::string("VK_FORMAT_ASTC_8x6_SRGB_BLOCK");
            case VK_FORMAT_ASTC_8x8_UNORM_BLOCK: return std::string("VK_FORMAT_ASTC_8x8_UNORM_BLOCK");
            case VK_FORMAT_ASTC_8x8_SRGB_BLOCK: return std::string("VK_FORMAT_ASTC_8x8_SRGB_BLOCK");
            case VK_FORMAT_ASTC_10x5_UNORM_BLOCK: return std::string("VK_FORMAT_ASTC_10x5_UNORM_BLOCK");
            case VK_FORMAT_ASTC_10x5_SRGB_BLOCK: return std::string("VK_FORMAT_ASTC_10x5_SRGB_BLOCK");
            case VK_FORMAT_ASTC_10x6_UNORM_BLOCK: return std::string("VK_FORMAT_ASTC_10x6_UNORM_BLOCK");
            case VK_FORMAT_ASTC_10x6_SRGB_BLOCK: return std::string("VK_FORMAT_ASTC_10x6_SRGB_BLOCK");
            case VK_FORMAT_ASTC_10x8_UNORM_BLOCK: return std::string("VK_FORMAT_ASTC_10x8_UNORM_BLOCK");
            case VK_FORMAT_ASTC_10x8_SRGB_BLOCK: return std::string("VK_FORMAT_ASTC_10x8_SRGB_BLOCK");
            case VK_FORMAT_ASTC_10x10_UNORM_BLOCK: return std::string("VK_FORMAT_ASTC_10x10_UNORM_BLOCK");
            case VK_FORMAT_ASTC_10x10_SRGB_BLOCK: return std::string("VK_FORMAT_ASTC_10x10_SRGB_BLOCK");
            case VK_FORMAT_ASTC_12x10_UNORM_BLOCK: return std::string("VK_FORMAT_ASTC_12x10_UNORM_BLOCK");
            case VK_FORMAT_ASTC_12x10_SRGB_BLOCK: return std::string("VK_FORMAT_ASTC_12x10_SRGB_BLOCK");
            case VK_FORMAT_ASTC_12x12_UNORM_BLOCK: return std::string("VK_FORMAT_ASTC_12x12_UNORM_BLOCK");
            case VK_FORMAT_ASTC_12x12_SRGB_BLOCK: return std::string("VK_FORMAT_ASTC_12x12_SRGB_BLOCK");
            case VK_FORMAT_G8B8G8R8_422_UNORM: return std::string("VK_FORMAT_G8B8G8R8_422_UNORM");
            case VK_FORMAT_B8G8R8G8_422_UNORM: return std::string("VK_FORMAT_B8G8R8G8_422_UNORM");
            case VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM: return std::string("VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM");
            case VK_FORMAT_G8_B8R8_2PLANE_420_UNORM: return std::string("VK_FORMAT_G8_B8R8_2PLANE_420_UNORM");
            case VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM: return std::string("VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM");
            case VK_FORMAT_G8_B8R8_2PLANE_422_UNORM: return std::string("VK_FORMAT_G8_B8R8_2PLANE_422_UNORM");
            case VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM: return std::string("VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM");
            case VK_FORMAT_R10X6_UNORM_PACK16: return std::string("VK_FORMAT_R10X6_UNORM_PACK16");
            case VK_FORMAT_R10X6G10X6_UNORM_2PACK16: return std::string("VK_FORMAT_R10X6G10X6_UNORM_2PACK16");
            case VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16: return std::string("VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16");
            case VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16: return std::string("VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16");
            case VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16: return std::string("VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16");
            case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16: return std::string("VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16");
            case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16: return std::string("VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16");
            case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16: return std::string("VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16");
            case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16: return std::string("VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16");
            case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16: return std::string("VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16");
            case VK_FORMAT_R12X4_UNORM_PACK16: return std::string("VK_FORMAT_R12X4_UNORM_PACK16");
            case VK_FORMAT_R12X4G12X4_UNORM_2PACK16: return std::string("VK_FORMAT_R12X4G12X4_UNORM_2PACK16");
            case VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16: return std::string("VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16");
            case VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16: return std::string("VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16");
            case VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16: return std::string("VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16");
            case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16: return std::string("VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16");
            case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16: return std::string("VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16");
            case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16: return std::string("VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16");
            case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16: return std::string("VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16");
            case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16: return std::string("VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16");
            case VK_FORMAT_G16B16G16R16_422_UNORM: return std::string("VK_FORMAT_G16B16G16R16_422_UNORM");
            case VK_FORMAT_B16G16R16G16_422_UNORM: return std::string("VK_FORMAT_B16G16R16G16_422_UNORM");
            case VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM: return std::string("VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM");
            case VK_FORMAT_G16_B16R16_2PLANE_420_UNORM: return std::string("VK_FORMAT_G16_B16R16_2PLANE_420_UNORM");
            case VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM: return std::string("VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM");
            case VK_FORMAT_G16_B16R16_2PLANE_422_UNORM: return std::string("VK_FORMAT_G16_B16R16_2PLANE_422_UNORM");
            case VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM: return std::string("VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM");
            case VK_FORMAT_G8_B8R8_2PLANE_444_UNORM: return std::string("VK_FORMAT_G8_B8R8_2PLANE_444_UNORM");
            case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16: return std::string("VK_FORMAT_G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16");
            case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16: return std::string("VK_FORMAT_G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16");
            case VK_FORMAT_G16_B16R16_2PLANE_444_UNORM: return std::string("VK_FORMAT_G16_B16R16_2PLANE_444_UNORM");
            case VK_FORMAT_A4R4G4B4_UNORM_PACK16: return std::string("VK_FORMAT_A4R4G4B4_UNORM_PACK16");
            case VK_FORMAT_A4B4G4R4_UNORM_PACK16: return std::string("VK_FORMAT_A4B4G4R4_UNORM_PACK16");
            case VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK: return std::string("VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK");
            case VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK: return std::string("VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK");
            case VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK: return std::string("VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK");
            case VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK: return std::string("VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK");
            case VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK: return std::string("VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK");
            case VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK: return std::string("VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK");
            case VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK: return std::string("VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK");
            case VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK: return std::string("VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK");
            case VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK: return std::string("VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK");
            case VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK: return std::string("VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK");
            case VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK: return std::string("VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK");
            case VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK: return std::string("VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK");
            case VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK: return std::string("VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK");
            case VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK: return std::string("VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK");
            case VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG: return std::string("VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG");
            case VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG: return std::string("VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG");
            case VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG: return std::string("VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG");
            case VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG: return std::string("VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG");
            case VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG: return std::string("VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG");
            case VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG: return std::string("VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG");
            case VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG: return std::string("VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG");
            case VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG: return std::string("VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG");
            case VK_FORMAT_R16G16_SFIXED5_NV: return std::string("VK_FORMAT_R16G16_SFIXED5_NV");
            case VK_FORMAT_A1B5G5R5_UNORM_PACK16_KHR: return std::string("VK_FORMAT_A1B5G5R5_UNORM_PACK16_KHR");
            case VK_FORMAT_A8_UNORM_KHR: return std::string("VK_FORMAT_A8_UNORM_KHR");
            case VK_FORMAT_MAX_ENUM: return std::string("VK_FORMAT_MAX_ENUM");
            default: return "UNKOWN_VK_FORMAT (maybe vulkan backend has been updated)";
            }
        }

        std::string VkShaderStageFlagsToString(VkShaderStageFlags vkShaderStageFlags)
        {
            if (vkShaderStageFlags == 0)
                return "NONE";

            struct FlagMapping
            {
                VkShaderStageFlagBits flag;
                std::string_view name;
            };

            static constexpr FlagMapping flagMappings[] =
            {
                { VK_SHADER_STAGE_VERTEX_BIT, "VK_SHADER_STAGE_VERTEX_BIT" },
                { VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT, "VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT" },
                { VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT, "VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT" },
                { VK_SHADER_STAGE_GEOMETRY_BIT, "VK_SHADER_STAGE_GEOMETRY_BIT" },
                { VK_SHADER_STAGE_FRAGMENT_BIT, "VK_SHADER_STAGE_FRAGMENT_BIT" },
                { VK_SHADER_STAGE_COMPUTE_BIT, "VK_SHADER_STAGE_COMPUTE_BIT" },
                { VK_SHADER_STAGE_TASK_BIT_EXT, "VK_SHADER_STAGE_TASK_BIT_EXT" },
                { VK_SHADER_STAGE_MESH_BIT_EXT, "VK_SHADER_STAGE_MESH_BIT_EXT" },
                { VK_SHADER_STAGE_RAYGEN_BIT_KHR, "VK_SHADER_STAGE_RAYGEN_BIT_KHR" },
                { VK_SHADER_STAGE_ANY_HIT_BIT_KHR, "VK_SHADER_STAGE_ANY_HIT_BIT_KHR" },
                { VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR, "VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR" },
                { VK_SHADER_STAGE_MISS_BIT_KHR, "VK_SHADER_STAGE_MISS_BIT_KHR" },
                { VK_SHADER_STAGE_INTERSECTION_BIT_KHR, "VK_SHADER_STAGE_INTERSECTION_BIT_KHR" },
                { VK_SHADER_STAGE_CALLABLE_BIT_KHR, "VK_SHADER_STAGE_CALLABLE_BIT_KHR" },
                { VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI, "VK_SHADER_STAGE_SUBPASS_SHADING_BIT_HUAWEI" },
                { VK_SHADER_STAGE_CLUSTER_CULLING_BIT_HUAWEI, "VK_SHADER_STAGE_CLUSTER_CULLING_BIT_HUAWEI" }
            };

            // Multiple flags in one:
            if (vkShaderStageFlags == VK_SHADER_STAGE_ALL_GRAPHICS)
                return "VK_SHADER_STAGE_ALL_GRAPHICS";

            // Multiple flags in one:
            if (vkShaderStageFlags == VK_SHADER_STAGE_ALL)
                return "VK_SHADER_STAGE_ALL";

            std::vector<std::string> flagNames;

            // Iterate through defined flags and check if they are set:
            for (const auto& mapping : flagMappings)
                if (vkShaderStageFlags & mapping.flag)
                    flagNames.emplace_back(mapping.name);

            // Super flag with all known flags:
            VkShaderStageFlags knownFlags = VK_SHADER_STAGE_ALL | VK_SHADER_STAGE_ALL_GRAPHICS;
            for (const auto& mapping : flagMappings)
                knownFlags |= mapping.flag;

            // Check if given flags are outside the set of known flags:
            if (vkShaderStageFlags & ~knownFlags)
                flagNames.emplace_back("UNKNOWN_VK_SHADER_STAGE_FLAG (maybe vulkan backend has been updated)");

            // Join flag names:
            std::string result;
            for (size_t i = 0; i < flagNames.size(); i++)
            {
                if (i > 0) result += " | ";
                result += flagNames[i];
            }

            return result;
        }
    }
}