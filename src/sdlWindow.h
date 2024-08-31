#ifndef __INCLUDE_GUARD_window_h__
#define __INCLUDE_GUARD_window_h__
#include <SDL3/SDL.h>
#include <vulkan/vulkan.h>
#include <vector>



class SdlWindow
{
public: // Members:
	SDL_Window* window;
	bool isMinimized = false;

private: // Members:
	SDL_Event event;

public: // Methods:
	SdlWindow(uint16_t width = 1280, uint16_t height = 720);
	~SdlWindow();

	bool HandelEvents();
	void AddSdlInstanceExtensions(std::vector<const char*>& extensions);
	int Width();
	int Height();
	VkExtent2D Extent();
};



#endif // __INCLUDE_GUARD_window_h__