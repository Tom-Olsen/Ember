#include "sdlWindow.h"
#include "vulkanMacros.h"
#include <SDL3/SDL_vulkan.h>



// Constructor:
SdlWindow::SdlWindow(uint16_t width, uint16_t height)
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




// Public:
bool SdlWindow::HandelEvents()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))	// poll next event
	{
		switch (event.type)
		{
			// General events:
			case SDL_EVENT_QUIT:
				return false;	// stop application

			// Window events:
			case SDL_EVENT_WINDOW_MINIMIZED:
				isMinimized = true;
				break;
			case SDL_EVENT_WINDOW_RESTORED:
				isMinimized = false;
				break;
			case SDL_EVENT_WINDOW_RESIZED:
				framebufferResized = true;
				break;
			case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
				if (event.window.windowID == SDL_GetWindowID(window)) // check if the event is for this window
					return false;	// stop application
				break;

			// Keyboard events:
			case SDL_EVENT_KEY_DOWN:
				if (event.key.key == SDLK_ESCAPE)
					return false; // stop application or trigger other behavior
				break;
		}
	}

	return true;	// application running
}



void SdlWindow::AddSdlInstanceExtensions(std::vector<const char*>& extensions)
{
	// Get instance extensions:
	uint32_t sdlInstanceExtensionCount = 0;
	const char* const* sdlInstanceExtensions = SDL_Vulkan_GetInstanceExtensions(&sdlInstanceExtensionCount);

	// Insert array at the end of the vector:
	extensions.insert(extensions.end(), sdlInstanceExtensions, sdlInstanceExtensions + sdlInstanceExtensionCount);
}



int SdlWindow::Width()
{
	int width, height;
	SDL_GetWindowSize(window, &width, &height);
	return width;
}
int SdlWindow::Height()
{
	int width, height;
	SDL_GetWindowSize(window, &width, &height);
	return height;
}
VkExtent2D SdlWindow::Extent()
{
	int width, height;
	SDL_GetWindowSize(window, &width, &height);
	return VkExtent2D{ (uint32_t)width, (uint32_t)height };
}