#ifndef __INCLUDE_GUARD_vulkanMacros_h__
#define __INCLUDE_GUARD_vulkanMacros_h__
#include "logger.h"
#include <string>



namespace emberEngine
{
    namespace vulkanBackend
    {
        // Macro to check if a Vulkan function returns VK_SUCCESS.
        // Do not use this macro directly, use the VKA macro instead, as it can be disabled easily.
        #define VULKAN_ASSERT(val) \
        if (val != VK_SUCCESS) { \
            LOG_CRITICAL("File: {}, Line: {} Vulkan error: {}", __FILE__, __LINE__, std::to_string(val)); \
            std::abort(); \
        }

        // Shorthand for VULKAN_ASSERT which can be disabled:
        #ifndef VKA
        #define VKA(f) VULKAN_ASSERT(f)
        #endif

        // Feature toggle macros:
        #define VALIDATION_LAYERS_ACTIVE        // enable/disable vulkan validation layers and vulkanAllocationTracker.
        //#define RESIZEABLE_BAR                // enabled = no staging buffer, disabled = with staging buffer



        // Naming macros:
        #ifdef VALIDATION_LAYERS_ACTIVE
            #define NAME_VK_BUFFER(buffer, name)                emberEngine::vulkanBackend::Context::SetObjectName(buffer, name)
            #define NAME_VK_IMAGE(image, name)                  emberEngine::vulkanBackend::Context::SetObjectName(image, name)
            #define NAME_VK_IMAGE_VIEW(imageView, name)         emberEngine::vulkanBackend::Context::SetObjectName(imageView, name)
            #define NAME_VK_BUFFER_VIEW(bufferView, name)       emberEngine::vulkanBackend::Context::SetObjectName(bufferView, name)
            #define NAME_VK_MEMORY(memory, name)                emberEngine::vulkanBackend::Context::SetObjectName(memory, name)
            #define NAME_VK_SHADER_MODULE(shaderModule, name)   emberEngine::vulkanBackend::Context::SetObjectName(shaderModule, name)
            #define NAME_VK_PIPELINE(pipeline, name)            emberEngine::vulkanBackend::Context::SetObjectName(pipeline, name)
            #define NAME_VK_LAYOUT(layout, name)                emberEngine::vulkanBackend::Context::SetObjectName(layout, name)
            #define NAME_VK_RENDER_PASS(renderPass, name)       emberEngine::vulkanBackend::Context::SetObjectName(renderPass, name)
            #define NAME_VK_FRAMEBUFFER(framebuffer, name)      emberEngine::vulkanBackend::Context::SetObjectName(framebuffer, name)
            #define NAME_VK_LAYOUT(layout, name)                emberEngine::vulkanBackend::Context::SetObjectName(layout, name)
            #define NAME_VK_DESCRIPTOR_SET(descriptorSet, name) emberEngine::vulkanBackend::Context::SetObjectName(descriptorSet, name)
            #define NAME_VK_POOL(pool, name)                    emberEngine::vulkanBackend::Context::SetObjectName(pool, name)
            #define NAME_VK_SAMPLER(sampler, name)              emberEngine::vulkanBackend::Context::SetObjectName(sampler, name)
            #define NAME_VK_COMMAND_POOL(commandPool, name)     emberEngine::vulkanBackend::Context::SetObjectName(commandPool, name)
            #define NAME_VK_COMMAND_BUFFER(commandBuffer, name) emberEngine::vulkanBackend::Context::SetObjectName(commandBuffer, name)
            #define NAME_VK_QUEUE(queue, name)                  emberEngine::vulkanBackend::Context::SetObjectName(queue, name)
            #define NAME_VK_SEMAPHORE(semaphore, name)          emberEngine::vulkanBackend::Context::SetObjectName(semaphore, name)
            #define NAME_VK_FENCE(fence, name)                  emberEngine::vulkanBackend::Context::SetObjectName(fence, name)
            #define NAME_VK_EVENT(vKevent, name)                emberEngine::vulkanBackend::Context::SetObjectName(vKevent, name)
            #define NAME_VK_QUERY_POOL(queryPool, name)         emberEngine::vulkanBackend::Context::SetObjectName(queryPool, name)
            #define NAME_VK_SWAPCHAIN(swapchain, name)          emberEngine::vulkanBackend::Context::SetObjectName(swapchain, name)
        #else
            #define NAME_VK_BUFFER(buffer, name)                ((void)0)
            #define NAME_VK_IMAGE(image, name)                  ((void)0)
            #define NAME_VK_IMAGE_VIEW(imageView, name)         ((void)0)
            #define NAME_VK_BUFFER_VIEW(bufferView, name)       ((void)0)
            #define NAME_VK_MEMORY(memory, name)                ((void)0)
            #define NAME_VK_SHADER_MODULE(shaderModule, name)   ((void)0)
            #define NAME_VK_PIPELINE(pipeline, name)            ((void)0)
            #define NAME_VK_LAYOUT(layout, name)                ((void)0)
            #define NAME_VK_RENDER_PASS(renderPass, name)       ((void)0)
            #define NAME_VK_FRAMEBUFFER(framebuffer, name)      ((void)0)
            #define NAME_VK_LAYOUT(layout, name)                ((void)0)
            #define NAME_VK_DESCRIPTOR_SET(descriptorSet, name) ((void)0)
            #define NAME_VK_POOL(pool, name)                    ((void)0)
            #define NAME_VK_SAMPLER(sampler, name)              ((void)0)
            #define NAME_VK_COMMAND_POOL(commandPool, name)     ((void)0)
            #define NAME_VK_COMMAND_BUFFER(commandBuffer, name) ((void)0)
            #define NAME_VK_QUEUE(queue, name)                  ((void)0)
            #define NAME_VK_SEMAPHORE(semaphore, name)          ((void)0)
            #define NAME_VK_FENCE(fence, name)                  ((void)0)
            #define NAME_VK_EVENT(vKevent, name)                ((void)0)
            #define NAME_VK_QUERY_POOL(queryPool, name)         ((void)0)
            #define NAME_VK_SWAPCHAIN(swapchain, name)          ((void)0)
        #endif
    }
}



#endif // __INCLUDE_GUARD_vulkanMacros_h__