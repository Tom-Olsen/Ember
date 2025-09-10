#include "nullWindow.h"
#include "event.h"



namespace nullWindowBackend
{
	// Constructor/Destructor:
	NullWindow::NullWindow() : m_pWindow(nullptr)
	{

	}
	NullWindow::~NullWindow()
	{

	}

	// Movable:
	NullWindow::NullWindow(NullWindow&& other) noexcept
	{
		m_pWindow = nullptr;
	}
	NullWindow& NullWindow::operator=(NullWindow&& other) noexcept
	{
		m_pWindow = nullptr;
		return *this;
	}



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
	bool NullWindow::GetIsMinimized() const
	{
		return true;
	}
	bool NullWindow::GetIsResized() const
	{
		return false;
	}
	void* NullWindow::GetNativeHandle() const
	{
		return m_pWindow;
	}
	iMath::Int2 NullWindow::GetSize() const
	{
		return iMath::Int2{ 0, 0 };
	}
	uint32_t NullWindow::GetWindowID() const
	{
		return 0;
	}
}