#pragma once
#include "iWindow.h"
#include "commonEvent.h"
#include "emberMath.h"
#include "sdlWindowExport.h"
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>



// Forward decleration:
struct SDL_Gamepad;
struct SDL_Window;
typedef struct VkInstance_T* VkInstance;
struct VkAllocationCallbacks;
typedef struct VkSurfaceKHR_T* VkSurfaceKHR;
namespace emberBackendInterface
{
	class IGui;
}



namespace sdlWindowBackend
{
	class SDL_WINDOW_API Window : public emberBackendInterface::IWindow
	{
	private: // Members:
		SDL_Window* m_pSdlWindow = nullptr;
		emberBackendInterface::IGui* m_pIGui = nullptr;
		std::vector<emberCommon::Event> m_events;
		std::unordered_map<uint32_t, SDL_Gamepad*> m_gamepads;
		const int m_maxEvents = 32;
		bool m_isMinimized = false;
		bool m_isResizing = false;
		bool m_enableDockSpace = true;

	public: // Methods:
		// Constructor/Destructor:
		Window(int windowWidth, int windowHeight, bool forceX11VideoDriver = false);
		~Window();

		// Non-copyable:
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		// Non-movable:
		Window(Window&&) = delete;
		Window& operator=(Window&&) = delete;

		// Window Methods:
		void LinkIGuiHandle(emberBackendInterface::IGui* pIGui) override;
		std::vector<emberCommon::Event> PollEvents() override;
		void AddWindowInstanceExtensions(std::vector<const char*>& instanceExtensions) const override;
		void CreateSurface(VkInstance vkInstance, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pVkSurfaceKHR) const override;

		// Getters:
		void* GetNativeHandle() const override;
		bool GetIsMinimized() const override;
		bool GetIsResizing() const override;
		Int2 GetSize() const override;
		Int2 GetSizeInPixels() const override;
		uint32_t GetWindowID() const override;

		// Setters:
		void ResetIsResizing() override;

	private: // Methods:
		void CloseGamepads();
	};
}