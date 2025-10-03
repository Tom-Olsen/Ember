#pragma once
#include "commonTextureFormat.h"
#include "vulkanFormat.h"



namespace vulkanRendererBackend
{
    // Format -> emberCommon::DepthStencilFormat:
    const emberCommon::DepthStencilFormat& VulkanFormatToDepthStencilFormat(vulkanRendererBackend::Format format);
    // emberCommon::DepthStencilFormat -> Format:
    const vulkanRendererBackend::Format DepthStencilFormatToVulkanFormat(const emberCommon::DepthStencilFormat& format);



    // Format -> emberCommon::TextureFormat:
    const emberCommon::TextureFormat& VulkanFormatToTextureFormat(vulkanRendererBackend::Format format);
    // emberCommon::TextureFormat -> Format:
    const vulkanRendererBackend::Format TextureFormatToVulkanFormat(const emberCommon::TextureFormat& format);
}