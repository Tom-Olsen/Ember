#pragma once
#include "vulkanDescriptorType.h"
#include "vulkanShaderStage.h"
#include <string>



// Forward decleration:
typedef struct VkSampler_T* VkSampler;



namespace vulkanRendererBackend
{
    struct DescriptorSetLayoutBinding
    {
        uint32_t binding;
        DescriptorType descriptorType;
        uint32_t descriptorCount;
        ShaderStage shaderStage;
        const VkSampler* pImmutableSamplers;
        std::string ToString();
    };
}