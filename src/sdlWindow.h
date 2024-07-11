#ifndef __INCLUDE_GUARD_widnow_h__
#define __INCLUDE_GUARD_widnow_h__
#include <SDL3/SDL.h>
#include <vulkan/vulkan.h>
#include <memory>
#include "logger.h"



class SdlWindow
{
public: // Members:
	SDL_Window* window;
	VkExtent2D extent;

private: // Members:
	SDL_Event event;

public: // Methods:
	SdlWindow(uint16_t width = 1280, uint16_t height = 720);
	~SdlWindow();
	bool HandelEvents();
	void SdlInstanceExtensions(std::vector<const char*>& extensions);
	int Width();
	int Height();
};
#endif // __INCLUDE_GUARD_widnow_h__