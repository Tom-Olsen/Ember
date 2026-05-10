#pragma once
#include "commonTextureFormat.h"
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
    // emberCommon::TextureFormat -> VkFormat:
    [[nodiscard]] VkFormat TextureFormatCommonToVulkan(emberCommon::TextureFormat format);
    // VkFormat -> emberCommon::TextureFormat:
    [[nodiscard]] emberCommon::TextureFormat TextureFormatVulkanToCommon(VkFormat format);
}