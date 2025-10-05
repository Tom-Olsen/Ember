#pragma once
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
	class Application;



	class Window
	{
		// Friends:
		friend class DearImGui;
		friend class Application;

	private: // Members:
		static bool s_isInitialized;
		static std::unique_ptr<emberBackendInterface::IWindow> s_pIWindow;
		static emberBackendInterface::IWindow* GetInterfaceHandle();

	public: // Methods:
		// Initialization/Cleanup:
		static void Init(int windowWidth, int windowHeight);
		static void Clear();

		// Window Methods:
		static void LinkDearImGui(void* pDeatImGui);
		static std::vector<Event> PollEvents();
		static void AddWindowInstanceExtensions(std::vector<const char*>& instanceExtensions);
		static void CreateSurface(VkInstance vkInstance, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pVkSurfaceKHR);

		// Getters:
		static bool GetIsMinimized();
		static bool GetIsResized();
		static Int2 GetSize();
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