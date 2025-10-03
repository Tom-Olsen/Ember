#pragma once
#include "commonTextureFormat.h"
#include "vulkanFormat.h"
#include "vulkanRendererExport.h"



namespace vulkanRendererBackend
{
    // emberCommon::DepthStencilFormat -> vulkanRendererBackend::Format:
    [[nodiscard]] vulkanRendererBackend::Format VULKAN_RENDERER_API DepthStencilFormatCommonToVulkan(emberCommon::DepthStencilFormat format);
    // vulkanRendererBackend::Format -> emberCommon::DepthStencilFormat:
    [[nodiscard]] emberCommon::DepthStencilFormat DepthStencilFormatVulkanToCommon(vulkanRendererBackend::Format format);



    // emberCommon::TextureFormat -> vulkanRendererBackend::Format:
    [[nodiscard]] vulkanRendererBackend::Format VULKAN_RENDERER_API TextureFormatCommonToVulkan(emberCommon::TextureFormat format);
    // vulkanRendererBackend::Format -> emberCommon::TextureFormat:
    [[nodiscard]] emberCommon::TextureFormat TextureFormatVulkanToCommon(vulkanRendererBackend::Format format);
}