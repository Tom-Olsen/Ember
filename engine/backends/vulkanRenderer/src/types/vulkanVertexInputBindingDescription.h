#pragma once
#include <string>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
    struct VertexInputBindingDescription
    {
        uint32_t binding;
        uint32_t stride;
        VkVertexInputRate inputRate;
        std::string ToString();
    };
}