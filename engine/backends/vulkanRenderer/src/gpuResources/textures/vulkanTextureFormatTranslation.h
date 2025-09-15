#pragma once
#include "textureFormat.h"
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
    const emberEngine::DepthStencilFormat& TranslateDepthStencilFormat(VkFormat vkFormat)
    {
        switch (vkFormat)
        {
            // Depth/Stencil:
            case VK_FORMAT_S8_UINT:             return emberEngine::DepthStencilFormats::d00_s8;
            case VK_FORMAT_D16_UNORM:           return emberEngine::DepthStencilFormats::d16_s0;
            case VK_FORMAT_D16_UNORM_S8_UINT:   return emberEngine::DepthStencilFormats::d16_s8;
            case VK_FORMAT_D24_UNORM_S8_UINT:   return emberEngine::DepthStencilFormats::d24_s8;
            case VK_FORMAT_D32_SFLOAT:          return emberEngine::DepthStencilFormats::d32_s0;
            case VK_FORMAT_D32_SFLOAT_S8_UINT:  return emberEngine::DepthStencilFormats::d32_s8;

            // Fallback:
            default:                            return emberEngine::DepthStencilFormats::unknown;
        }
    }


    const emberEngine::TextureFormat& TranslateTextureFormat(VkFormat vkFormat)
    {
        switch (vkFormat)
        {
            // Single Channel:
            case VK_FORMAT_R8_SRGB:                 return emberEngine::TextureFormats::r08_srgb;
            case VK_FORMAT_R8_UINT:                 return emberEngine::TextureFormats::r08_uint;
            case VK_FORMAT_R8_SINT:                 return emberEngine::TextureFormats::r08_sint;
			case VK_FORMAT_R8_USCALED:              return emberEngine::TextureFormats::r08_uscaled;
			case VK_FORMAT_R8_SSCALED:              return emberEngine::TextureFormats::r08_sscaled;
            case VK_FORMAT_R8_UNORM:                return emberEngine::TextureFormats::r08_unorm;
            case VK_FORMAT_R8_SNORM:                return emberEngine::TextureFormats::r08_snorm;

            case VK_FORMAT_R16_UINT:                return emberEngine::TextureFormats::r16_uint;
            case VK_FORMAT_R16_SINT:                return emberEngine::TextureFormats::r16_sint;
            case VK_FORMAT_R16_USCALED:             return emberEngine::TextureFormats::r16_uscaled;
            case VK_FORMAT_R16_SSCALED:             return emberEngine::TextureFormats::r16_sscaled;
            case VK_FORMAT_R16_UNORM:               return emberEngine::TextureFormats::r16_unorm;
            case VK_FORMAT_R16_SNORM:               return emberEngine::TextureFormats::r16_snorm;
            case VK_FORMAT_R16_SFLOAT:              return emberEngine::TextureFormats::r16_sfloat;

            case VK_FORMAT_R32_UINT:                return emberEngine::TextureFormats::r32_uint;
            case VK_FORMAT_R32_SINT:                return emberEngine::TextureFormats::r32_sint;
            case VK_FORMAT_R32_SFLOAT:              return emberEngine::TextureFormats::r32_sfloat;

            case VK_FORMAT_R64_UINT:                return emberEngine::TextureFormats::r64_uint;
            case VK_FORMAT_R64_SINT:                return emberEngine::TextureFormats::r64_sint;
            case VK_FORMAT_R64_SFLOAT:              return emberEngine::TextureFormats::r64_sfloat;

            // Dual Channel:
            case VK_FORMAT_R8G8_SRGB:               return emberEngine::TextureFormats::rg08_srgb;
            case VK_FORMAT_R8G8_UINT:               return emberEngine::TextureFormats::rg08_uint;
            case VK_FORMAT_R8G8_SINT:               return emberEngine::TextureFormats::rg08_sint;
            case VK_FORMAT_R8G8_USCALED:            return emberEngine::TextureFormats::rg08_uscaled;
            case VK_FORMAT_R8G8_SSCALED:            return emberEngine::TextureFormats::rg08_sscaled;
            case VK_FORMAT_R8G8_UNORM:              return emberEngine::TextureFormats::rg08_unorm;
            case VK_FORMAT_R8G8_SNORM:              return emberEngine::TextureFormats::rg08_snorm;

            case VK_FORMAT_R16G16_UINT:             return emberEngine::TextureFormats::rg16_uint;
            case VK_FORMAT_R16G16_SINT:             return emberEngine::TextureFormats::rg16_sint;
            case VK_FORMAT_R16G16_USCALED:          return emberEngine::TextureFormats::rg16_uscaled;
            case VK_FORMAT_R16G16_SSCALED:          return emberEngine::TextureFormats::rg16_sscaled;
            case VK_FORMAT_R16G16_UNORM:            return emberEngine::TextureFormats::rg16_unorm;
            case VK_FORMAT_R16G16_SNORM:            return emberEngine::TextureFormats::rg16_snorm;
            case VK_FORMAT_R16G16_SFLOAT:           return emberEngine::TextureFormats::rg16_sfloat;

            case VK_FORMAT_R32G32_UINT:             return emberEngine::TextureFormats::rg32_uint;
            case VK_FORMAT_R32G32_SINT:             return emberEngine::TextureFormats::rg32_sint;
            case VK_FORMAT_R32G32_SFLOAT:           return emberEngine::TextureFormats::rg32_sfloat;

            case VK_FORMAT_R64G64_UINT:             return emberEngine::TextureFormats::rg64_uint;
            case VK_FORMAT_R64G64_SINT:             return emberEngine::TextureFormats::rg64_sint;
            case VK_FORMAT_R64G64_SFLOAT:           return emberEngine::TextureFormats::rg64_sfloat;

            // Triple Channel:
            case VK_FORMAT_R8G8B8_SRGB:             return emberEngine::TextureFormats::rgb08_srgb;
            case VK_FORMAT_R8G8B8_UINT:             return emberEngine::TextureFormats::rgb08_uint;
            case VK_FORMAT_R8G8B8_SINT:             return emberEngine::TextureFormats::rgb08_sint;
            case VK_FORMAT_R8G8B8_USCALED:          return emberEngine::TextureFormats::rgb08_uscaled;
            case VK_FORMAT_R8G8B8_SSCALED:          return emberEngine::TextureFormats::rgb08_sscaled;
            case VK_FORMAT_R8G8B8_UNORM:            return emberEngine::TextureFormats::rgb08_unorm;
            case VK_FORMAT_R8G8B8_SNORM:            return emberEngine::TextureFormats::rgb08_snorm;

            case VK_FORMAT_R16G16B16_UINT:          return emberEngine::TextureFormats::rgb16_uint;
            case VK_FORMAT_R16G16B16_SINT:          return emberEngine::TextureFormats::rgb16_sint;
            case VK_FORMAT_R16G16B16_USCALED:       return emberEngine::TextureFormats::rgb16_uscaled;
            case VK_FORMAT_R16G16B16_SSCALED:       return emberEngine::TextureFormats::rgb16_sscaled;
            case VK_FORMAT_R16G16B16_UNORM:         return emberEngine::TextureFormats::rgb16_unorm;
            case VK_FORMAT_R16G16B16_SNORM:         return emberEngine::TextureFormats::rgb16_snorm;
            case VK_FORMAT_R16G16B16_SFLOAT:        return emberEngine::TextureFormats::rgb16_sfloat;

            case VK_FORMAT_R32G32B32_UINT:          return emberEngine::TextureFormats::rgb32_uint;
            case VK_FORMAT_R32G32B32_SINT:          return emberEngine::TextureFormats::rgb32_sint;
            case VK_FORMAT_R32G32B32_SFLOAT:        return emberEngine::TextureFormats::rgb32_sfloat;

            case VK_FORMAT_R64G64B64_UINT:          return emberEngine::TextureFormats::rgb64_uint;
            case VK_FORMAT_R64G64B64_SINT:          return emberEngine::TextureFormats::rgb64_sint;
            case VK_FORMAT_R64G64B64_SFLOAT:        return emberEngine::TextureFormats::rgb64_sfloat;

            // Quadrupal Channel:
            case VK_FORMAT_R8G8B8A8_SRGB:           return emberEngine::TextureFormats::rgba08_srgb;
            case VK_FORMAT_R8G8B8A8_UINT:           return emberEngine::TextureFormats::rgba08_uint;
            case VK_FORMAT_R8G8B8A8_SINT:           return emberEngine::TextureFormats::rgba08_sint;
            case VK_FORMAT_R8G8B8A8_USCALED:        return emberEngine::TextureFormats::rgba08_uscaled;
            case VK_FORMAT_R8G8B8A8_SSCALED:        return emberEngine::TextureFormats::rgba08_sscaled;
            case VK_FORMAT_R8G8B8A8_UNORM:          return emberEngine::TextureFormats::rgba08_unorm;
            case VK_FORMAT_R8G8B8A8_SNORM:          return emberEngine::TextureFormats::rgba08_snorm;

            case VK_FORMAT_R16G16B16A16_UINT:       return emberEngine::TextureFormats::rgba16_uint;
            case VK_FORMAT_R16G16B16A16_SINT:       return emberEngine::TextureFormats::rgba16_sint;
            case VK_FORMAT_R16G16B16A16_USCALED:    return emberEngine::TextureFormats::rgba16_uscaled;
            case VK_FORMAT_R16G16B16A16_SSCALED:    return emberEngine::TextureFormats::rgba16_sscaled;
            case VK_FORMAT_R16G16B16A16_UNORM:      return emberEngine::TextureFormats::rgba16_unorm;
            case VK_FORMAT_R16G16B16A16_SNORM:      return emberEngine::TextureFormats::rgba16_snorm;
            case VK_FORMAT_R16G16B16A16_SFLOAT:     return emberEngine::TextureFormats::rgba16_sfloat;

            case VK_FORMAT_R32G32B32A32_UINT:       return emberEngine::TextureFormats::rgba32_uint;
            case VK_FORMAT_R32G32B32A32_SINT:       return emberEngine::TextureFormats::rgba32_sint;
            case VK_FORMAT_R32G32B32A32_SFLOAT:     return emberEngine::TextureFormats::rgba32_sfloat;

            case VK_FORMAT_R64G64B64A64_UINT:       return emberEngine::TextureFormats::rgba64_uint;
            case VK_FORMAT_R64G64B64A64_SINT:       return emberEngine::TextureFormats::rgba64_sint;
            case VK_FORMAT_R64G64B64A64_SFLOAT:     return emberEngine::TextureFormats::rgba64_sfloat;

            // Fallback
            default: return emberEngine::TextureFormats::unknown;
        }
    }
}