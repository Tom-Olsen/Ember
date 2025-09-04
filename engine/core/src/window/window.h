#ifndef __INCLUDE_GUARD_window_h__
#define __INCLUDE_GUARD_window_h__
#include "emberMath.h"
#include <memory>
#include <vector>



// Forward decleration:
typedef struct VkInstance_T* VkInstance;
struct VkAllocationCallbacks;
typedef struct VkSurfaceKHR_T* VkSurfaceKHR;
namespace emberBackendInterface
{
	class IWindow;
}



namespace emberEngine
{
	// Forward decleration:
	struct Event;



	class Window
	{
	private: // Members:
		static std::unique_ptr<emberBackendInterface::IWindow> s_pIWindow;

	public: // Methods:
		// Initialization/Ceanup:
		static void Init(uint16_t width, uint16_t height);
		static void Clear();

		// Window Methods:
		static void LinkDearImGui(void* pDeatImGui);
		static std::vector<Event> PollEvents();
		static void AddWindowInstanceExtensions(std::vector<const char*>& instanceExtensions);
		static void CreateSurface(VkInstance vkInstance, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pVkSurfaceKHR);

		// Getters:
		static bool GetIsMinimized();
		static bool GetIsResized();
		static void* GetNativeHandle();
		static Float2 GetSize();
		static uint32_t GetWindowID();

	private: // Methods
		// Delete all constructors:
		Window() = delete;
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;
		Window(Window&&) = delete;
		Window& operator=(Window&&) = delete;
		~Window() = delete;
	};
}



#endif // __INCLUDE_GUARD_window_h__