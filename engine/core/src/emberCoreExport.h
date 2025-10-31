#pragma once



#if defined(_WIN32) || defined(_WIN64)
	#if defined(EMBER_CORE_EXPORTS)
		#define EMBER_CORE_API __declspec(dllexport)
	#elif defined(EMBER_CORE_STATIC)
		#define EMBER_CORE_API
	#else
		#define EMBER_CORE_API __declspec(dllimport)
	#endif
#else
	#if defined(EMBER_CORE_STATIC)
		#define EMBER_CORE_API
	#else
		#define EMBER_CORE_API __attribute__((visibility("default")))
	#endif
#endif