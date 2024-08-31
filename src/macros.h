#ifndef __INCLUDE_GUARD_myMacros_h__
#define __INCLUDE_GUARD_myMacros_h__
#include <string>
#include "logger.h"



// Macro to check if a Vulkan function returns VK_SUCCESS:
#define VULKAN_ASSERT(val) \
    if (val != VK_SUCCESS) { \
        LOG_CRITICAL("File: {}, Line: {} Vulkan error: {}", __FILE__, __LINE__, std::to_string(val)); \
        std::abort(); \
    }

// Shorthand for VULKAN_ASSERT which can be disabled:
#ifndef VKA
#define VKA(f) VULKAN_ASSERT(f)
#endif



#endif // __INCLUDE_GUARD_myMacros_h__