#ifndef __INCLUDE_GUARD_nullWindowExport_h__
#define __INCLUDE_GUARD_nullWindowExport_h__



#if defined(_WIN32) || defined(_WIN64)
	#if defined(NULL_WINDOW_BACKEND_EXPORTS)
		#define NULL_WINDOW_API __declspec(dllexport)
	#elif defined(NULL_WINDOW_BACKEND_STATIC)
		#define NULL_WINDOW_API
	#else
		#define NULL_WINDOW_API __declspec(dllimport)
	#endif
#else
	#if defined(NULL_WINDOW_BACKEND_STATIC)
		#define NULL_WINDOW_API
	#else
		#define NULL_WINDOW_API __attribute__((visibility("default")))
	#endif
#endif



#endif // __INCLUDE_GUARD_nullWindowExport_h__