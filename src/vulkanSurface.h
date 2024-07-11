#ifndef __INCLUDE_GUARD_vulkanSurface_h__
#define __INCLUDE_GUARD_vulkanSurface_h__
#include <SDL3/SDL.h>
#include "sdlWindow.h"
#include "vulkanContext.h"



class VulkanSurface
{
private:
	// external:
	VulkanContext* context;
	SdlWindow* window;

public:
	// internal:
	VkSurfaceKHR surface;
	VkSurfaceFormatKHR surfaceFormat;

	// Constructor:
	VulkanSurface(VulkanContext* context, SdlWindow* window);

	// Destructor:
	~VulkanSurface();

private:
	void CreateSurface();
	void CreateSurfaceFormat();
};
#endif // __INCLUDE_GUARD_vulkanSurface_h__