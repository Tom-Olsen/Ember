#pragma once
#include "vulkanImageAspectFlag.h"
#include <cstdint>
#include <string>



namespace vulkanRendererBackend
{
    struct ImageSubresourceLayers
    {
        ImageAspectFlag aspectMask;
        uint32_t mipLevel;
        uint32_t baseArrayLayer;
        uint32_t layerCount;
        std::string ToString() const;
    };
}