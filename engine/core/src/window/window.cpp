#include "window.h"
#include "event.h"
#include "logger.h"
#include "macros.h"
#include "iMath.h"
#include "nullWindow.h"
#include "profiler.h"
#include "sdlWindow.h"



namespace emberEngine
{
	// Static members:
	std::unique_ptr<emberBackendInterface::IWindow> Window::s_pIWindow;



	// Initialization/Cleanup:
	void Window::Init(uint16_t width, uint16_t height)
	{
		if (true)
			s_pIWindow = std::make_unique<sdlWindowBackend::SdlWindow>(width, height);
		else
			s_pIWindow = std::make_unique<nullWindowBackend::NullWindow>();

		#ifdef LOG_INITIALIZATION
		LOG_TRACE("Window initialized.");
		#endif
	}
	void Window::Clear()
	{
		s_pIWindow.reset();
	}



	// Public methods:
	void Window::LinkDearImGui(void* pDearImGui)
	{
		s_pIWindow->LinkDearImGui(pDearImGui);
	}
	std::vector<Event> Window::PollEvents()
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
	void* Window::GetNativeHandle()
	{
		return s_pIWindow->GetNativeHandle();
	}
	Int2 Window::GetSize()
	{
		iMath::Int2 size = s_pIWindow->GetSize();
		return Int2(size[0], size[1]);
	}
	uint32_t Window::GetWindowID()
	{
		return s_pIWindow->GetWindowID();
	}
}