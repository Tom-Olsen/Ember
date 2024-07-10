#ifndef __INCLUDE_GUARD_myMacros_h__
#define __INCLUDE_GUARD_myMacros_h__
// Macro to check if a Vulkan function returns VK_SUCCESS, if not it will break the program.
#define ASSERT_VULKAN(val) if (val != VK_SUCCESS) { __debugbreak(); }

// Quick ASSERT_VULKAN macro:
#ifndef VKA
#define VKA(f) ASSERT_VULKAN(f)
#endif

// Macro for array size:
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))
#endif // __INCLUDE_GUARD_myMacros_h__