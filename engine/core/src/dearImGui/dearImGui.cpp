#include "dearImGui.h"
#include "iDearImGui.h"
#include "iTexture.h"
#include "logger.h"
#include "macros.h"
#include "nullDearImGui.h"
#include "renderer.h"
#include "sdlDearImGui.h"
#include "texture2d.h"
#include "vmaImage.h"
#include "vulkanContext.h"
#include "vulkanPresentRenderPass.h"
#include "window.h"
#include <tuple>



namespace emberEngine
{
	// Static members:
	bool DearImGui::s_isInitialized = false;
	std::unique_ptr<emberBackendInterface::IDearImGui> DearImGui::s_pIDearImGui;
	emberBackendInterface::IDearImGui* DearImGui::GetInterfaceHandle()
	{
		return s_pIDearImGui.get();
	}



	// Initialization/Cleanup:
	void DearImGui::Init(bool enableDockSpace)
	{
		if (s_isInitialized)
			return;
		s_isInitialized = true;

		if (true)
			s_pIDearImGui = std::make_unique<sdlWindowBackend::SdlDearImGui>(Window::GetInterfaceHandle(), Renderer::GetInterfaceHandle(), enableDockSpace);
		else
			s_pIDearImGui = std::make_unique<nullWindowBackend::NullDearImGui>();

		Window::LinkDearImGui(s_pIDearImGui.get());
		#ifdef LOG_INITIALIZATION
		LOG_TRACE("DearImGui initialized.");
		#endif
	}
	void DearImGui::Clear()
	{
		s_pIDearImGui.reset();
		s_isInitialized = false;
	}



	// Render Logic:
	void DearImGui::Update()
	{
		s_pIDearImGui->Update();
	}
	void DearImGui::ProcessEvent(const void* pEvent)
	{
		s_pIDearImGui->Update();
	}
	void DearImGui::Render(VkCommandBuffer vkCommandBuffer)
	{
		s_pIDearImGui->Render(vkCommandBuffer);
	}



	// Getters:
	bool DearImGui::WantCaptureKeyboard()
	{
		return s_pIDearImGui->WantCaptureKeyboard();
	}
	bool DearImGui::WantCaptureMouse()
	{
		return s_pIDearImGui->WantCaptureMouse();
	}
	uintptr_t DearImGui::GetTextureID(Texture* pTexture)
	{
		return s_pIDearImGui->GetTextureID(pTexture->GetInterfaceHandle()->GetVkImageView());
	}



	// Wrappers:
	bool DearImGui::IsWindowFocused(emberEngine::DearImGuiFocusedFlags flags)
	{
		return s_pIDearImGui->IsWindowFocused(flags);
	}
	bool DearImGui::Begin(const char* name, bool* pOpen, emberEngine::DearImGuiWindowFlags flags)
	{
		return s_pIDearImGui->Begin(name, pOpen, flags);
	}
	void DearImGui::End()
	{
		s_pIDearImGui->End();
	}
	void DearImGui::PushID(const char* str_id)
	{
		s_pIDearImGui->PushID(str_id);
	}
	void DearImGui::PopID()
	{
		s_pIDearImGui->PopID();
	}
	Float2 DearImGui::GetWindowSize()
	{
		Float2 size = s_pIDearImGui->GetWindowSize();
		return Float2(size[0], size[1]);
	}
	Float2 DearImGui::GetContentRegionalAvail()
	{
		Float2 avail = s_pIDearImGui->GetContentRegionalAvail();
		return Float2(avail[0], avail[1]);
	}
	Float2 DearImGui::GetCursorPos()
	{
		Float2 cursorPos = s_pIDearImGui->GetCursorPos();
		return Float2(cursorPos[0], cursorPos[1]);
	}
	void DearImGui::SetCursorPos(float localPosX, float localPosY)
	{
		s_pIDearImGui->SetCursorPos(localPosX, localPosY);
	}
	void DearImGui::Image(uintptr_t textureID, float imageWidth, float imageHeight, Float2 uv0, Float2 uv1)
	{
		s_pIDearImGui->Image(textureID, imageWidth, imageHeight, uv0.x, uv0.y, uv1.x, uv1.y);
	}
}