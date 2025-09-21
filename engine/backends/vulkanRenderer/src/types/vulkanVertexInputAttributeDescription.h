#pragma once
#include "vulkanFormat.h"
#include <string>



namespace vulkanRendererBackend
{
    struct VertexInputAttributeDescription
    {
        uint32_t location;
        uint32_t binding;
        Format format;
        uint32_t offset;
        std::string ToString();
    };
}