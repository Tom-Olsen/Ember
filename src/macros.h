#ifndef __INCLUDE_GUARD_myMacros_h__
#define __INCLUDE_GUARD_myMacros_h__
#include <string>
#include "logger.h"



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



// Macro to check if a SPIRV-Reflect function returns SPV_REFLECT_RESULT_SUCCESS.
#define SPIRV_REFLECT_ASSERT(val) \
    if (val != SPV_REFLECT_RESULT_SUCCESS) { \
        LOG_CRITICAL("File: {}, Line: {} SPIRV-Reflect error: {}", __FILE__, __LINE__, std::to_string(val)); \
        std::abort(); \
    }

// Shorthand for SPIRV_REFLECT_ASSERT which can be disabled:
#ifndef SPVA
#define SPVA(f) SPIRV_REFLECT_ASSERT(f)
#endif



// Controle flow macros:
#define VALIDATION_LAYERS_ACTIVE    // enable/disable vulkan validation layers
//#define RESIZEABLE_BAR    // enabled = no staging buffer, disabled = with staging buffer



#endif // __INCLUDE_GUARD_myMacros_h__