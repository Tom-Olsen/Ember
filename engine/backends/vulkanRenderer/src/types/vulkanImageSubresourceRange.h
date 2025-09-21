#pragma once
#include "vulkanImageAspectFlag.h"
#include <cstdint>
#include <string>



namespace vulkanRendererBackend
{
	struct ImageSubresourceRange
	{
        ImageAspectFlag aspectMask;
        uint32_t baseMipLevel;
        uint32_t levelCount;
        uint32_t baseArrayLayer;
        uint32_t layerCount;
        std::string ToString() const;
	};
}