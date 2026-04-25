#pragma once
#include "commonTextureFormat.h"
#include "vulkanFormat.h"



namespace vulkanRendererBackend
{
    // emberCommon::TextureFormat -> vulkanRendererBackend::Format:
    [[nodiscard]] vulkanRendererBackend::Format TextureFormatCommonToVulkan(emberCommon::TextureFormat format);
    // vulkanRendererBackend::Format -> emberCommon::TextureFormat:
    [[nodiscard]] emberCommon::TextureFormat TextureFormatVulkanToCommon(vulkanRendererBackend::Format format);
}