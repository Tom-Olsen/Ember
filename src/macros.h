#ifndef __INCLUDE_GUARD_myMacros_h__
#define __INCLUDE_GUARD_myMacros_h__
#include <string>
#include "logger.h"



// Macro to check if a Vulkan function returns VK_SUCCESS:
#define ASSERT_VULKAN(val) \
    if (val != VK_SUCCESS) { \
        LOG_CRITICAL("File: {}, Line: {} Vulkan error: {}", __FILE__, __LINE__, std::to_string(val)); \
        std::abort(); \
    }

// Quick ASSERT_VULKAN macro:
#ifndef VKA
#define VKA(f) ASSERT_VULKAN(f)
#endif



#endif // __INCLUDE_GUARD_myMacros_h__