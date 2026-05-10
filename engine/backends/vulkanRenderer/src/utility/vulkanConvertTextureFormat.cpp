#include "vulkanConvertTextureFormat.h"
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
    // emberCommon::TextureFormat -> VkFormat:
    VkFormat TextureFormatCommonToVulkan(emberCommon::TextureFormat format)
    {
        switch (format.flag)
        {
            case emberCommon::FormatFlag::r08_srgb: return VK_FORMAT_R8_SRGB;
            case emberCommon::FormatFlag::r08_uint: return VK_FORMAT_R8_UINT;
            case emberCommon::FormatFlag::r08_sint: return VK_FORMAT_R8_SINT;
            case emberCommon::FormatFlag::r08_uscaled: return VK_FORMAT_R8_USCALED;
            case emberCommon::FormatFlag::r08_sscaled: return VK_FORMAT_R8_SSCALED;
            case emberCommon::FormatFlag::r08_unorm: return VK_FORMAT_R8_UNORM;
            case emberCommon::FormatFlag::r08_snorm: return VK_FORMAT_R8_SNORM;
            case emberCommon::FormatFlag::r16_uint: return VK_FORMAT_R16_UINT;
            case emberCommon::FormatFlag::r16_sint: return VK_FORMAT_R16_SINT;
            case emberCommon::FormatFlag::r16_uscaled: return VK_FORMAT_R16_USCALED;
            case emberCommon::FormatFlag::r16_sscaled: return VK_FORMAT_R16_SSCALED;
            case emberCommon::FormatFlag::r16_unorm: return VK_FORMAT_R16_UNORM;
            case emberCommon::FormatFlag::r16_snorm: return VK_FORMAT_R16_SNORM;
            case emberCommon::FormatFlag::r16_sfloat: return VK_FORMAT_R16_SFLOAT;
            case emberCommon::FormatFlag::r32_uint: return VK_FORMAT_R32_UINT;
            case emberCommon::FormatFlag::r32_sint: return VK_FORMAT_R32_SINT;
            case emberCommon::FormatFlag::r32_sfloat: return VK_FORMAT_R32_SFLOAT;
            case emberCommon::FormatFlag::r64_uint: return VK_FORMAT_R64_UINT;
            case emberCommon::FormatFlag::r64_sint: return VK_FORMAT_R64_SINT;
            case emberCommon::FormatFlag::r64_sfloat: return VK_FORMAT_R64_SFLOAT;
            case emberCommon::FormatFlag::rg08_srgb: return VK_FORMAT_R8G8_SRGB;
            case emberCommon::FormatFlag::rg08_uint: return VK_FORMAT_R8G8_UINT;
            case emberCommon::FormatFlag::rg08_sint: return VK_FORMAT_R8G8_SINT;
            case emberCommon::FormatFlag::rg08_uscaled: return VK_FORMAT_R8G8_USCALED;
            case emberCommon::FormatFlag::rg08_sscaled: return VK_FORMAT_R8G8_SSCALED;
            case emberCommon::FormatFlag::rg08_unorm: return VK_FORMAT_R8G8_UNORM;
            case emberCommon::FormatFlag::rg08_snorm: return VK_FORMAT_R8G8_SNORM;
            case emberCommon::FormatFlag::rg16_uint: return VK_FORMAT_R16G16_UINT;
            case emberCommon::FormatFlag::rg16_sint: return VK_FORMAT_R16G16_SINT;
            case emberCommon::FormatFlag::rg16_uscaled: return VK_FORMAT_R16G16_USCALED;
            case emberCommon::FormatFlag::rg16_sscaled: return VK_FORMAT_R16G16_SSCALED;
            case emberCommon::FormatFlag::rg16_unorm: return VK_FORMAT_R16G16_UNORM;
            case emberCommon::FormatFlag::rg16_snorm: return VK_FORMAT_R16G16_SNORM;
            case emberCommon::FormatFlag::rg16_sfloat: return VK_FORMAT_R16G16_SFLOAT;
            case emberCommon::FormatFlag::rg32_uint: return VK_FORMAT_R32G32_UINT;
            case emberCommon::FormatFlag::rg32_sint: return VK_FORMAT_R32G32_SINT;
            case emberCommon::FormatFlag::rg32_sfloat: return VK_FORMAT_R32G32_SFLOAT;
            case emberCommon::FormatFlag::rg64_uint: return VK_FORMAT_R64G64_UINT;
            case emberCommon::FormatFlag::rg64_sint: return VK_FORMAT_R64G64_SINT;
            case emberCommon::FormatFlag::rg64_sfloat: return VK_FORMAT_R64G64_SFLOAT;
            case emberCommon::FormatFlag::rgb08_srgb: return VK_FORMAT_R8G8B8_SRGB;
            case emberCommon::FormatFlag::rgb08_uint: return VK_FORMAT_R8G8B8_UINT;
            case emberCommon::FormatFlag::rgb08_sint: return VK_FORMAT_R8G8B8_SINT;
            case emberCommon::FormatFlag::rgb08_uscaled: return VK_FORMAT_R8G8B8_USCALED;
            case emberCommon::FormatFlag::rgb08_sscaled: return VK_FORMAT_R8G8B8_SSCALED;
            case emberCommon::FormatFlag::rgb08_unorm: return VK_FORMAT_R8G8B8_UNORM;
            case emberCommon::FormatFlag::rgb08_snorm: return VK_FORMAT_R8G8B8_SNORM;
            case emberCommon::FormatFlag::rgb16_uint: return VK_FORMAT_R16G16B16_UINT;
            case emberCommon::FormatFlag::rgb16_sint: return VK_FORMAT_R16G16B16_SINT;
            case emberCommon::FormatFlag::rgb16_uscaled: return VK_FORMAT_R16G16B16_USCALED;
            case emberCommon::FormatFlag::rgb16_sscaled: return VK_FORMAT_R16G16B16_SSCALED;
            case emberCommon::FormatFlag::rgb16_unorm: return VK_FORMAT_R16G16B16_UNORM;
            case emberCommon::FormatFlag::rgb16_snorm: return VK_FORMAT_R16G16B16_SNORM;
            case emberCommon::FormatFlag::rgb16_sfloat: return VK_FORMAT_R16G16B16_SFLOAT;
            case emberCommon::FormatFlag::rgb32_uint: return VK_FORMAT_R32G32B32_UINT;
            case emberCommon::FormatFlag::rgb32_sint: return VK_FORMAT_R32G32B32_SINT;
            case emberCommon::FormatFlag::rgb32_sfloat: return VK_FORMAT_R32G32B32_SFLOAT;
            case emberCommon::FormatFlag::rgb64_uint: return VK_FORMAT_R64G64B64_UINT;
            case emberCommon::FormatFlag::rgb64_sint: return VK_FORMAT_R64G64B64_SINT;
            case emberCommon::FormatFlag::rgb64_sfloat: return VK_FORMAT_R64G64B64_SFLOAT;
            case emberCommon::FormatFlag::rgba08_srgb: return VK_FORMAT_R8G8B8A8_SRGB;
            case emberCommon::FormatFlag::rgba08_uint: return VK_FORMAT_R8G8B8A8_UINT;
            case emberCommon::FormatFlag::rgba08_sint: return VK_FORMAT_R8G8B8A8_SINT;
            case emberCommon::FormatFlag::rgba08_uscaled: return VK_FORMAT_R8G8B8A8_USCALED;
            case emberCommon::FormatFlag::rgba08_sscaled: return VK_FORMAT_R8G8B8A8_SSCALED;
            case emberCommon::FormatFlag::rgba08_unorm: return VK_FORMAT_R8G8B8A8_UNORM;
            case emberCommon::FormatFlag::rgba08_snorm: return VK_FORMAT_R8G8B8A8_SNORM;
            case emberCommon::FormatFlag::rgba16_uint: return VK_FORMAT_R16G16B16A16_UINT;
            case emberCommon::FormatFlag::rgba16_sint: return VK_FORMAT_R16G16B16A16_SINT;
            case emberCommon::FormatFlag::rgba16_uscaled: return VK_FORMAT_R16G16B16A16_USCALED;
            case emberCommon::FormatFlag::rgba16_sscaled: return VK_FORMAT_R16G16B16A16_SSCALED;
            case emberCommon::FormatFlag::rgba16_unorm: return VK_FORMAT_R16G16B16A16_UNORM;
            case emberCommon::FormatFlag::rgba16_snorm: return VK_FORMAT_R16G16B16A16_SNORM;
            case emberCommon::FormatFlag::rgba16_sfloat: return VK_FORMAT_R16G16B16A16_SFLOAT;
            case emberCommon::FormatFlag::rgba32_uint: return VK_FORMAT_R32G32B32A32_UINT;
            case emberCommon::FormatFlag::rgba32_sint: return VK_FORMAT_R32G32B32A32_SINT;
            case emberCommon::FormatFlag::rgba32_sfloat: return VK_FORMAT_R32G32B32A32_SFLOAT;
            case emberCommon::FormatFlag::rgba64_uint: return VK_FORMAT_R64G64B64A64_UINT;
            case emberCommon::FormatFlag::rgba64_sint: return VK_FORMAT_R64G64B64A64_SINT;
            case emberCommon::FormatFlag::rgba64_sfloat: return VK_FORMAT_R64G64B64A64_SFLOAT;
            default: return VK_FORMAT_UNDEFINED;
        }
    }

    // VkFormat -> emberCommon::TextureFormat:
    emberCommon::TextureFormat TextureFormatVulkanToCommon(VkFormat format)
    {
        switch (format)
        {
            case VK_FORMAT_R8_SRGB: return emberCommon::TextureFormat{ emberCommon::FormatFlag::r08_srgb, 1, 1 };
            case VK_FORMAT_R8_UINT: return emberCommon::TextureFormat{ emberCommon::FormatFlag::r08_uint, 1, 1 };
            case VK_FORMAT_R8_SINT: return emberCommon::TextureFormat{ emberCommon::FormatFlag::r08_sint, 1, 1 };
            case VK_FORMAT_R8_USCALED: return emberCommon::TextureFormat{ emberCommon::FormatFlag::r08_uscaled, 1, 1 };
            case VK_FORMAT_R8_SSCALED: return emberCommon::TextureFormat{ emberCommon::FormatFlag::r08_sscaled, 1, 1 };
            case VK_FORMAT_R8_UNORM: return emberCommon::TextureFormat{ emberCommon::FormatFlag::r08_unorm, 1, 1 };
            case VK_FORMAT_R8_SNORM: return emberCommon::TextureFormat{ emberCommon::FormatFlag::r08_snorm, 1, 1 };
            case VK_FORMAT_R16_UINT: return emberCommon::TextureFormat{ emberCommon::FormatFlag::r16_uint, 1, 2 };
            case VK_FORMAT_R16_SINT: return emberCommon::TextureFormat{ emberCommon::FormatFlag::r16_sint, 1, 2 };
            case VK_FORMAT_R16_USCALED: return emberCommon::TextureFormat{ emberCommon::FormatFlag::r16_uscaled, 1, 2 };
            case VK_FORMAT_R16_SSCALED: return emberCommon::TextureFormat{ emberCommon::FormatFlag::r16_sscaled, 1, 2 };
            case VK_FORMAT_R16_UNORM: return emberCommon::TextureFormat{ emberCommon::FormatFlag::r16_unorm, 1, 2 };
            case VK_FORMAT_R16_SNORM: return emberCommon::TextureFormat{ emberCommon::FormatFlag::r16_snorm, 1, 2 };
            case VK_FORMAT_R16_SFLOAT: return emberCommon::TextureFormat{ emberCommon::FormatFlag::r16_sfloat, 1, 2 };
            case VK_FORMAT_R32_UINT: return emberCommon::TextureFormat{ emberCommon::FormatFlag::r32_uint, 1, 4 };
            case VK_FORMAT_R32_SINT: return emberCommon::TextureFormat{ emberCommon::FormatFlag::r32_sint, 1, 4 };
            case VK_FORMAT_R32_SFLOAT: return emberCommon::TextureFormat{ emberCommon::FormatFlag::r32_sfloat, 1, 4 };
            case VK_FORMAT_R64_UINT: return emberCommon::TextureFormat{ emberCommon::FormatFlag::r64_uint, 1, 8 };
            case VK_FORMAT_R64_SINT: return emberCommon::TextureFormat{ emberCommon::FormatFlag::r64_sint, 1, 8 };
            case VK_FORMAT_R64_SFLOAT: return emberCommon::TextureFormat{ emberCommon::FormatFlag::r64_sfloat, 1, 8 };
            case VK_FORMAT_R8G8_SRGB: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rg08_srgb, 2, 1 };
            case VK_FORMAT_R8G8_UINT: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rg08_uint, 2, 1 };
            case VK_FORMAT_R8G8_SINT: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rg08_sint, 2, 1 };
            case VK_FORMAT_R8G8_USCALED: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rg08_uscaled, 2, 1 };
            case VK_FORMAT_R8G8_SSCALED: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rg08_sscaled, 2, 1 };
            case VK_FORMAT_R8G8_UNORM: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rg08_unorm, 2, 1 };
            case VK_FORMAT_R8G8_SNORM: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rg08_snorm, 2, 1 };
            case VK_FORMAT_R16G16_UINT: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rg16_uint, 2, 2 };
            case VK_FORMAT_R16G16_SINT: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rg16_sint, 2, 2 };
            case VK_FORMAT_R16G16_USCALED: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rg16_uscaled, 2, 2 };
            case VK_FORMAT_R16G16_SSCALED: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rg16_sscaled, 2, 2 };
            case VK_FORMAT_R16G16_UNORM: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rg16_unorm, 2, 2 };
            case VK_FORMAT_R16G16_SNORM: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rg16_snorm, 2, 2 };
            case VK_FORMAT_R16G16_SFLOAT: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rg16_sfloat, 2, 2 };
            case VK_FORMAT_R32G32_UINT: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rg32_uint, 2, 4 };
            case VK_FORMAT_R32G32_SINT: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rg32_sint, 2, 4 };
            case VK_FORMAT_R32G32_SFLOAT: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rg32_sfloat, 2, 4 };
            case VK_FORMAT_R64G64_UINT: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rg64_uint, 2, 8 };
            case VK_FORMAT_R64G64_SINT: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rg64_sint, 2, 8 };
            case VK_FORMAT_R64G64_SFLOAT: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rg64_sfloat, 2, 8 };
            case VK_FORMAT_R8G8B8_SRGB: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rgb08_srgb, 3, 1 };
            case VK_FORMAT_R8G8B8_UINT: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rgb08_uint, 3, 1 };
            case VK_FORMAT_R8G8B8_SINT: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rgb08_sint, 3, 1 };
            case VK_FORMAT_R8G8B8_USCALED: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rgb08_uscaled, 3, 1 };
            case VK_FORMAT_R8G8B8_SSCALED: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rgb08_sscaled, 3, 1 };
            case VK_FORMAT_R8G8B8_UNORM: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rgb08_unorm, 3, 1 };
            case VK_FORMAT_R8G8B8_SNORM: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rgb08_snorm, 3, 1 };
            case VK_FORMAT_R16G16B16_UINT: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rgb16_uint, 3, 2 };
            case VK_FORMAT_R16G16B16_SINT: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rgb16_sint, 3, 2 };
            case VK_FORMAT_R16G16B16_USCALED: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rgb16_uscaled, 3, 2 };
            case VK_FORMAT_R16G16B16_SSCALED: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rgb16_sscaled, 3, 2 };
            case VK_FORMAT_R16G16B16_UNORM: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rgb16_unorm, 3, 2 };
            case VK_FORMAT_R16G16B16_SNORM: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rgb16_snorm, 3, 2 };
            case VK_FORMAT_R16G16B16_SFLOAT: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rgb16_sfloat, 3, 2 };
            case VK_FORMAT_R32G32B32_UINT: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rgb32_uint, 3, 4 };
            case VK_FORMAT_R32G32B32_SINT: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rgb32_sint, 3, 4 };
            case VK_FORMAT_R32G32B32_SFLOAT: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rgb32_sfloat, 3, 4 };
            case VK_FORMAT_R64G64B64_UINT: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rgb64_uint, 3, 8 };
            case VK_FORMAT_R64G64B64_SINT: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rgb64_sint, 3, 8 };
            case VK_FORMAT_R64G64B64_SFLOAT: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rgb64_sfloat, 3, 8 };
            case VK_FORMAT_R8G8B8A8_SRGB: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rgba08_srgb, 4, 1 };
            case VK_FORMAT_R8G8B8A8_UINT: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rgba08_uint, 4, 1 };
            case VK_FORMAT_R8G8B8A8_SINT: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rgba08_sint, 4, 1 };
            case VK_FORMAT_R8G8B8A8_USCALED: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rgba08_uscaled, 4, 1 };
            case VK_FORMAT_R8G8B8A8_SSCALED: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rgba08_sscaled, 4, 1 };
            case VK_FORMAT_R8G8B8A8_UNORM: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rgba08_unorm, 4, 1 };
            case VK_FORMAT_R8G8B8A8_SNORM: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rgba08_snorm, 4, 1 };
            case VK_FORMAT_R16G16B16A16_UINT: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rgba16_uint, 4, 2 };
            case VK_FORMAT_R16G16B16A16_SINT: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rgba16_sint, 4, 2 };
            case VK_FORMAT_R16G16B16A16_USCALED: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rgba16_uscaled, 4, 2 };
            case VK_FORMAT_R16G16B16A16_SSCALED: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rgba16_sscaled, 4, 2 };
            case VK_FORMAT_R16G16B16A16_UNORM: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rgba16_unorm, 4, 2 };
            case VK_FORMAT_R16G16B16A16_SNORM: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rgba16_snorm, 4, 2 };
            case VK_FORMAT_R16G16B16A16_SFLOAT: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rgba16_sfloat, 4, 2 };
            case VK_FORMAT_R32G32B32A32_UINT: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rgba32_uint, 4, 4 };
            case VK_FORMAT_R32G32B32A32_SINT: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rgba32_sint, 4, 4 };
            case VK_FORMAT_R32G32B32A32_SFLOAT: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rgba32_sfloat, 4, 4 };
            case VK_FORMAT_R64G64B64A64_UINT: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rgba64_uint, 4, 8 };
            case VK_FORMAT_R64G64B64A64_SINT: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rgba64_sint, 4, 8 };
            case VK_FORMAT_R64G64B64A64_SFLOAT: return emberCommon::TextureFormat{ emberCommon::FormatFlag::rgba64_sfloat, 4, 8 };
            default: return emberCommon::TextureFormat{ emberCommon::FormatFlag::undefined };
        }
    }
}