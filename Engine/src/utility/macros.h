#ifndef __INCLUDE_GUARD_macros_h__
#define __INCLUDE_GUARD_macros_h__



namespace emberEngine
{
	// enable/disable initialization logging:
	#define LOG_INITIALIZATION



	// enable/disable Dear ImGui:
	//#define DISABLE_DEAR_IMGUI
	#ifdef DISABLE_DEAR_IMGUI
	#define RETURN_DISABLED() return
	#define RETURN_FALSE_DISABLED() return false
	#else
	#define RETURN_DISABLED()
	#define RETURN_FALSE_DISABLED()
	#endif
}



#endif // __INCLUDE_GUARD_macros_h__