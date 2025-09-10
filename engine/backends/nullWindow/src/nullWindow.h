#ifndef __INCLUDE_GUARD_nullWindow_h__
#define __INCLUDE_GUARD_nullWindow_h__
#include "nullWindowExport.h"
#include "iMath.h"
#include "iWindow.h"
#include <vector>



// Forward decleration:
struct SDL_Window;



namespace nullWindowBackend
{
	class NULL_WINDOW_API NullWindow : public emberBackendInterface::IWindow
	{
	private: // Members:
		void* m_pWindow;

	public: // Methods:
		// Constructor/Destructor:
		NullWindow();
		~NullWindow();

		// Non-copyable:
		NullWindow(const NullWindow&) = delete;
		NullWindow& operator=(const NullWindow&) = delete;

		// Movable:
		NullWindow(NullWindow&& other) noexcept;
		NullWindow& operator=(NullWindow&& other) noexcept;

		// Window Methods:
		void LinkDearImGui(void* pDearImGui) override;
		std::vector<emberEngine::Event> PollEvents() override;
		void AddWindowInstanceExtensions(std::vector<const char*>& instanceExtensions) const override;
		void CreateSurface(VkInstance vkInstance, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pVkSurfaceKHR) const override;

		// Getters:
		bool GetIsMinimized() const override;
		bool GetIsResized() const override;
		void* GetNativeHandle() const;
		iMath::Int2 GetSize() const;
		uint32_t GetWindowID() const;
	};
}



#endif // __INCLUDE_GUARD_nullWindow_h__