#include "vulkanConvertTextureFormat.h"
#include <unordered_map>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
    // emberCommon::DepthStencilFormat -> vulkanRendererBackend::Format:
    vulkanRendererBackend::Format DepthStencilFormatCommonToVulkan(emberCommon::DepthStencilFormat format)
    {
        switch (format.flag)
        {
            case emberCommon::FormatFlag::d00_s8: return vulkanRendererBackend::Formats::d16_unorm;
            case emberCommon::FormatFlag::d16_s0: return vulkanRendererBackend::Formats::d32_sfloat;
            case emberCommon::FormatFlag::d16_s8: return vulkanRendererBackend::Formats::s8_uint;
            case emberCommon::FormatFlag::d24_s8: return vulkanRendererBackend::Formats::d16_unorm_s8_uint;
            case emberCommon::FormatFlag::d32_s0: return vulkanRendererBackend::Formats::d24_unorm_s8_uint;
            case emberCommon::FormatFlag::d32_s8: return vulkanRendererBackend::Formats::d32_sfloat_s8_uint;
            default: return vulkanRendererBackend::Formats::undefined;
        }
    }

    // vulkanRendererBackend::Format -> emberCommon::DepthStencilFormat:
    emberCommon::DepthStencilFormat DepthStencilFormatVulkanToCommon(vulkanRendererBackend::Format format)
    {
        switch (format)
        {
            case VK_FORMAT_D16_UNORM: return emberCommon::DepthStencilFormat{ emberCommon::FormatFlag::d00_s8 };
            case VK_FORMAT_D32_SFLOAT: return emberCommon::DepthStencilFormat{ emberCommon::FormatFlag::d16_s0 };
            case VK_FORMAT_S8_UINT: return emberCommon::DepthStencilFormat{ emberCommon::FormatFlag::d16_s8 };
            case VK_FORMAT_D16_UNORM_S8_UINT: return emberCommon::DepthStencilFormat{ emberCommon::FormatFlag::d24_s8 };
            case VK_FORMAT_D24_UNORM_S8_UINT: return emberCommon::DepthStencilFormat{ emberCommon::FormatFlag::d32_s0 };
            case VK_FORMAT_D32_SFLOAT_S8_UINT: return emberCommon::DepthStencilFormat{ emberCommon::FormatFlag::d32_s8 };
            default: return emberCommon::DepthStencilFormat{ emberCommon::FormatFlag::undefined };
        }
    }



    // emberCommon::TextureFormat -> vulkanRendererBackend::Format:
    vulkanRendererBackend::Format TextureFormatCommonToVulkan(emberCommon::TextureFormat format)
    {
        switch (format.flag)
        {
            case emberCommon::FormatFlag::r08_srgb: return vulkanRendererBackend::Formats::r8_srgb;
            case emberCommon::FormatFlag::r08_uint: return vulkanRendererBackend::Formats::r8_uint;
            case emberCommon::FormatFlag::r08_sint: return vulkanRendererBackend::Formats::r8_sint;
            case emberCommon::FormatFlag::r08_uscaled: return vulkanRendererBackend::Formats::r8_uscaled;
            case emberCommon::FormatFlag::r08_sscaled: return vulkanRendererBackend::Formats::r8_sscaled;
            case emberCommon::FormatFlag::r08_unorm: return vulkanRendererBackend::Formats::r8_unorm;
            case emberCommon::FormatFlag::r08_snorm: return vulkanRendererBackend::Formats::r8_snorm;
            case emberCommon::FormatFlag::r16_uint: return vulkanRendererBackend::Formats::r16_uint;
            case emberCommon::FormatFlag::r16_sint: return vulkanRendererBackend::Formats::r16_sint;
            case emberCommon::FormatFlag::r16_uscaled: return vulkanRendererBackend::Formats::r16_uscaled;
            case emberCommon::FormatFlag::r16_sscaled: return vulkanRendererBackend::Formats::r16_sscaled;
            case emberCommon::FormatFlag::r16_unorm: return vulkanRendererBackend::Formats::r16_unorm;
            case emberCommon::FormatFlag::r16_snorm: return vulkanRendererBackend::Formats::r16_snorm;
            case emberCommon::FormatFlag::r16_sfloat: return vulkanRendererBackend::Formats::r16_sfloat;
            case emberCommon::FormatFlag::r32_uint: return vulkanRendererBackend::Formats::r32_uint;
            case emberCommon::FormatFlag::r32_sint: return vulkanRendererBackend::Formats::r32_sint;
            case emberCommon::FormatFlag::r32_sfloat: return vulkanRendererBackend::Formats::r32_sfloat;
            case emberCommon::FormatFlag::r64_uint: return vulkanRendererBackend::Formats::r64_uint;
            case emberCommon::FormatFlag::r64_sint: return vulkanRendererBackend::Formats::r64_sint;
            case emberCommon::FormatFlag::r64_sfloat: return vulkanRendererBackend::Formats::r64_sfloat;
            case emberCommon::FormatFlag::rg08_srgb: return vulkanRendererBackend::Formats::r8g8_srgb;
            case emberCommon::FormatFlag::rg08_uint: return vulkanRendererBackend::Formats::r8g8_uint;
            case emberCommon::FormatFlag::rg08_sint: return vulkanRendererBackend::Formats::r8g8_sint;
            case emberCommon::FormatFlag::rg08_uscaled: return vulkanRendererBackend::Formats::r8g8_uscaled;
            case emberCommon::FormatFlag::rg08_sscaled: return vulkanRendererBackend::Formats::r8g8_sscaled;
            case emberCommon::FormatFlag::rg08_unorm: return vulkanRendererBackend::Formats::r8g8_unorm;
            case emberCommon::FormatFlag::rg08_snorm: return vulkanRendererBackend::Formats::r8g8_snorm;
            case emberCommon::FormatFlag::rg16_uint: return vulkanRendererBackend::Formats::r16g16_uint;
            case emberCommon::FormatFlag::rg16_sint: return vulkanRendererBackend::Formats::r16g16_sint;
            case emberCommon::FormatFlag::rg16_uscaled: return vulkanRendererBackend::Formats::r16g16_uscaled;
            case emberCommon::FormatFlag::rg16_sscaled: return vulkanRendererBackend::Formats::r16g16_sscaled;
            case emberCommon::FormatFlag::rg16_unorm: return vulkanRendererBackend::Formats::r16g16_unorm;
            case emberCommon::FormatFlag::rg16_snorm: return vulkanRendererBackend::Formats::r16g16_snorm;
            case emberCommon::FormatFlag::rg16_sfloat: return vulkanRendererBackend::Formats::r16g16_sfloat;
            case emberCommon::FormatFlag::rg32_uint: return vulkanRendererBackend::Formats::r32g32_uint;
            case emberCommon::FormatFlag::rg32_sint: return vulkanRendererBackend::Formats::r32g32_sint;
            case emberCommon::FormatFlag::rg32_sfloat: return vulkanRendererBackend::Formats::r32g32_sfloat;
            case emberCommon::FormatFlag::rg64_uint: return vulkanRendererBackend::Formats::r64g64_uint;
            case emberCommon::FormatFlag::rg64_sint: return vulkanRendererBackend::Formats::r64g64_sint;
            case emberCommon::FormatFlag::rg64_sfloat: return vulkanRendererBackend::Formats::r64g64_sfloat;
            case emberCommon::FormatFlag::rgb08_srgb: return vulkanRendererBackend::Formats::r8g8b8_srgb;
            case emberCommon::FormatFlag::rgb08_uint: return vulkanRendererBackend::Formats::r8g8b8_uint;
            case emberCommon::FormatFlag::rgb08_sint: return vulkanRendererBackend::Formats::r8g8b8_sint;
            case emberCommon::FormatFlag::rgb08_uscaled: return vulkanRendererBackend::Formats::r8g8b8_uscaled;
            case emberCommon::FormatFlag::rgb08_sscaled: return vulkanRendererBackend::Formats::r8g8b8_sscaled;
            case emberCommon::FormatFlag::rgb08_unorm: return vulkanRendererBackend::Formats::r8g8b8_unorm;
            case emberCommon::FormatFlag::rgb08_snorm: return vulkanRendererBackend::Formats::r8g8b8_snorm;
            case emberCommon::FormatFlag::rgb16_uint: return vulkanRendererBackend::Formats::r16g16b16_uint;
            case emberCommon::FormatFlag::rgb16_sint: return vulkanRendererBackend::Formats::r16g16b16_sint;
            case emberCommon::FormatFlag::rgb16_uscaled: return vulkanRendererBackend::Formats::r16g16b16_uscaled;
            case emberCommon::FormatFlag::rgb16_sscaled: return vulkanRendererBackend::Formats::r16g16b16_sscaled;
            case emberCommon::FormatFlag::rgb16_unorm: return vulkanRendererBackend::Formats::r16g16b16_unorm;
            case emberCommon::FormatFlag::rgb16_snorm: return vulkanRendererBackend::Formats::r16g16b16_snorm;
            case emberCommon::FormatFlag::rgb16_sfloat: return vulkanRendererBackend::Formats::r16g16b16_sfloat;
            case emberCommon::FormatFlag::rgb32_uint: return vulkanRendererBackend::Formats::r32g32b32_uint;
            case emberCommon::FormatFlag::rgb32_sint: return vulkanRendererBackend::Formats::r32g32b32_sint;
            case emberCommon::FormatFlag::rgb32_sfloat: return vulkanRendererBackend::Formats::r32g32b32_sfloat;
            case emberCommon::FormatFlag::rgb64_uint: return vulkanRendererBackend::Formats::r64g64b64_uint;
            case emberCommon::FormatFlag::rgb64_sint: return vulkanRendererBackend::Formats::r64g64b64_sint;
            case emberCommon::FormatFlag::rgb64_sfloat: return vulkanRendererBackend::Formats::r64g64b64_sfloat;
            case emberCommon::FormatFlag::rgba08_srgb: return vulkanRendererBackend::Formats::r8g8b8a8_srgb;
            case emberCommon::FormatFlag::rgba08_uint: return vulkanRendererBackend::Formats::r8g8b8a8_uint;
            case emberCommon::FormatFlag::rgba08_sint: return vulkanRendererBackend::Formats::r8g8b8a8_sint;
            case emberCommon::FormatFlag::rgba08_uscaled: return vulkanRendererBackend::Formats::r8g8b8a8_uscaled;
            case emberCommon::FormatFlag::rgba08_sscaled: return vulkanRendererBackend::Formats::r8g8b8a8_sscaled;
            case emberCommon::FormatFlag::rgba08_unorm: return vulkanRendererBackend::Formats::r8g8b8a8_unorm;
            case emberCommon::FormatFlag::rgba08_snorm: return vulkanRendererBackend::Formats::r8g8b8a8_snorm;
            case emberCommon::FormatFlag::rgba16_uint: return vulkanRendererBackend::Formats::r16g16b16a16_uint;
            case emberCommon::FormatFlag::rgba16_sint: return vulkanRendererBackend::Formats::r16g16b16a16_sint;
            case emberCommon::FormatFlag::rgba16_uscaled: return vulkanRendererBackend::Formats::r16g16b16a16_uscaled;
            case emberCommon::FormatFlag::rgba16_sscaled: return vulkanRendererBackend::Formats::r16g16b16a16_sscaled;
            case emberCommon::FormatFlag::rgba16_unorm: return vulkanRendererBackend::Formats::r16g16b16a16_unorm;
            case emberCommon::FormatFlag::rgba16_snorm: return vulkanRendererBackend::Formats::r16g16b16a16_snorm;
            case emberCommon::FormatFlag::rgba16_sfloat: return vulkanRendererBackend::Formats::r16g16b16a16_sfloat;
            case emberCommon::FormatFlag::rgba32_uint: return vulkanRendererBackend::Formats::r32g32b32a32_uint;
            case emberCommon::FormatFlag::rgba32_sint: return vulkanRendererBackend::Formats::r32g32b32a32_sint;
            case emberCommon::FormatFlag::rgba32_sfloat: return vulkanRendererBackend::Formats::r32g32b32a32_sfloat;
            case emberCommon::FormatFlag::rgba64_uint: return vulkanRendererBackend::Formats::r64g64b64a64_uint;
            case emberCommon::FormatFlag::rgba64_sint: return vulkanRendererBackend::Formats::r64g64b64a64_sint;
            case emberCommon::FormatFlag::rgba64_sfloat: return vulkanRendererBackend::Formats::r64g64b64a64_sfloat;
            default: return vulkanRendererBackend::Formats::undefined;
        }
    }

    // vulkanRendererBackend::Format -> emberCommon::TextureFormat:
    emberCommon::TextureFormat TextureFormatVulkanToCommon(vulkanRendererBackend::Format format)
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