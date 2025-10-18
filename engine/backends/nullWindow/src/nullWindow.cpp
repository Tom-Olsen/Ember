#include "nullWindow.h"



namespace nullWindowBackend
{
	// Public methods:
	// Constructor/Destructor:
	Window::Window()
	{

	}
	Window::~Window()
	{

	}



	// Window Methods:
	void Window::LinkIGuiHandle(emberBackendInterface::IGui* pDearImGui)
	{

	}
	std::vector<emberCommon::Event> Window::PollEvents()
	{
		return std::vector<emberCommon::Event>();
	}
	void Window::AddWindowInstanceExtensions(std::vector<const char*>& instanceExtensions) const
	{

	}
	void Window::CreateSurface(VkInstance vkInstance, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pVkSurfaceKHR) const
	{

	}

	// Getters:
	void* Window::GetNativeHandle() const
	{
		return nullptr;
	}
	bool Window::GetIsMinimized() const
	{
		return true;
	}
	bool Window::GetIsResized() const
	{
		return false;
	}
	Int2 Window::GetSize() const
	{
		return Int2{ 0, 0 };
	}
	uint32_t Window::GetWindowID() const
	{
		return 0;
	}



	// Setters:
	void Window::ResetWindowResized()
	{
		
	}
}