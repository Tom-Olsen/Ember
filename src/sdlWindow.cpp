#include "sdlWindow.h"
#include <SDL3/SDL_vulkan.h>
#include "logger.h"



// Constructor:
SdlWindow::SdlWindow(uint16_t width, uint16_t height) : extent{ width, height }
{
	// Initialize SDL:
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
		throw std::runtime_error((std::string)"SDL_Init: " + (std::string)SDL_GetError());

	// Create a window:
	window = SDL_CreateWindow("My Engine", width, height, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
	if (!window)
		throw std::runtime_error((std::string)"SDL_CreateWindow Error: " + (std::string)SDL_GetError());
}



// Destructor:
SdlWindow::~SdlWindow()
{
	SDL_DestroyWindow(window);
	SDL_Quit();
}




bool SdlWindow::HandelEvents()
{
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_EVENT_QUIT:
			return false;
		}
	}
	return true;
}



void SdlWindow::SdlInstanceExtensions(std::vector<const char*>& extensions)
{
	// Get instance extensions:
	uint32_t sdlInstanceExtensionCount = 0;
	const char* const* sdlInstanceExtensions = SDL_Vulkan_GetInstanceExtensions(&sdlInstanceExtensionCount);

	// Insert array at the end of the vector:
	extensions.insert(extensions.end(), sdlInstanceExtensions, sdlInstanceExtensions + sdlInstanceExtensionCount);
}



int SdlWindow::Width()
{ return extent.width; }
int SdlWindow::Height()
{ return extent.height; }