#pragma once



#if defined(_WIN32) || defined(_WIN64)
	#if defined(VULKAN_RENDERER_BACKEND_EXPORTS)
		#define VULKAN_RENDERER_API __declspec(dllexport)
	#elif defined(VULKAN_RENDERER_BACKEND_STATIC)
		#define VULKAN_RENDERER_API
	#else
		#define VULKAN_RENDERER_API __declspec(dllimport)
	#endif
#else
	#if defined(VULKAN_RENDERER_BACKEND_STATIC)
		#define VULKAN_RENDERER_API
	#else
		#define VULKAN_RENDERER_API __attribute__((visibility("default")))
	#endif
#endif