#include "imGuiSdlVulkan.h"
#include "iWindow.h"
#include "iRenderer.h"
#include "iTexture.h"
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>
#include <vulkan/vulkan.h>



namespace imGuiSdlVulkanBackend
{
	// Public methods:
	// Constructor/Destructor:
	Gui::Gui(emberBackendInterface::IWindow* pIWindow, emberBackendInterface::IRenderer* pIRenderer, bool enableDockSpace)
	{
		m_vkDevice = static_cast<VkDevice>(pIRenderer->GetVkDevice());
		m_vkDescriptorPool = static_cast<VkDescriptorPool>(pIRenderer->GetVkDescriptorPool());
		m_vkColorSampler = static_cast<VkSampler>(pIRenderer->GetColorSampler());
		m_wantCaptureKeyboard = false;
		m_wantCaptureMouse = false;
		m_enableDockSpace = enableDockSpace;

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		m_pIo = &ImGui::GetIO();
		m_pIo->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		m_pIo->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		m_pIo->ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
		m_pIo->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
		m_pIo->FontGlobalScale = 2.0f;

		ImGui::StyleColorsDark();
		ImGui_ImplSDL3_InitForVulkan(static_cast<SDL_Window*>(pIWindow->GetNativeHandle()));

		ImGui_ImplVulkan_InitInfo initInfo = {};
		initInfo.Instance = static_cast<VkInstance>(pIRenderer->GetVkInstance());
		initInfo.PhysicalDevice = static_cast<VkPhysicalDevice>(pIRenderer->GetVkPhysicalDevice());
		initInfo.Device = m_vkDevice;
		initInfo.Queue = static_cast<VkQueue>(pIRenderer->GetGraphicsVkQueue());
		initInfo.QueueFamily = pIRenderer->GetGraphicsVkQueueFamilyIndex();
		initInfo.RenderPass = static_cast<VkRenderPass>(pIRenderer->GetPresentVkRenderPass());
		initInfo.DescriptorPoolSize = 8 * pIRenderer->GetFramesInFlight();	// ImGui needs at least 8 descriptor sets per frame. If you use more than 8 textures in a single frame, increase this value.
		initInfo.MinImageCount = 2;
		initInfo.ImageCount = pIRenderer->GetSwapchainImageCount();
		initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;	// same as renderer present pass, which is hardcoded to 1.
		ImGui_ImplVulkan_Init(&initInfo);

		CreateDescriptorSetLayout();
	}
	Gui::~Gui()
	{
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplSDL3_Shutdown();
		if (m_descriptorSetLayout != VK_NULL_HANDLE)
			vkDestroyDescriptorSetLayout(m_vkDevice, m_descriptorSetLayout, nullptr);
		ImGui::DestroyContext();
		m_vkImageViewToDescriptorMap.clear();
	}



	// Move semantics:
	Gui::Gui(Gui&& other) noexcept
	{
		// Transfer resources: other->this
		m_vkDevice = other.m_vkDevice;
		m_vkDescriptorPool = other.m_vkDescriptorPool;
		m_descriptorSetLayout = other.m_descriptorSetLayout;
		m_pIo = other.m_pIo;
		m_wantCaptureKeyboard = other.m_wantCaptureKeyboard;
		m_wantCaptureMouse = other.m_wantCaptureMouse;
		m_enableDockSpace = other.m_enableDockSpace;
		m_vkImageViewToDescriptorMap = other.m_vkImageViewToDescriptorMap;

		// Invalidate other:
		other.m_vkDevice = VK_NULL_HANDLE;
		other.m_vkDescriptorPool = VK_NULL_HANDLE;
		other.m_descriptorSetLayout = VK_NULL_HANDLE;
		other.m_pIo = nullptr;
		other.m_wantCaptureKeyboard = false;
		other.m_wantCaptureMouse = false;
		other.m_enableDockSpace = false;
		other.m_vkImageViewToDescriptorMap.clear();
	}
	Gui& Gui::operator=(Gui&& other) noexcept
	{
		if (this != &other)
		{
			// Release own resources:
			ImGui_ImplVulkan_Shutdown();
			ImGui_ImplSDL3_Shutdown();
			if (m_descriptorSetLayout != VK_NULL_HANDLE)
				vkDestroyDescriptorSetLayout(m_vkDevice, m_descriptorSetLayout, nullptr);
			ImGui::DestroyContext();
			m_vkImageViewToDescriptorMap.clear();

			// Transfer resources: other->this
			m_vkDevice = other.m_vkDevice;
			m_vkDescriptorPool = other.m_vkDescriptorPool;
			m_descriptorSetLayout = other.m_descriptorSetLayout;
			m_pIo = other.m_pIo;
			m_wantCaptureKeyboard = other.m_wantCaptureKeyboard;
			m_wantCaptureMouse = other.m_wantCaptureMouse;
			m_enableDockSpace = other.m_enableDockSpace;
			m_vkImageViewToDescriptorMap = other.m_vkImageViewToDescriptorMap;
		}
		return *this;
	}



