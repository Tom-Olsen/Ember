#define SDL_STATIC
#include "sdlWindow.h"
#include "iDearImGui.h"
#include "event.h"
#include "sdlEventTranslation.h"
#include <assert.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <stdexcept>
#include <string>
#include <vulkan/vulkan.h>



namespace sdlWindowBackend
{
	// Constructor/Destructor:
	SdlWindow::SdlWindow(uint16_t width, uint16_t height)
	{
		// Assertions:
		assert(width > 0);
		assert(height > 0);

		// Initialize SDL:
		if (SDL_Init(SDL_INIT_VIDEO) == false)	// crashes after pulling latest version of sdl3
			throw std::runtime_error((std::string)"SdlWindow::Init: " + (std::string)SDL_GetError());

		// Create a window pointer:
		SDL_WindowFlags windowFlags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);
		m_pWindow = SDL_CreateWindow("Ember", width, height, windowFlags);
		if (!m_pWindow)
			throw std::runtime_error((std::string)"SdlWindow::Init: " + (std::string)SDL_GetError());

		m_events.reserve(m_maxEvents);
	}
	SdlWindow::~SdlWindow()
	{
		SDL_DestroyWindow(m_pWindow);
		SDL_Quit();
	}



	// Move semantics:
	SdlWindow::SdlWindow(SdlWindow&& other) noexcept
	{
		m_pWindow = other.m_pWindow;
		m_events.reserve(m_maxEvents);
		other.m_pWindow = nullptr;
	}
	SdlWindow& SdlWindow::operator=(SdlWindow&& other) noexcept
	{
		if (this != &other)
		{
			if (m_pWindow)
				SDL_DestroyWindow(m_pWindow);
			m_pWindow = other.m_pWindow;
			m_events.reserve(m_maxEvents);
			other.m_pWindow = nullptr;
		}
		return *this;
	}



	// Public methods:
	// Window Methods:
	void SdlWindow::LinkDearImGui(void* pDearImGui)
	{
		m_pDearImGui = static_cast<emberBackendInterface::IDearImGui*>(pDearImGui);
	}
	std::vector<emberEngine::Event> SdlWindow::PollEvents()
	{
		SDL_Event sdlEvent;
		m_pDearImGui->ProcessEvent(&sdlEvent);
		m_events.clear();

		while (SDL_PollEvent(&sdlEvent))
		{
			emberEngine::Event event{};
			bool isEvent = true;

			// System / Window:
			switch (sdlEvent.type)
			{
				case SDL_EVENT_QUIT:
					event.type = emberEngine::EventType::Quit;
					break;
				case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
					event.type = emberEngine::EventType::WindowClose;
					break;
				case SDL_EVENT_WINDOW_RESIZED:
					m_isResized = true;
					event.type = emberEngine::EventType::WindowResized;
					event.windowID = sdlEvent.window.windowID;
					event.resizeWidth = sdlEvent.window.data1;
					event.resizeHeight = sdlEvent.window.data2;
					break;
				case SDL_EVENT_WINDOW_MINIMIZED:
					m_isMinimized = true;
					event.type = emberEngine::EventType::WindowMinimized;
					event.windowID = sdlEvent.window.windowID;
					event.resizeWidth = 0;
					event.resizeHeight = 0;
					break;
				case SDL_EVENT_WINDOW_RESTORED:
					m_isMinimized = false;
					event.type = emberEngine::EventType::WindowRestored;
					event.windowID = sdlEvent.window.windowID;
					event.resizeWidth = sdlEvent.window.data1;
					event.resizeHeight = sdlEvent.window.data2;
					break;
				case SDL_EVENT_WINDOW_FOCUS_GAINED:
					event.type = emberEngine::EventType::WindowFocusGained;
					event.windowID = sdlEvent.window.windowID;
					break;
				case SDL_EVENT_WINDOW_FOCUS_LOST:
					event.type = emberEngine::EventType::WindowFocusLost;
					event.windowID = sdlEvent.window.windowID;
					break;

				// Keyboard:
				case SDL_EVENT_KEY_DOWN:
					event.type = emberEngine::EventType::KeyDown;
					event.key = TranslateKey(sdlEvent.key.key);
					break;
				case SDL_EVENT_KEY_UP:
					event.type = emberEngine::EventType::KeyUp;
					event.key = TranslateKey(sdlEvent.key.key);
					break;
				case SDL_EVENT_TEXT_INPUT:
					event.type = emberEngine::EventType::TextInput;
					event.text = sdlEvent.text.text;
					break;

				// Mouse:
				case SDL_EVENT_MOUSE_MOTION:
					event.type = emberEngine::EventType::MouseMoved;
					event.mousePosX = sdlEvent.motion.x;
					event.mousePosY = sdlEvent.motion.y;
					break;
				case SDL_EVENT_MOUSE_BUTTON_DOWN:
					event.type = emberEngine::EventType::MouseButtonDown;
					event.mouseButton = TranslateMouseButton(sdlEvent.button.button);
					event.mousePosX = sdlEvent.button.x;
					event.mousePosY = sdlEvent.button.y;
					break;
				case SDL_EVENT_MOUSE_BUTTON_UP:
					event.type = emberEngine::EventType::MouseButtonUp;
					event.mouseButton = TranslateMouseButton(sdlEvent.button.button);
					event.mousePosX = sdlEvent.button.x;
					event.mousePosY = sdlEvent.button.y;
					break;
				case SDL_EVENT_MOUSE_WHEEL:
					event.type = emberEngine::EventType::MouseWheel;
					event.mouseWheelX = sdlEvent.wheel.x;
					event.mouseWheelY = sdlEvent.wheel.y;
					break;

				// Controller:
				case SDL_EVENT_GAMEPAD_ADDED:
					event.type = emberEngine::EventType::ControllerConnected;
					event.controllerId = sdlEvent.cdevice.which;
					break;
				case SDL_EVENT_GAMEPAD_REMOVED:
					event.type = emberEngine::EventType::ControllerDisconnected;
					event.controllerId = sdlEvent.cdevice.which;
					break;
				case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
					event.type = emberEngine::EventType::ControllerButtonDown;
					event.controllerId = sdlEvent.gbutton.which;
					event.controllerButton = TranslateControllerButton(sdlEvent.gbutton.button);
					break;
				case SDL_EVENT_GAMEPAD_BUTTON_UP:
					event.type = emberEngine::EventType::ControllerButtonUp;
					event.controllerId = sdlEvent.gbutton.which;
					event.controllerButton = TranslateControllerButton(sdlEvent.gbutton.button);
					break;
				case SDL_EVENT_GAMEPAD_AXIS_MOTION:
					event.type = emberEngine::EventType::ControllerAxisMotion;
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
	void SdlWindow::AddWindowInstanceExtensions(std::vector<const char*>& instanceExtensions) const
	{
		// Get instance extensions:
		uint32_t sdlInstanceExtensionCount = 0;
		const char* const* sdlInstanceExtensions = SDL_Vulkan_GetInstanceExtensions(&sdlInstanceExtensionCount);

		// Insert array at the end of the vector:
		instanceExtensions.insert(instanceExtensions.end(), sdlInstanceExtensions, sdlInstanceExtensions + sdlInstanceExtensionCount);
	}
	void SdlWindow::CreateSurface(VkInstance vkInstance, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pVkSurfaceKHR) const
	{
		SDL_Vulkan_CreateSurface(m_pWindow, vkInstance, pAllocator, pVkSurfaceKHR);
	}


	// Getters:
	bool SdlWindow::GetIsMinimized() const
	{
		return m_isMinimized;
	}
	bool SdlWindow::GetIsResized() const
	{
		return m_isResized;
	}
	void* SdlWindow::GetNativeHandle() const
	{
		return (void*)m_pWindow;
	}
	iMath::Int2 SdlWindow::GetSize() const
	{
		int width, height;
		SDL_GetWindowSize(m_pWindow, &width, &height);
		return iMath::Int2{ width, height };
	}
	uint32_t SdlWindow::GetWindowID() const
	{
		return (uint32_t)SDL_GetWindowID(m_pWindow);
	}
}