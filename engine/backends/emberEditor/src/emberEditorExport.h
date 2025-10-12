#pragma once



#if defined(_WIN32) || defined(_WIN64)
	#if defined(EMBER_EDITOR_BACKEND_EXPORTS)
		#define EMBER_EDITOR_API __declspec(dllexport)
	#elif defined(EMBER_EDITOR_BACKEND_STATIC)
		#define EMBER_EDITOR_API
	#else
		#define EMBER_EDITOR_API __declspec(dllimport)
	#endif
#else
	#if defined(EMBER_EDITOR_BACKEND_STATIC)
		#define EMBER_EDITOR_API
	#else
		#define EMBER_EDITOR_API __attribute__((visibility("default")))
	#endif
#endif