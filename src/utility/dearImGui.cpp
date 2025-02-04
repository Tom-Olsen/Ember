#include "dearImGui.h"
#include "renderPass.h"
#include "renderPassManager.h"
#include "vulkanContext.h"
#include <imgui.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_vulkan.h>



// Static members:
bool DearImGui::s_isEnabled = true;
bool DearImGui::s_showDemoWindow = true;
bool DearImGui::s_wantCaptureKeyboard = false;
bool DearImGui::s_wantCaptureMouse = false;



// Constructor/Destructor:
void DearImGui::Init(VulkanContext* pContext)
{
	if (s_isEnabled == false)
		return;

	RenderPassManager::Init(pContext);

	ImGui::CreateContext();
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
	if (s_isEnabled == false)
		return;
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();
}



// Public methods:
void DearImGui::Update()
{
	if (s_isEnabled == false)
		return;
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();

	if (s_showDemoWindow)
		ImGui::ShowDemoWindow(&s_showDemoWindow);

	ImGui::EndFrame();
}
void DearImGui::ProcessEvent(const SDL_Event& event)
{
	if (s_isEnabled == false)
		return;
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplSDL3_ProcessEvent(&event);

	s_wantCaptureKeyboard = io.WantCaptureKeyboard;
	s_wantCaptureMouse = io.WantCaptureMouse;
}
void DearImGui::Render(VkCommandBuffer& commandBuffer)
{
	if (s_isEnabled == false)
		return;
	ImGui::Render();
	ImDrawData* drawData = ImGui::GetDrawData();
	ImGui_ImplVulkan_RenderDrawData(drawData, commandBuffer);
}
bool DearImGui::WantCaptureKeyboard()
{
	if (s_isEnabled == false)
		return false;
	return s_wantCaptureKeyboard;
}
bool DearImGui::WantCaptureMouse()
{
	if (s_isEnabled == false)
		return false;
	return s_wantCaptureMouse;
}