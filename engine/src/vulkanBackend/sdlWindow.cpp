#define SDL_STATIC
#include "sdlWindow.h"
#include "eventSystem.h"
#include "dearImGui.h"
#include "profiler.h"
#include "vulkanMacros.h"
#include <assert.h>
#include <SDL3/SDL_vulkan.h>



namespace emberEngine
{
	namespace vulkanBackend
	{
		// Constructor/Destructor:
		SdlWindow::SdlWindow()
		{
			m_pWindow = nullptr;
			m_isMinimized = false;
			m_framebufferResized = false;
		}
		void SdlWindow::Init(uint16_t width, uint16_t height)
		{
			// Assertions:
			assert(width > 0);
			assert(height > 0);

			// Initialize SDL:
			if (SDL_Init(SDL_INIT_VIDEO) == false)	// crashes after pulling latest version of sdl3
				throw std::runtime_error((std::string)"SDL_Init: " + (std::string)SDL_GetError());

			// Create a window pointer:
			SDL_WindowFlags windowFlags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);
			m_pWindow = SDL_CreateWindow("Ember", width, height, windowFlags);
			if (!m_pWindow)
				throw std::runtime_error((std::string)"SDL_CreateWindow Error: " + (std::string)SDL_GetError());
		}
		SdlWindow::~SdlWindow()
		{
			SDL_DestroyWindow(m_pWindow);
			SDL_Quit();
		}



		// Public methods:
		bool SdlWindow::HandleEvents()
		{
			PROFILE_FUNCTION();
			// Reset event system:
			EventSystem::ClearEvents();

			// Poll events:
			SDL_Event event;
			while (SDL_PollEvent(&event)) // use SDL_WaitEvent?
			{
				// Forward event to ImGui and event system:
				DearImGui::ProcessEvent(event);
				EventSystem::ProcessEvent(event);

				switch (event.type)
				{
					// General events:
					case SDL_EVENT_QUIT:
						return false;	// stop application

					// Window events:
					case SDL_EVENT_WINDOW_MINIMIZED:
						m_isMinimized = true;
						break;
					case SDL_EVENT_WINDOW_RESTORED:
						m_isMinimized = false;
						break;
					case SDL_EVENT_WINDOW_RESIZED:
						m_framebufferResized = true;
						break;
					case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
						if (event.window.windowID == SDL_GetWindowID(m_pWindow)) // check if the event is for this window
							return false;	// stop application
						break;
						
					// Keyboard events:
					case SDL_EVENT_KEY_DOWN:
						if (!DearImGui::WantCaptureKeyboard() && event.key.key == SDLK_ESCAPE)
							return false; // stop application or trigger other behavior
						break;
				}
			}

			return true;	// application running
		}
		void SdlWindow::AddSdlInstanceExtensions(std::vector<const char*>& instanceExtensions) const
		{
			// Get instance extensions:
			uint32_t sdlInstanceExtensionCount = 0;
			const char* const* sdlInstanceExtensions = SDL_Vulkan_GetInstanceExtensions(&sdlInstanceExtensionCount);

			// Insert array at the end of the vector:
			instanceExtensions.insert(instanceExtensions.end(), sdlInstanceExtensions, sdlInstanceExtensions + sdlInstanceExtensionCount);
		}

		// Getters:
		SDL_Window* const SdlWindow::GetSDL_Window() const
		{
			return m_pWindow;
		}
		bool SdlWindow::GetIsMinimized() const
		{
			return m_isMinimized;
		}
		bool SdlWindow::GetFramebufferResized() const
		{
			return m_framebufferResized;
		}
		int SdlWindow::GetWidth() const
		{
			int width, height;
			SDL_GetWindowSize(m_pWindow, &width, &height);
			return width;
		}
		int SdlWindow::GetHeight() const
		{
			int width, height;
			SDL_GetWindowSize(m_pWindow, &width, &height);
			return height;
		}
		VkExtent2D SdlWindow::GetExtent() const
		{
			int width, height;
			SDL_GetWindowSize(m_pWindow, &width, &height);
			return VkExtent2D{ (uint32_t)width, (uint32_t)height };
		}

		// Setters:
		void SdlWindow::SetFramebufferResized(bool value)
		{
			m_framebufferResized = value;
		}
	}
}