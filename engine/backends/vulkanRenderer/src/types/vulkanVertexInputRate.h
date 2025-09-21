#pragma once
#include <cstdint>
#include <string>



namespace vulkanRendererBackend
{
    using VertexInputRate = uint32_t;
    struct VertexInputRates
    {
        static VertexInputRate vertex;
        static VertexInputRate instance;
        static VertexInputRate max_enum;
        static std::string ToString(VertexInputRate vertexInputRate);
    };
}