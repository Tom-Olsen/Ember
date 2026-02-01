#pragma once
#include "logger.h"
#include "vulkanContext.h"
#include <string>



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
#define VALIDATION_LAYERS_ACTIVE      // enable/disable vulkan validation layers and vulkanAllocationTracker.
//#define RESIZEABLE_BAR                // enabled = no staging buffer, disabled = with staging buffer



// Naming macros:
#ifdef VALIDATION_LAYERS_ACTIVE
    #define NAME_VK_OBJECT(buffer, name) vulkanRendererBackend::Context::SetObjectName(buffer, name)
#else
    #define NAME_VK_OBJECT(buffer, name) ((void)0)
#endif