	// Public methods:
	void Gui::Update()
	{
		//PROFILE_FUNCTION();
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplSDL3_NewFrame();
		ImGui::NewFrame();
		{
			bool showDemoWindow;
			ImGui::ShowDemoWindow(&showDemoWindow);

			// Enable docking in SDL window:
			if (m_enableDockSpace)
				ShowDockSpace();

			// Render all editorWindows:
			// m_pIEditor->Render();
		}
		ImGui::EndFrame();

		// Update additional platform windows:
		if (m_pIo->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			ImGui::UpdatePlatformWindows();
	}
	void Gui::ProcessEvent(const void* pWindowEvent)
	{
		ImGui_ImplSDL3_ProcessEvent(static_cast<const SDL_Event*>(pWindowEvent));

		// bool wantCaptureEvents = false;
		// void* focusedWindow = m_pIEditor->GetFocusedWindow();
		// if (focusedWindow != nullptr)
		// 	wantCaptureEvents = m_pIEditor->GetWindowWantCaptureEvents();
		
		// m_wantCaptureKeyboard = wantCaptureEvents;
		// m_wantCaptureMouse = wantCaptureEvents;
	}
	void Gui::Render(VkCommandBuffer vkCommandBuffer)
	{
		ImGui::Render();

		ImDrawData* drawData = ImGui::GetDrawData();
		ImGui_ImplVulkan_RenderDrawData(drawData, vkCommandBuffer);

		// Render additional platform windows:
		if (m_pIo->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			ImGui::RenderPlatformWindowsDefault();
	}



	// Getters:
	bool Gui::WantCaptureKeyboard()
	{
		return m_wantCaptureKeyboard;
	}
	bool Gui::WantCaptureMouse()
	{
		return m_wantCaptureMouse;
	}
	uintptr_t Gui::GetTextureID(emberBackendInterface::ITexture* pITexture)
	{
		if (!pITexture)
			return 0;

		VkImageView imageView = pITexture->GetVkImageView();

		// Return cached descriptor set if it exists:
		auto it = m_vkImageViewToDescriptorMap.find(imageView);
		if (it != m_vkImageViewToDescriptorMap.end())
			return reinterpret_cast<ImTextureID>(it->second);

		// Create descriptor set for this vkImageView:
		VkDescriptorSet vkDescriptorSet = CreateDescriptorSet();
		UpdateDescriptor(vkDescriptorSet, imageView, m_vkColorSampler);

		// Cache and return:
		m_vkImageViewToDescriptorMap[imageView] = vkDescriptorSet;
		return reinterpret_cast<uintptr_t>(vkDescriptorSet);
	}



	// Wrappers:
	bool Gui::IsWindowFocused(emberCommon::GuiFocusedFlags flags)
	{
		return ImGui::IsWindowFocused(flags);
	}
	bool Gui::Begin(const char* name, bool* pOpen, emberCommon::GuiWindowFlags flags)
	{
		return ImGui::Begin(name, pOpen, flags);
	}
	void Gui::End()
	{
		ImGui::End();
	}
	void Gui::PushID(const char* str_id)
	{
		ImGui::PushID(str_id);
	}
	void Gui::PopID()
	{
		ImGui::PopID();
	}
	Float2 Gui::GetWindowSize()
	{
		ImVec2 size = ImGui::GetWindowSize();
		return Float2{ size.x, size.y };
	}
	Float2 Gui::GetContentRegionalAvail()
	{
		ImVec2 regionAvail = ImGui::GetContentRegionAvail();
		return Float2{ regionAvail.x, regionAvail.y };
	}
	Float2 Gui::GetCursorPos()
	{
		ImVec2 cursorPos = ImGui::GetCursorPos();
		return Float2{ cursorPos.x, cursorPos.y };
	}
	void Gui::SetCursorPos(const Float2& localPos)
	{
		ImGui::SetCursorPos(ImVec2(localPos.x, localPos.y));
	}
	void Gui::Image(uintptr_t textureID, float imageWidth, float imageHeight, float u0, float v0, float u1, float v1)
	{
		ImGui::Image(static_cast<ImTextureID>(textureID), ImVec2(imageWidth, imageHeight), ImVec2(u0, v0), ImVec2(u1, v1));
	}



	// Private methods:
	void Gui::ShowDockSpace()
	{
		static bool dockspaceOpen = true;
		static bool optionFullscreen = true;
		static ImGuiDockNodeFlags dockspaceFlags = emberCommon::GuiWindowFlag_None;

		ImGuiWindowFlags windowFlags = emberCommon::GuiWindowFlag_MenuBar | emberCommon::GuiWindowFlag_NoDocking;
		if (optionFullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			windowFlags |= emberCommon::GuiWindowFlag_NoTitleBar | emberCommon::GuiWindowFlag_NoResize | emberCommon::GuiWindowFlag_NoMove;
			windowFlags |= emberCommon::GuiWindowFlag_NoBringToFrontOnFocus | emberCommon::GuiWindowFlag_NoNavFocus;
		}

		// Set up the dockspace
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, windowFlags);
		ImGui::DockSpace(ImGui::GetID("MyDockspace"), ImVec2(0.0f, 0.0f), dockspaceFlags);
		ImGui::End();
	}
	void Gui::CreateDescriptorSetLayout()
	{
		VkDescriptorSetLayoutBinding binding = {};
		binding.binding = 0;
		binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		binding.descriptorCount = 1;
		binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutCreateInfo layoutInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
		layoutInfo.bindingCount = 1;
		layoutInfo.pBindings = &binding;

		if (vkCreateDescriptorSetLayout(m_vkDevice, &layoutInfo, nullptr, &m_descriptorSetLayout) != VK_SUCCESS)
			throw std::runtime_error((std::string)"DearImGui::CreateDescriptorSetLayout: " + (std::string)"failed to create descriptor set layout!");
	}
	VkDescriptorSet Gui::CreateDescriptorSet()
	{
		VkDescriptorSetAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
		allocInfo.descriptorPool = m_vkDescriptorPool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &m_descriptorSetLayout;

		VkDescriptorSet descriptorSet;
		if (vkAllocateDescriptorSets(m_vkDevice, &allocInfo, &descriptorSet) != VK_SUCCESS)
			throw std::runtime_error((std::string)"DearImGui::CreateDescriptorSet: " + (std::string)"failed to allocate descriptor sets!");
		return descriptorSet;
	}
	void Gui::UpdateDescriptor(VkDescriptorSet vkDescriptorSet, VkImageView vkImageView, VkSampler vkSampler)
	{
		VkDescriptorImageInfo imageInfo = {};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = vkImageView;
		imageInfo.sampler = vkSampler;

		VkWriteDescriptorSet writeDescriptor = { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
		writeDescriptor.dstSet = vkDescriptorSet;
		writeDescriptor.dstBinding = 0;
		writeDescriptor.dstArrayElement = 0;
		writeDescriptor.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writeDescriptor.descriptorCount = 1;
		writeDescriptor.pImageInfo = &imageInfo;

		vkUpdateDescriptorSets(m_vkDevice, 1, &writeDescriptor, 0, nullptr);
	}
}