#pragma once
#include "textureFormat.h"
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
    emberEngine::TextureFormat TranslateTextureFormat(VkFormat vkFormat)
    {
        switch (vkFormat)
        {
            // Color:
            case VK_FORMAT_R8G8B8A8_UNORM:      return emberEngine::TextureFormat::rgba08_UNorm;
            case VK_FORMAT_R8G8B8A8_SRGB:       return emberEngine::TextureFormat::rgba08_SRGB;
            case VK_FORMAT_R16G16B16A16_SFLOAT: return emberEngine::TextureFormat::rgba16_Float;
            case VK_FORMAT_R32G32B32A32_SFLOAT: return emberEngine::TextureFormat::rgba32_Float;

            // Depth/Stencil:
            case VK_FORMAT_D24_UNORM_S8_UINT:   return emberEngine::TextureFormat::d24_UNorm_S8_UInt;
            case VK_FORMAT_D32_SFLOAT:          return emberEngine::TextureFormat::d32_Float;
            case VK_FORMAT_D32_SFLOAT_S8_UINT:  return emberEngine::TextureFormat::d32_Float_S8_UInt;

                // Single channel:
            case VK_FORMAT_R8_UNORM:            return emberEngine::TextureFormat::r08_UNorm;
            case VK_FORMAT_R8_UINT:             return emberEngine::TextureFormat::r08_UInt;
            case VK_FORMAT_R16_SFLOAT:          return emberEngine::TextureFormat::r16_Float;
            case VK_FORMAT_R32_SFLOAT:          return emberEngine::TextureFormat::r32_Float;

                // Fallback:
            default: return emberEngine::TextureFormat::unknown;
        }
    }
}