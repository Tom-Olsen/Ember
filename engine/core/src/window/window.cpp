#include "window.h"
#include "iWindow.h"



namespace emberEngine
{
	// Static members:
	bool Window::s_isInitialized = false;
	std::unique_ptr<emberBackendInterface::IWindow> Window::s_pIWindow;



	// Initialization/Cleanup:
	void Window::Init(emberBackendInterface::IWindow* pIWindow)
	{
		if (s_isInitialized)
			return;
		s_isInitialized = true;

		s_pIWindow = std::unique_ptr<emberBackendInterface::IWindow>(pIWindow);
	}
	void Window::Clear()
	{
		s_pIWindow.reset();
		s_isInitialized = false;
	}



	// Public methods:
	std::vector<emberCommon::Event> Window::PollEvents()
	{
		return s_pIWindow->PollEvents();
	}
	void Window::AddWindowInstanceExtensions(std::vector<const char*>& instanceExtensions)
	{
		s_pIWindow->AddWindowInstanceExtensions(instanceExtensions);
	}
	void Window::CreateSurface(VkInstance vkInstance, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pVkSurfaceKHR)
	{
		s_pIWindow->CreateSurface(vkInstance, pAllocator, pVkSurfaceKHR);
	}



	// Getters:
	bool Window::GetIsMinimized()
	{
		return s_pIWindow->GetIsMinimized();
	}
	bool Window::GetIsResized()
	{
		return s_pIWindow->GetIsResized();
	}
	Int2 Window::GetSize()
	{
		Int2 size = s_pIWindow->GetSize();
		return Int2(size[0], size[1]);
	}
	uint32_t Window::GetWindowID()
	{
		return s_pIWindow->GetWindowID();
	}
}