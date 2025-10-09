#include "window.h"
#include "logger.h"
#include "macros.h"
#include "nullWindow.h"
#include "profiler.h"
#include "sdlWindow.h"



namespace emberEngine
{
	// Static members:
	bool Window::s_isInitialized = false;
	std::unique_ptr<emberBackendInterface::IWindow> Window::s_pIWindow;
	emberBackendInterface::IWindow* Window::GetInterfaceHandle()
	{
		return s_pIWindow.get();
	}



	// Initialization/Cleanup:
	void Window::Init(int windowWidth, int windowHeight)
	{
		if (s_isInitialized)
			return;
		s_isInitialized = true;

		if (true)
			s_pIWindow = std::make_unique<sdlWindowBackend::Window>(windowWidth, windowHeight);
		else
			s_pIWindow = std::make_unique<nullWindowBackend::Window>();

		#ifdef LOG_INITIALIZATION
		LOG_TRACE("Window initialized.");
		#endif
	}
	void Window::Clear()
	{
		s_pIWindow.reset();
		s_isInitialized = false;
	}



	// Public methods:
	void Window::LinkDearImGui(emberBackendInterface::IGui* pDearImGui)
	{
		s_pIWindow->LinkDearImGui(pDearImGui);
	}
	std::vector<emberCommon::Event> Window::PollEvents()
	{
		PROFILE_FUNCTION();
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