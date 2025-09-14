#include "nullWindow.h"
#include "event.h"



namespace nullWindowBackend
{
	// Constructor/Destructor:
	NullWindow::NullWindow()
	{

	}
	NullWindow::~NullWindow()
	{

	}



	// Window Methods:
	void NullWindow::LinkDearImGui(void* pDearImGui)
	{

	}
	std::vector<emberEngine::Event> NullWindow::PollEvents()
	{
		return std::vector<emberEngine::Event>();
	}
	void NullWindow::AddWindowInstanceExtensions(std::vector<const char*>& instanceExtensions) const
	{

	}
	void NullWindow::CreateSurface(VkInstance vkInstance, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pVkSurfaceKHR) const
	{

	}

	// Getters:
	void* NullWindow::GetNativeHandle() const
	{
		return nullptr;
	}
	bool NullWindow::GetIsMinimized() const
	{
		return true;
	}
	bool NullWindow::GetIsResized() const
	{
		return false;
	}
	Int2 NullWindow::GetSize() const
	{
		return Int2{ 0, 0 };
	}
	uint32_t NullWindow::GetWindowID() const
	{
		return 0;
	}
}