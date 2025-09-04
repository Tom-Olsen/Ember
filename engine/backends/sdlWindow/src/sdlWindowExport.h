#ifndef __INCLUDE_GUARD_sdlWindowExport_h__
#define __INCLUDE_GUARD_sdlWindowExport_h__



#if defined(_WIN32) || defined(_WIN64)
	#if defined(SDL_WINDOW_BACKEND_EXPORTS)
		#define SDL_WINDOW_API __declspec(dllexport)
	#elif defined(SDL_WINDOW_BACKEND_STATIC)
		#define SDL_WINDOW_API
	#else
		#define SDL_WINDOW_API __declspec(dllimport)
	#endif
#else
	#if defined(SDL_WINDOW_BACKEND_STATIC)
		#define SDL_WINDOW_API
	#else
		#define SDL_WINDOW_API __attribute__((visibility("default")))
	#endif
#endif



#endif // __INCLUDE_GUARD_sdlWindowExport_h__