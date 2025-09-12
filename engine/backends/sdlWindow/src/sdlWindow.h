#ifndef __INCLUDE_GUARD_sdlWindow_h__
#define __INCLUDE_GUARD_sdlWindow_h__
#include "emberMath.h"
#include "sdlWindowExport.h"
#include "iWindow.h"
#include <memory>
#include <vector>



// Forward decleration:
struct SDL_Window;
typedef struct VkInstance_T* VkInstance;
struct VkAllocationCallbacks;
typedef struct VkSurfaceKHR_T* VkSurfaceKHR;
namespace emberEngine
{
	struct Event;
}
namespace emberBackendInterface
{
	class IDearImGui;
}



namespace sdlWindowBackend
{
	class SDL_WINDOW_API SdlWindow : public emberBackendInterface::IWindow
	{
	private: // Members:
		SDL_Window* m_pWindow = nullptr;
		emberBackendInterface::IDearImGui* m_pDearImGui = nullptr;
		std::vector<emberEngine::Event> m_events;
		const int m_maxEvents = 32;
		bool m_isMinimized = false;
		bool m_isResized = false;
		bool m_enableDockSpace = true;

	public: // Methods:
		// Constructor/Destructor:
		SdlWindow(uint16_t width, uint16_t height);
		~SdlWindow();

		// Non-copyable:
		SdlWindow(const SdlWindow&) = delete;
		SdlWindow& operator=(const SdlWindow&) = delete;

		// Movable:
		SdlWindow(SdlWindow&& other) noexcept;
		SdlWindow& operator=(SdlWindow&& other) noexcept;

		// Window Methods:
		void LinkDearImGui(void* pDearImGui) override;
		std::vector<emberEngine::Event> PollEvents() override;
		void AddWindowInstanceExtensions(std::vector<const char*>& instanceExtensions) const override;
		void CreateSurface(VkInstance vkInstance, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pVkSurfaceKHR) const override;

		// Getters:
		bool GetIsMinimized() const override;
		bool GetIsResized() const override;
		void* GetNativeHandle() const override;
		Int2 GetSize() const override;
		uint32_t GetWindowID() const override;
	};
}



#endif // __INCLUDE_GUARD_sdlWindow_h__