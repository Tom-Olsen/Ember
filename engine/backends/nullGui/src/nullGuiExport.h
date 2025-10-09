#pragma once



#if defined(_WIN32) || defined(_WIN64)
	#if defined(NULL_GUI_BACKEND_EXPORTS)
		#define NULL_GUI_API __declspec(dllexport)
	#elif defined(NULL_GUI_BACKEND_STATIC)
		#define NULL_GUI_API
	#else
		#define NULL_GUI_API __declspec(dllimport)
	#endif
#else
	#if defined(NULL_GUI_BACKEND_STATIC)
		#define NULL_GUI_API
	#else
		#define NULL_GUI_API __attribute__((visibility("default")))
	#endif
#endif