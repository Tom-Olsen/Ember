#include "gui.h"
#include "iGui.h"
#include "imGuiSdlVulkan.h"
#include "logger.h"
#include "macros.h"
#include "nullGui.h"
#include "renderer.h"
#include "texture2d.h"
#include "window.h"



namespace emberEngine
{
	// Static members:
	bool Gui::s_isInitialized = false;
	std::unique_ptr<emberBackendInterface::IGui> Gui::s_pIGui;
	emberBackendInterface::IGui* Gui::GetInterfaceHandle()
	{
		return s_pIGui.get();
	}



	// Initialization/Cleanup:
	void Gui::Init(bool enableDockSpace)
	{
		if (s_isInitialized)
			return;
		s_isInitialized = true;

		if (true)
			s_pIGui = std::make_unique<imGuiSdlVulkanBackend::Gui>(Window::GetInterfaceHandle(), Renderer::GetInterfaceHandle(), enableDockSpace);
		else
			s_pIGui = std::make_unique<nullGuiBackend::Gui>();

		Window::LinkDearImGui(s_pIGui.get());
		#ifdef LOG_INITIALIZATION
		LOG_TRACE("Gui initialized.");
		#endif
	}
	void Gui::Clear()
	{
		s_pIGui.reset();
		s_isInitialized = false;
	}



	// Render Logic:
	void Gui::Update()
	{
		s_pIGui->Update();
	}
	void Gui::ProcessEvent(const void* pEvent)
	{
		s_pIGui->ProcessEvent(pEvent);
	}
	void Gui::Render(VkCommandBuffer vkCommandBuffer)
	{
		s_pIGui->Render(vkCommandBuffer);
	}



	// Getters:
	bool Gui::WantCaptureKeyboard()
	{
		return s_pIGui->WantCaptureKeyboard();
	}
	bool Gui::WantCaptureMouse()
	{
		return s_pIGui->WantCaptureMouse();
	}
	uintptr_t Gui::GetTextureID(Texture& texture)
	{
		return s_pIGui->GetTextureID(texture.GetInterfaceHandle());
	}



	// Wrappers:
	bool Gui::IsWindowFocused(emberCommon::GuiFocusedFlags flags)
	{
		return s_pIGui->IsWindowFocused(flags);
	}
	bool Gui::Begin(const char* name, bool* pOpen, emberCommon::GuiWindowFlags flags)
	{
		return s_pIGui->Begin(name, pOpen, flags);
	}
	void Gui::End()
	{
		s_pIGui->End();
	}
	void Gui::PushID(const char* str_id)
	{
		s_pIGui->PushID(str_id);
	}
	void Gui::PopID()
	{
		s_pIGui->PopID();
	}
	Float2 Gui::GetWindowSize()
	{
		return s_pIGui->GetWindowSize();
	}
	Float2 Gui::GetContentRegionalAvail()
	{
		return s_pIGui->GetContentRegionalAvail();
	}
	Float2 Gui::GetCursorPos()
	{
		return s_pIGui->GetCursorPos();
	}
	void Gui::SetCursorPos(const Float2& localPos)
	{
		s_pIGui->SetCursorPos(localPos);
	}
	void Gui::Image(uintptr_t textureID, float imageWidth, float imageHeight, Float2 uv0, Float2 uv1)
	{
		s_pIGui->Image(textureID, imageWidth, imageHeight, uv0.x, uv0.y, uv1.x, uv1.y);
	}
}