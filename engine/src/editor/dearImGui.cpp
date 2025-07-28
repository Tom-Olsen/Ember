#include "dearImGui.h"
#include "editor.h"
#include "logger.h"
#include "macros.h"
#include "profiler.h"
#include "renderPassManager.h"
#include "sampler.h"
#include "samplerManager.h"
#include "texture2d.h"
#include "vmaImage.h"
#include "vulkanContext.h"
#include "vulkanForwardRenderPass.h"
#include "vulkanMacros.h"
#include "vulkanPresentRenderPass.h"
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_vulkan.h>



namespace emberEngine
{
	using namespace vulkanBackend;



	// Static members:
	bool DearImGui::s_showDemoWindow = true;
	bool DearImGui::s_wantCaptureKeyboard = false;
	bool DearImGui::s_wantCaptureMouse = false;
	ImGuiIO* DearImGui::s_pIo = nullptr;
	VkDescriptorSetLayout DearImGui::s_descriptorSetLayout;
	std::unordered_map<Texture2d*, VkDescriptorSet> DearImGui::s_textureToDescriptorMap;



	// Initialization and cleanup:
	void DearImGui::Init()
	{
		RETURN_DISABLED();

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		s_pIo = &ImGui::GetIO();
		s_pIo->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		s_pIo->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		s_pIo->ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
		s_pIo->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
		s_pIo->FontGlobalScale = 2.0f;

		ImGui::StyleColorsDark();
		ImGui_ImplSDL3_InitForVulkan(Context::GetSDL_Window());

		ImGui_ImplVulkan_InitInfo initInfo = {};
		initInfo.Instance = Context::GetVkInstance();
		initInfo.PhysicalDevice = Context::GetVkPhysicalDevice();
		initInfo.Device = Context::GetVkDevice();
		initInfo.QueueFamily = Context::logicalDevice.GetGraphicsQueue().familyIndex;
		initInfo.Queue = Context::logicalDevice.GetGraphicsQueue().queue;
		initInfo.RenderPass = RenderPassManager::GetPresentRenderPass()->GetVkRenderPass();
		initInfo.DescriptorPoolSize = 2;	// DescriptorPoolSize > 0 means Imgui backend creates its own VkDescriptorPool. Need at least 1 + as many as additional calls done to ImGui_ImplVulkan_AddTexture()
		initInfo.MinImageCount = 2;
		initInfo.ImageCount = Context::swapchains[Context::swapchainIndex].GetImages().size();
		initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		ImGui_ImplVulkan_Init(&initInfo);

		CreateDescriptorSetLayout();

		// Upload fonts:
		ImGui_ImplVulkan_CreateFontsTexture();

		#ifdef LOG_INITIALIZATION
		LOG_TRACE("DearImGui initialized.");
		#endif
	}
	void DearImGui::Clear()
	{
		RETURN_DISABLED();
		s_textureToDescriptorMap.clear();
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplSDL3_Shutdown();
		vkDestroyDescriptorSetLayout(Context::GetVkDevice(), s_descriptorSetLayout, nullptr);
		ImGui::DestroyContext();
	}



	// Public methods:
	// Render Logic:
	void DearImGui::Update()
	{
		PROFILE_FUNCTION();
		RETURN_DISABLED();
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplSDL3_NewFrame();
		ImGui::NewFrame();
		{
			// Demo window:
			//if (s_showDemoWindow)
			//	ImGui::ShowDemoWindow(&s_showDemoWindow);

			// Render vulkanRenderer output into ImGui window:
			if (Context::renderToImGuiWindow)
				ShowDockSpace();

			// Render all editorWindows:
			Editor::Render();
		}
		ImGui::EndFrame();

		// Update additional platform windows:
		if (s_pIo->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			ImGui::UpdatePlatformWindows();
	}
	void DearImGui::ProcessEvent(const SDL_Event& event)
	{
		RETURN_DISABLED();
		ImGui_ImplSDL3_ProcessEvent(&event);
		s_wantCaptureKeyboard = s_pIo->WantCaptureKeyboard;
		s_wantCaptureMouse = s_pIo->WantCaptureMouse;
	}
	void DearImGui::Render(VkCommandBuffer& commandBuffer)
	{
		RETURN_DISABLED();
		ImGui::Render();

		ImDrawData* drawData = ImGui::GetDrawData();
		ImGui_ImplVulkan_RenderDrawData(drawData, commandBuffer);

		// Render additional platform windows:
		if (s_pIo->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
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
	ImTextureID DearImGui::GetTextureID(Texture2d* pTexture)
	{
		if (!pTexture)
			return static_cast<ImTextureID>(0);

		// Return cached descriptor set if it exists:
		auto it = s_textureToDescriptorMap.find(pTexture);
		if (it != s_textureToDescriptorMap.end())
			return reinterpret_cast<ImTextureID>(it->second);

		// Create descriptor set for this texture:
		VkDescriptorSet descriptorSet = CreateDescriptorSet();
		UpdateDescriptor(descriptorSet, pTexture->GetVmaImage()->GetVkImageView(), SamplerManager::GetSampler("colorSampler")->GetVkSampler());

		// Cache and return:
		s_textureToDescriptorMap[pTexture] = descriptorSet;
		return reinterpret_cast<ImTextureID>(descriptorSet);
	}



	// Helper functions:
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



	// Private methods:
	void DearImGui::ShowDockSpace()
	{
		static bool dockspaceOpen = true;
		static bool optionFullscreen = true;
		static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;

		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (optionFullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		// Set up the dockspace
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, windowFlags);
		ImGui::DockSpace(ImGui::GetID("MyDockspace"), ImVec2(0.0f, 0.0f), dockspaceFlags);
		ImGui::End();
	}
	void DearImGui::CreateDescriptorSetLayout()
	{
		VkDescriptorSetLayoutBinding binding = {};
		binding.binding = 0;
		binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		binding.descriptorCount = 1;
		binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutCreateInfo layoutInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
		layoutInfo.bindingCount = 1;
		layoutInfo.pBindings = &binding;

		VKA(vkCreateDescriptorSetLayout(Context::GetVkDevice(), &layoutInfo, nullptr, &s_descriptorSetLayout));
	}
	VkDescriptorSet DearImGui::CreateDescriptorSet()
	{
		VkDescriptorSetAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
		allocInfo.descriptorPool = Context::GetVkDescriptorPool();
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &s_descriptorSetLayout;

		VkDescriptorSet descriptorSet;
		VKA(vkAllocateDescriptorSets(Context::GetVkDevice(), &allocInfo, &descriptorSet));
		return descriptorSet;
	}
	void DearImGui::UpdateDescriptor(VkDescriptorSet descriptorSet, VkImageView imageView, VkSampler sampler)
	{
		VkDescriptorImageInfo imageInfo = {};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = imageView;
		imageInfo.sampler = sampler;

		VkWriteDescriptorSet writeDescriptor = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
		writeDescriptor.dstSet = descriptorSet;
		writeDescriptor.dstBinding = 0;
		writeDescriptor.dstArrayElement = 0;
		writeDescriptor.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writeDescriptor.descriptorCount = 1;
		writeDescriptor.pImageInfo = &imageInfo;

		vkUpdateDescriptorSets(Context::logicalDevice.GetVkDevice(), 1, &writeDescriptor, 0, nullptr);
	}
}