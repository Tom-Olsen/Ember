#define SDL_MAIN_HANDLED
#include "sdlWindow.h"
#include "iGui.h"
#include "sdlEventTranslation.h"
#include <assert.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <stdexcept>
#include <string>
#include <vulkan/vulkan.h>



namespace sdlWindowBackend
{
	// Public methods:
	// Constructor/Destructor:
	Window::Window(int windowWidth, int windowHeight)
	{
		// Assertions:
		assert(windowWidth > 0);
		assert(windowHeight > 0);

		// Initialize SDL:
		if (SDL_Init(SDL_INIT_VIDEO) == false)	// crashes after pulling latest version of sdl3
			throw std::runtime_error((std::string)"Window::Init: " + (std::string)SDL_GetError());

		// Create a window pointer:
		SDL_WindowFlags windowFlags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);
		m_pSdlWindow = SDL_CreateWindow("Ember", windowWidth, windowHeight, windowFlags);
		if (!m_pSdlWindow)
			throw std::runtime_error((std::string)"Window::Init: " + (std::string)SDL_GetError());

		m_events.reserve(m_maxEvents);
	}
	Window::~Window()
	{
		SDL_DestroyWindow(m_pSdlWindow);
		SDL_Quit();
	}



	// Move semantics:
	Window::Window(Window&& other) noexcept
	{
		m_pSdlWindow = other.m_pSdlWindow;
		m_events.reserve(m_maxEvents);
		other.m_pSdlWindow = nullptr;
	}
	Window& Window::operator=(Window&& other) noexcept
	{
		if (this != &other)
		{
			if (m_pSdlWindow)
				SDL_DestroyWindow(m_pSdlWindow);
			m_pSdlWindow = other.m_pSdlWindow;
			m_events.reserve(m_maxEvents);
			other.m_pSdlWindow = nullptr;
		}
		return *this;
	}



	// Window Methods:
	void Window::LinkDearImGui(emberBackendInterface::IGui* pIGui)
	{
		m_pIGui = pIGui;
	}
	std::vector<emberCommon::Event> Window::PollEvents()
	{
		SDL_Event sdlEvent;
		m_pIGui->ProcessEvent(&sdlEvent);
		m_events.clear();

		while (SDL_PollEvent(&sdlEvent))
		{
			emberCommon::Event event{};
			bool isEvent = true;

			// System / Window:
			switch (sdlEvent.type)
			{
				case SDL_EVENT_QUIT:
					event.type = emberCommon::EventType::Quit;
					break;
				case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
					event.type = emberCommon::EventType::WindowClose;
					break;
				case SDL_EVENT_WINDOW_RESIZED:
					m_isResized = true;
					event.type = emberCommon::EventType::WindowResized;
					event.windowID = sdlEvent.window.windowID;
					event.resizeWidth = sdlEvent.window.data1;
					event.resizeHeight = sdlEvent.window.data2;
					break;
				case SDL_EVENT_WINDOW_MINIMIZED:
					m_isMinimized = true;
					event.type = emberCommon::EventType::WindowMinimized;
					event.windowID = sdlEvent.window.windowID;
					event.resizeWidth = 0;
					event.resizeHeight = 0;
					break;
				case SDL_EVENT_WINDOW_RESTORED:
					m_isMinimized = false;
					event.type = emberCommon::EventType::WindowRestored;
					event.windowID = sdlEvent.window.windowID;
					event.resizeWidth = sdlEvent.window.data1;
					event.resizeHeight = sdlEvent.window.data2;
					break;
				case SDL_EVENT_WINDOW_FOCUS_GAINED:
					event.type = emberCommon::EventType::WindowFocusGained;
					event.windowID = sdlEvent.window.windowID;
					break;
				case SDL_EVENT_WINDOW_FOCUS_LOST:
					event.type = emberCommon::EventType::WindowFocusLost;
					event.windowID = sdlEvent.window.windowID;
					break;

				// Keyboard:
				case SDL_EVENT_KEY_DOWN:
					event.type = emberCommon::EventType::KeyDown;
					event.key = TranslateKey(sdlEvent.key.key);
					break;
				case SDL_EVENT_KEY_UP:
					event.type = emberCommon::EventType::KeyUp;
					event.key = TranslateKey(sdlEvent.key.key);
					break;
				case SDL_EVENT_TEXT_INPUT:
					event.type = emberCommon::EventType::TextInput;
					event.text = sdlEvent.text.text;
					break;

				// Mouse:
				case SDL_EVENT_MOUSE_MOTION:
					event.type = emberCommon::EventType::MouseMoved;
					event.mousePosX = sdlEvent.motion.x;
					event.mousePosY = sdlEvent.motion.y;
					break;
				case SDL_EVENT_MOUSE_BUTTON_DOWN:
					event.type = emberCommon::EventType::MouseButtonDown;
					event.mouseButton = TranslateMouseButton(sdlEvent.button.button);
					event.mousePosX = sdlEvent.button.x;
					event.mousePosY = sdlEvent.button.y;
					break;
				case SDL_EVENT_MOUSE_BUTTON_UP:
					event.type = emberCommon::EventType::MouseButtonUp;
					event.mouseButton = TranslateMouseButton(sdlEvent.button.button);
					event.mousePosX = sdlEvent.button.x;
					event.mousePosY = sdlEvent.button.y;
					break;
				case SDL_EVENT_MOUSE_WHEEL:
					event.type = emberCommon::EventType::MouseWheel;
					event.mouseWheelX = sdlEvent.wheel.x;
					event.mouseWheelY = sdlEvent.wheel.y;
					break;

				// Controller:
				case SDL_EVENT_GAMEPAD_ADDED:
					event.type = emberCommon::EventType::ControllerConnected;
					event.controllerId = sdlEvent.cdevice.which;
					break;
				case SDL_EVENT_GAMEPAD_REMOVED:
					event.type = emberCommon::EventType::ControllerDisconnected;
					event.controllerId = sdlEvent.cdevice.which;
					break;
				case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
					event.type = emberCommon::EventType::ControllerButtonDown;
					event.controllerId = sdlEvent.gbutton.which;
					event.controllerButton = TranslateControllerButton(sdlEvent.gbutton.button);
					break;
				case SDL_EVENT_GAMEPAD_BUTTON_UP:
					event.type = emberCommon::EventType::ControllerButtonUp;
					event.controllerId = sdlEvent.gbutton.which;
					event.controllerButton = TranslateControllerButton(sdlEvent.gbutton.button);
					break;
				case SDL_EVENT_GAMEPAD_AXIS_MOTION:
					event.type = emberCommon::EventType::ControllerAxisMotion;
					event.controllerId = sdlEvent.gaxis.which;
					event.axis = sdlEvent.gaxis.axis;
					event.axisValue = sdlEvent.gaxis.value;
					break;
				default:
					isEvent = false;
					break;
			}

			if (isEvent)
				m_events.push_back(event);
		}
		return m_events;
	}
	void Window::AddWindowInstanceExtensions(std::vector<const char*>& instanceExtensions) const
	{
		// Get instance extensions:
		uint32_t sdlInstanceExtensionCount = 0;
		const char* const* sdlInstanceExtensions = SDL_Vulkan_GetInstanceExtensions(&sdlInstanceExtensionCount);

		// Insert array at the end of the vector:
		instanceExtensions.insert(instanceExtensions.end(), sdlInstanceExtensions, sdlInstanceExtensions + sdlInstanceExtensionCount);
	}
	void Window::CreateSurface(VkInstance vkInstance, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pVkSurfaceKHR) const
	{
		SDL_Vulkan_CreateSurface(m_pSdlWindow, vkInstance, pAllocator, pVkSurfaceKHR);
	}



	// Getters:
	void* Window::GetNativeHandle() const
	{
		return (void*)m_pSdlWindow;
	}
	bool Window::GetIsMinimized() const
	{
		return m_isMinimized;
	}
	bool Window::GetIsResized() const
	{
		return m_isResized;
	}
	Int2 Window::GetSize() const
	{
		int width, height;
		SDL_GetWindowSize(m_pSdlWindow, &width, &height);
		return Int2{ width, height };
	}
	uint32_t Window::GetWindowID() const
	{
		return (uint32_t)SDL_GetWindowID(m_pSdlWindow);
	}
}