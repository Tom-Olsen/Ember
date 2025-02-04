#ifndef __INCLUDE_GUARD_sdlWindow_h__
#define __INCLUDE_GUARD_sdlWindow_h__
#include <SDL3/SDL.h>
#include <vulkan/vulkan.h>
#include <vector>



class SdlWindow
{
private: // Members:
	SDL_Window* m_pWindow;
	bool m_isMinimized = false;
	bool m_framebufferResized = false;

public: // Methods:
	SdlWindow(uint16_t width = 1280, uint16_t height = 720);
	~SdlWindow();

	bool HandleEvents();
	void AddSdlInstanceExtensions(std::vector<const char*>& extensions) const;

	// Getters:
	SDL_Window* const GetSDL_Window() const;
	bool GetIsMinimized() const;
	bool GetFramebufferResized() const;
	int GetWidth() const;
	int GetHeight() const;
	VkExtent2D GetExtent() const;

	// Setters:
	void SetFramebufferResized(bool value);
};



#endif // __INCLUDE_GUARD_sdlWindow_h__