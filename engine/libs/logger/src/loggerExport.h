#pragma once



#if defined(_WIN32) || defined(_WIN64)
	#if defined(LOGGER_EXPORTS)
		#define LOGGER_API __declspec(dllexport)
	#elif defined(LOGGER_STATIC)
		#define LOGGER_API
	#else
		#define LOGGER_API __declspec(dllimport)
	#endif
#else
	#if defined(LOGGER_STATIC)
		#define LOGGER_API
	#else
		#define LOGGER_API __attribute__((visibility("default")))
	#endif
#endif