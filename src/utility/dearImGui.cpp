#include "dearImGui.h"
#include "renderPass.h"
#include "renderPassManager.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"
#include <imgui.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_vulkan.h>



// Static members:
bool DearImGui::s_showDemoWindow = true;
bool DearImGui::s_wantCaptureKeyboard = false;
bool DearImGui::s_wantCaptureMouse = false;
ImGuiIO* DearImGui::s_io = nullptr;



// Constructor/Destructor:
void DearImGui::Init(VulkanContext* pContext)
{
	RenderPassManager::Init(pContext);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	s_io = &ImGui::GetIO();
	s_io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	s_io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	s_io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	s_io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

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
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();
}



// Public methods:
void DearImGui::Update()
{
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();

	if (s_showDemoWindow)
		ImGui::ShowDemoWindow(&s_showDemoWindow);

	ImGui::EndFrame();

	// Update additional Platform Windows (needed for docking feature)
	if (s_io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		ImGui::UpdatePlatformWindows();
}
void DearImGui::ProcessEvent(const SDL_Event& event)
{
	ImGui_ImplSDL3_ProcessEvent(&event);
	s_wantCaptureKeyboard = s_io->WantCaptureKeyboard;
	s_wantCaptureMouse = s_io->WantCaptureMouse;
}
void DearImGui::Render(VkCommandBuffer& commandBuffer)
{
	ImGui::Render();

	ImDrawData* drawData = ImGui::GetDrawData();
	ImGui_ImplVulkan_RenderDrawData(drawData, commandBuffer);

	// Render additional Platform Windows (needed for docking feature)
	if (s_io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		ImGui::RenderPlatformWindowsDefault();
}
bool DearImGui::WantCaptureKeyboard()
{
	return s_wantCaptureKeyboard;
}
bool DearImGui::WantCaptureMouse()
{
	return s_wantCaptureMouse;
}


bool IsExtensionAvailable(const std::vector<VkExtensionProperties>& properties, const char* extension)
{
	for (const VkExtensionProperties& p : properties)
		if (strcmp(p.extensionName, extension) == 0)
			return true;
	return false;
}
void DearImGui::AddImGuiInstanceExtensions(std::vector<const char*>& instanceExtensions)
{
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