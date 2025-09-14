#include "renderer.h"
#include "emberTime.h"
#include "dearImGui.h"
#include "rendererCreateInfo.h"
#include "vulkanRenderer.h"
#include "window.h"



namespace emberEngine
{
	// Constructor/Destructor:
	Renderer::Renderer(const RendererCreateInfo& createInfo)
	{
		m_pIRenderer = std::make_unique<vulkanRendererBackend::Renderer>(createInfo);
		DearImGui::Init(Window::GetNativeHandle(), enableDockSpace);
	}
	Renderer::~Renderer()
	{
		DearImGui::Clear();
		m_pIRenderer.release();
	}



	// Public Methods:
	void Renderer::RenderFrame()
	{
		Int2 size = Window::GetSize();
		m_pIRenderer->RenderFrame(size.x, size.y, Time::GetTime(), Time::GetDeltaTime());
	}
}