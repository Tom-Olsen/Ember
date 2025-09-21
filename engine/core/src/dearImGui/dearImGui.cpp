#include "dearImGui.h"
#include "iDearImGui.h"
#include "logger.h"
#include "macros.h"
#include "nullDearImGui.h"
#include "renderPassManager.h"
#include "sampler.h"
#include "sdlDearImGui.h"
#include "texture2d.h"
#include "vmaImage.h"
#include "vulkanContext.h"
#include "vulkanPresentRenderPass.h"
#include "window.h"
#include <tuple>



namespace emberEngine
{
	using namespace vulkanBackend;



	// Static members:
	std::unique_ptr<emberBackendInterface::IDearImGui> DearImGui::s_pIDearImGui;



	// Initialization/Ceanup:
	void DearImGui::Init(void* pSdlWindow, bool enableDockSpace)
	{
		VkInstance vkInstance = Context::GetVkInstance();
		VkPhysicalDevice vkPhysicalDevice = Context::GetVkPhysicalDevice();
		VkDevice vkDevice = Context::GetVkDevice();
		VkRenderPass vkRenderPass = RenderPassManager::GetPresentRenderPass()->GetVkRenderPass();
		VkDescriptorPool vkDescriptorPool = Context::GetVkDescriptorPool();
		VkQueue vkQueue = Context::GetLogicalDevice()->GetGraphicsQueue().queue;
		uint32_t queueFamilyIndex = Context::GetLogicalDevice()->GetGraphicsQueue().familyIndex;
		uint32_t framesInFlight = Context::GetFramesInFlight();
		uint32_t spwachainImageCount = Context::swapchains[0].GetImages().size();

		if (true)
			s_pIDearImGui = std::make_unique<sdlWindowBackend::SdlDearImGui>(pSdlWindow, vkInstance, vkPhysicalDevice, vkDevice, vkRenderPass, vkDescriptorPool, vkQueue, queueFamilyIndex, framesInFlight, spwachainImageCount, enableDockSpace);
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
	emberBackendInterface::IDearImGui* DearImGui::GetInterfaceHandle()
	{
		return s_pIDearImGui.get();
	}
	bool DearImGui::WantCaptureKeyboard()
	{
		return s_pIDearImGui->WantCaptureKeyboard();
	}
	bool DearImGui::WantCaptureMouse()
	{
		return s_pIDearImGui->WantCaptureMouse();
	}
	uintptr_t DearImGui::GetTextureID(Texture2d* pTexture2d, Sampler* pSampler)
	{
		return s_pIDearImGui->GetTextureID(pTexture2d->GetVmaImage()->GetVkImageView(), pSampler->GetVkSampler());
	}



	// Helper functions:
	void DearImGui::AddDearImGuiInstanceExtensions(std::vector<const char*>& instanceExtensions)
	{
		std::unique_ptr<emberBackendInterface::IDearImGuiInstanceExtensionsLoader> pInstanceExtensionLoader;
		if (true)
			pInstanceExtensionLoader = std::make_unique<sdlWindowBackend::SdlDearImGuiInstanceExtensionsLoader>();
		else
			pInstanceExtensionLoader = std::make_unique <nullWindowBackend::NullDearImGuiInstanceExtensionsLoader>();
		pInstanceExtensionLoader->AddExtensions(instanceExtensions);
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