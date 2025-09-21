#pragma once
#include "vulkanVertexInputRate.h"
#include <string>



namespace vulkanRendererBackend
{
    struct VertexInputBindingDescription
    {
        uint32_t binding;
        uint32_t stride;
        VertexInputRate inputRate;
        std::string ToString();
    };
}