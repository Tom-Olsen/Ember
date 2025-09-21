#pragma once
#include "textureFormat.h"
#include "vulkanFormat.h"



namespace vulkanRendererBackend
{
    // Format -> emberEngine::DepthStencilFormat:
    const emberEngine::DepthStencilFormat& ConvertDepthStencilFormat(Format format);

    // Format -> emberEngine::TextureFormat:
    const emberEngine::TextureFormat& ConvertTextureFormat(Format vkFormat);
}