#include "dearImGui.h"
#include "renderPass.h"
#include "renderPassManager.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"
#include <imgui.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_vulkan.h>



// Macro to disable Dear ImGui:
#define DISABLE_DEAR_IMGUI
#ifdef DISABLE_DEAR_IMGUI
	#define RETURN_DISABLED() return
	#define RETURN_FALSE_DISABLED() return false
#else
	#define RETURN_DISABLED()
	#define RETURN_FALSE_DISABLED()
#endif



namespace emberEngine
{
	// Static members:
	bool DearImGui::s_showDemoWindow = true;
	bool DearImGui::s_wantCaptureKeyboard = false;
	bool DearImGui::s_wantCaptureMouse = false;
	ImGuiIO* DearImGui::s_io = nullptr;



	// Constructor/Destructor:
	void DearImGui::Init(VulkanContext* pContext)
	{
		RETURN_DISABLED();
		RenderPassManager::Init(pContext);

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		s_io = &ImGui::GetIO();
		s_io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		s_io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		s_io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
		s_io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
		s_io->FontGlobalScale = 2.0f;

		ImGui::StyleColorsDark();
		ImGui_ImplSDL3_InitForVulkan(pContext->GetSDL_Window());

		ImGui_ImplVulkan_InitInfo initInfo = {};
		initInfo.Instance = pContext->GetVkInstance();
		initInfo.PhysicalDevice = pContext->GetVkPhysicalDevice();
		initInfo.Device = pContext->GetVkDevice();
		initInfo.QueueFamily = pContext->pLogicalDevice->GetGraphicsQueue().familyIndex;
		initInfo.Queue = pContext->pLogicalDevice->GetGraphicsQueue().queue;
		initInfo.RenderPass = RenderPassManager::GetRenderPass("shadingRenderPass")->GetVkRenderPass();
		initInfo.DescriptorPoolSize = 2;	// DescriptorPoolSize > 0 means Imgui backend creates its own VkDescriptorPool. Need at least 1 + as many as additional calls done to ImGui_ImplVulkan_AddTexture()
		initInfo.MinImageCount = 2;
		initInfo.ImageCount = pContext->pSwapchain->GetImages().size();
		initInfo.MSAASamples = pContext->msaaSamples;
		ImGui_ImplVulkan_Init(&initInfo);

		// Upload fonts:
		ImGui_ImplVulkan_CreateFontsTexture();
	}
	void DearImGui::Clear()
	{
		RETURN_DISABLED();
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplSDL3_Shutdown();
		ImGui::DestroyContext();
	}



	// Public methods:

	// Must be called in main update loop of the engine.
	void DearImGui::Update()
	{
		RETURN_DISABLED();
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplSDL3_NewFrame();
		ImGui::NewFrame();

		if (s_showDemoWindow)
			ImGui::ShowDemoWindow(&s_showDemoWindow);

		ImGui::EndFrame();

		// Update additional platform windows:
		if (s_io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			ImGui::UpdatePlatformWindows();
	}
	// Must be called in SdlWindow::HandleEvents() before handing event over to event system.
	void DearImGui::ProcessEvent(const SDL_Event& event)
	{
		RETURN_DISABLED();
		ImGui_ImplSDL3_ProcessEvent(&event);
		s_wantCaptureKeyboard = s_io->WantCaptureKeyboard;
		s_wantCaptureMouse = s_io->WantCaptureMouse;
	}
	// Must be called in main render loop before vkCmdEndRenderPass(...).
	void DearImGui::Render(VkCommandBuffer& commandBuffer)
	{
		RETURN_DISABLED();
		ImGui::Render();

		ImDrawData* drawData = ImGui::GetDrawData();
		ImGui_ImplVulkan_RenderDrawData(drawData, commandBuffer);

		// Render additional platform windows:
		if (s_io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			ImGui::RenderPlatformWindowsDefault();
	}



	// Getters:
	bool DearImGui::WantCaptureKeyboard()
	{
		RETURN_FALSE_DISABLED();
		return s_wantCaptureKeyboard;
	}
	bool DearImGui::WantCaptureMouse()
	{
		RETURN_FALSE_DISABLED();
		return s_wantCaptureMouse;
	}


	// Helper function:
	bool IsExtensionAvailable(const std::vector<VkExtensionProperties>& properties, const char* extension)
	{
		for (const VkExtensionProperties& p : properties)
			if (strcmp(p.extensionName, extension) == 0)
				return true;
		return false;
	}
	void DearImGui::AddImGuiInstanceExtensions(std::vector<const char*>& instanceExtensions)
	{
		RETURN_DISABLED();
		// Enumerate available extensions:
		uint32_t propertiesCount;
		std::vector<VkExtensionProperties> properties;
		VKA(vkEnumerateInstanceExtensionProperties(nullptr, &propertiesCount, nullptr));
		properties.resize(propertiesCount);
		VKA(vkEnumerateInstanceExtensionProperties(nullptr, &propertiesCount, properties.data()));

		// Enable required extensions:
		if (IsExtensionAvailable(properties, VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME))
			instanceExtensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
		#ifdef VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME
		if (IsExtensionAvailable(properties, VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME))
			instanceExtensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
		#endif
	}
}