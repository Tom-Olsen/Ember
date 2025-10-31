#pragma once
#include "commonEvent.h"
#include "emberCoreExport.h"
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
	class EMBER_CORE_API Window
	{
	private: // Members:
		static bool s_isInitialized;
		static std::unique_ptr<emberBackendInterface::IWindow> s_pIWindow;

	public: // Methods:
		// Initialization/Cleanup:
		static void Init(emberBackendInterface::IWindow* pIWindow);
		static void Clear();

		// Window Methods:
		static std::vector<emberCommon::Event> PollEvents();
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