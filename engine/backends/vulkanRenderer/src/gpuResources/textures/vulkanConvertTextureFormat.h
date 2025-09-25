#pragma once
#include "commonTextureFormat.h"
#include "vulkanFormat.h"



namespace vulkanRendererBackend
{
    // Format -> emberEngine::DepthStencilFormat:
    const emberCommon::DepthStencilFormat& VulkanFormatToDepthStencilFormat(Format format);

    // Format -> emberEngine::TextureFormat:
    const emberCommon::TextureFormat& VulkanFormatToTextureFormat(Format vkFormat);

    // emberEngine::TextureFormat -> Format:
    // 1 Channel Formats:
    const Format& TextureFormatToVulkanFormat(emberCommon::TextureFormat1Channel08Bit textureFormat);
    const Format& TextureFormatToVulkanFormat(emberCommon::TextureFormat1Channel16Bit textureFormat);
    const Format& TextureFormatToVulkanFormat(emberCommon::TextureFormat1Channel32Bit textureFormat);
    const Format& TextureFormatToVulkanFormat(emberCommon::TextureFormat1Channel64Bit textureFormat);
    // 2 Channel Formats:
    const Format& TextureFormatToVulkanFormat(emberCommon::TextureFormat2Channel08Bit textureFormat);
    const Format& TextureFormatToVulkanFormat(emberCommon::TextureFormat2Channel16Bit textureFormat);
    const Format& TextureFormatToVulkanFormat(emberCommon::TextureFormat2Channel32Bit textureFormat);
    const Format& TextureFormatToVulkanFormat(emberCommon::TextureFormat2Channel64Bit textureFormat);
    // 3 Channel Formats:
    const Format& TextureFormatToVulkanFormat(emberCommon::TextureFormat3Channel08Bit textureFormat);
    const Format& TextureFormatToVulkanFormat(emberCommon::TextureFormat3Channel16Bit textureFormat);
    const Format& TextureFormatToVulkanFormat(emberCommon::TextureFormat3Channel32Bit textureFormat);
    const Format& TextureFormatToVulkanFormat(emberCommon::TextureFormat3Channel64Bit textureFormat);
    // 4 Channel Formats:
    const Format& TextureFormatToVulkanFormat(emberCommon::TextureFormat4Channel08Bit textureFormat);
    const Format& TextureFormatToVulkanFormat(emberCommon::TextureFormat4Channel16Bit textureFormat);
    const Format& TextureFormatToVulkanFormat(emberCommon::TextureFormat4Channel32Bit textureFormat);
    const Format& TextureFormatToVulkanFormat(emberCommon::TextureFormat4Channel64Bit textureFormat);
}