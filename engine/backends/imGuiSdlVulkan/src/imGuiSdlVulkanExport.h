#pragma once



#if defined(_WIN32) || defined(_WIN64)
	#if defined(IMGUI_SDL_VULKAN_BACKEND_EXPORTS)
		#define IMGUI_SDL_VULKAN_API __declspec(dllexport)
	#elif defined(IMGUI_SDL_VULKAN_BACKEND_STATIC)
		#define IMGUI_SDL_VULKAN_API
	#else
		#define IMGUI_SDL_VULKAN_API __declspec(dllimport)
	#endif
#else
	#if defined(IMGUI_SDL_VULKAN_BACKEND_STATIC)
		#define IMGUI_SDL_VULKAN_API
	#else
		#define IMGUI_SDL_VULKAN_API __attribute__((visibility("default")))
	#endif
#endif