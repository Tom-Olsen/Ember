#pragma once
#include "commonEvent.h"
#include "emberMath.h"
#include "nullWindowExport.h"
#include "iWindow.h"
#include <vector>



// Forward decleration:
namespace emberBackendInterface
{
	class IGui;
}



namespace nullWindowBackend
{
	class NULL_WINDOW_API Window : public emberBackendInterface::IWindow
	{
	public: // Methods:
		// Constructor/Destructor:
		Window();
		~Window();

		// Non-copyable:
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		// Movable:
		Window(Window&& other) noexcept = default;
		Window& operator=(Window&& other) noexcept = default;

		// Window Methods:
		void LinkIGuiHandle(emberBackendInterface::IGui* pDearImGui) override;
		std::vector<emberCommon::Event> PollEvents() override;
		void AddWindowInstanceExtensions(std::vector<const char*>& instanceExtensions) const override;
		void CreateSurface(VkInstance vkInstance, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pVkSurfaceKHR) const override;

		// Getters:
		void* GetNativeHandle() const;
		bool GetIsMinimized() const override;
		bool GetIsResized() const override;
		Int2 GetSize() const;
		uint32_t GetWindowID() const;

		// Setters:
		void ResetWindowResized() override;
	};
}