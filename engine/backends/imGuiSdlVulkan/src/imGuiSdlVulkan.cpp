#include "imGuiSdlVulkan.h"
#include "iRenderer.h"
#include "iTexture.h"
#include "iWindow.h"
#include "imGuiConvertGuiFlags.h"
#include "imGuiConvertGuiStyle.h"
#include <SDL3/SDL.h>
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
		m_pSdlWindow = static_cast<SDL_Window*>(pIWindow->GetNativeHandle());
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
		ImGui_ImplSDL3_InitForVulkan(m_pSdlWindow);
		ImGui_ImplSDL3_SetMouseCaptureMode(ImGui_ImplSDL3_MouseCaptureMode_Enabled);

		ImGui_ImplVulkan_InitInfo initInfo = {};
		initInfo.Instance = static_cast<VkInstance>(pIRenderer->GetVkInstance());
		initInfo.PhysicalDevice = static_cast<VkPhysicalDevice>(pIRenderer->GetVkPhysicalDevice());
		initInfo.Device = m_vkDevice;
		initInfo.Queue = static_cast<VkQueue>(pIRenderer->GetGraphicsVkQueue());
		initInfo.QueueFamily = pIRenderer->GetGraphicsVkQueueFamilyIndex();
		initInfo.PipelineInfoMain.RenderPass = static_cast<VkRenderPass>(pIRenderer->GetPresentVkRenderPass());
		initInfo.DescriptorPoolSize = 8 * pIRenderer->GetFramesInFlight();	// ImGui needs at least 8 descriptor sets per frame. If you use more than 8 textures in a single frame, increase this value.
		initInfo.MinImageCount = 2;
		initInfo.ImageCount = pIRenderer->GetSwapchainImageCount();
		initInfo.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;      // same as renderer present pass, which is hardcoded to 1.
		#ifdef VALIDATION_LAYERS_ACTIVE
		initInfo.MinAllocationSize = 1024 * 1024;
		#endif
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
		m_pSdlWindow = other.m_pSdlWindow;
		m_wantCaptureKeyboard = other.m_wantCaptureKeyboard;
		m_wantCaptureMouse = other.m_wantCaptureMouse;
		m_enableDockSpace = other.m_enableDockSpace;
		m_vkImageViewToDescriptorMap = other.m_vkImageViewToDescriptorMap;
		m_focusedWindowWantCaptureEventsCallback = other.m_focusedWindowWantCaptureEventsCallback;
		m_hoveredWindowWantCaptureEventsCallback = other.m_hoveredWindowWantCaptureEventsCallback;

		// Invalidate other:
		other.m_vkDevice = VK_NULL_HANDLE;
		other.m_vkDescriptorPool = VK_NULL_HANDLE;
		other.m_descriptorSetLayout = VK_NULL_HANDLE;
		other.m_pIo = nullptr;
		other.m_pSdlWindow = nullptr;
		other.m_wantCaptureKeyboard = false;
		other.m_wantCaptureMouse = false;
		other.m_enableDockSpace = false;
		other.m_vkImageViewToDescriptorMap.clear();
		other.m_focusedWindowWantCaptureEventsCallback = nullptr;
		other.m_hoveredWindowWantCaptureEventsCallback = nullptr;
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
			m_pSdlWindow = other.m_pSdlWindow;
			m_wantCaptureKeyboard = other.m_wantCaptureKeyboard;
			m_wantCaptureMouse = other.m_wantCaptureMouse;
			m_enableDockSpace = other.m_enableDockSpace;
			m_vkImageViewToDescriptorMap = other.m_vkImageViewToDescriptorMap;
			m_focusedWindowWantCaptureEventsCallback = other.m_focusedWindowWantCaptureEventsCallback;
			m_hoveredWindowWantCaptureEventsCallback = other.m_hoveredWindowWantCaptureEventsCallback;
		}
		return *this;
	}



	// Render Logic:
	void Gui::Update()
	{
		//PROFILE_FUNCTION();
		ImGui_ImplVulkan_NewFrame();
		ReleaseStaleMouseButtons();
		ImGui_ImplSDL3_NewFrame();
		ImGui::NewFrame();
		{
			//bool showDemoWindow;
			//ImGui::ShowDemoWindow(&showDemoWindow);

			// Enable docking in SDL window:
			if (m_enableDockSpace)
				ShowDockSpace();

			// Render all editorWindows:
			if (m_renderEditorCallback)
				m_renderEditorCallback();
		}
		ImGui::EndFrame();

		// Update additional platform windows:
		if (m_pIo->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			ImGui::UpdatePlatformWindows();
	}
	void Gui::ProcessEvent(const void* pWindowEvent)
	{
		ImGui_ImplSDL3_ProcessEvent(static_cast<const SDL_Event*>(pWindowEvent));
		bool focusedWindowWantsCaptureEvents = m_focusedWindowWantCaptureEventsCallback ? m_focusedWindowWantCaptureEventsCallback() : false;
		bool hoveredWindowWantsCaptureEvents = m_hoveredWindowWantCaptureEventsCallback ? m_hoveredWindowWantCaptureEventsCallback() : false;
		m_wantCaptureKeyboard = m_pIo->WantCaptureKeyboard && focusedWindowWantsCaptureEvents;
		m_wantCaptureMouse = m_pIo->WantCaptureMouse && hoveredWindowWantsCaptureEvents;
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
	Float2 Gui::GetWindowSize()
	{
		ImVec2 windowSize = ImGui::GetWindowSize();
		return Float2{ windowSize.x, windowSize.y };
	}
	Float2 Gui::GetContentRegionAvail()
	{
		ImVec2 regionAvail = ImGui::GetContentRegionAvail();
		return Float2{ regionAvail.x, regionAvail.y };
	}
	Float2 Gui::GetCursorPos()
	{
		ImVec2 cursorPos = ImGui::GetCursorPos();
		return Float2{ cursorPos.x, cursorPos.y };
	}
	Float2 Gui::GetCursorScreenPos()
	{
		ImVec2 cursorScreenPos = ImGui::GetCursorScreenPos();
		return Float2{ cursorScreenPos.x, cursorScreenPos.y };
	}
	Float2 Gui::GetMouseDragDelta(emberCommon::GuiMouseButton button, float lockThreshold)
	{
		ImVec2 mouseDragDelta = ImGui::GetMouseDragDelta(GuiMouseButtonCommonToImGui(button), lockThreshold);
		return Float2{ mouseDragDelta.x, mouseDragDelta.y };
	}
	emberCommon::GuiStyle Gui::GetStyle() const
	{
		return GuiStyleImGuiToCommon(ImGui::GetStyle());
	}



	// Setters:
	void Gui::SetEditorCallbacks(emberBackendInterface::EditorRenderCallback renderEditorCallback, emberBackendInterface::EditorCaptureQueryCallback focusedWindowWantCaptureEventsCallback, emberBackendInterface::EditorCaptureQueryCallback hoveredWindowWantCaptureEventsCallback)
	{
		m_renderEditorCallback = renderEditorCallback;
		m_focusedWindowWantCaptureEventsCallback = focusedWindowWantCaptureEventsCallback;
		m_hoveredWindowWantCaptureEventsCallback = hoveredWindowWantCaptureEventsCallback;
	}
	void Gui::SetCursorPos(const Float2& localPos)
	{
		ImGui::SetCursorPos(ImVec2{ localPos.x, localPos.y });
	}
	void Gui::SetCursorScreenPos(const Float2& pos)
	{
		ImGui::SetCursorScreenPos(ImVec2{ pos.x, pos.y });
	}
	void Gui::ResetMouseDragDelta(emberCommon::GuiMouseButton button)
	{
		ImGui::ResetMouseDragDelta(GuiMouseButtonCommonToImGui(button));
	}



	// Window management:
	bool Gui::Begin(const char* name, bool* pOpen, emberCommon::GuiWindowFlags flags)
	{
		return ImGui::Begin(name, pOpen, GuiWindowFlagsCommonToImGui(flags));
	}
	void Gui::End()
	{
		ImGui::End();
	}
	void Gui::PushID(const char* strID)
	{
		ImGui::PushID(strID);
	}
	void Gui::PopID()
	{
		ImGui::PopID();
	}
	bool Gui::IsWindowFocused(emberCommon::GuiFocusedFlags flags)
	{
		return ImGui::IsWindowFocused(GuiFocusedFlagsCommonToImGui(flags));
	}
	bool Gui::IsWindowHovered(emberCommon::GuiHoveredFlags flags)
	{
		return ImGui::IsWindowHovered(GuiHoveredFlagsCommonToImGui(flags));
	}



	// Layout:
	void Gui::SameLine(float offsetFromStartX, float spacingW)
	{
		ImGui::SameLine(offsetFromStartX, spacingW);
	}
	void Gui::SetNextItemWidth(float itemWidth)
	{
		ImGui::SetNextItemWidth(itemWidth);
	}
	Float2 Gui::CalcTextSize(const char* text, const char* textEnd, bool hideTextAfterDoubleHash, float wrapWidth)
	{
		ImVec2 textSizie = ImGui::CalcTextSize(text, textEnd, hideTextAfterDoubleHash, wrapWidth);
		return Float2{ textSizie.x, textSizie.y };
	}



	// State checks:
	bool Gui::IsItemActive()
	{
		return ImGui::IsItemActive();
	}
	bool Gui::IsItemActivated()
	{
		return ImGui::IsItemActivated();
	}
	bool Gui::IsMouseDragging(emberCommon::GuiMouseButton button, float lockThreshold)
	{
		return ImGui::IsMouseDragging(GuiMouseButtonCommonToImGui(button), lockThreshold);
	}



	// Widgets:
	bool Gui::Checkbox(const char* label, bool* value)
	{
		return ImGui::Checkbox(label, value);
	}
	bool Gui::InputInt(const char* label, int* value, int step, int stepFast, emberCommon::GuiInputTextFlags flags)
	{
		return ImGui::InputInt(label, value, step, stepFast, GuiInputTextFlagsCommonToImGui(flags));
	}
	bool Gui::InputFloat(const char* label, float* value, float step, float stepFast, const char* format, emberCommon::GuiInputTextFlags flags)
	{
		return ImGui::InputFloat(label, value, step, stepFast, format, GuiInputTextFlagsCommonToImGui(flags));
	}
	void Gui::TextUnformatted(const char* text, const char* textEnd)
	{
		return ImGui::TextUnformatted(text, textEnd);
	}
	void Gui::TextV(const char* format, va_list args)
	{
		return ImGui::TextV(format, args);
	}
	bool Gui::Button(const char* label, const Float2& size)
	{
		return ImGui::Button(label, ImVec2{ size.x, size.y });
	}
	bool Gui::InvisibleButton(const char* strId, const Float2& size, emberCommon::GuiButtonFlags flags)
	{
		return ImGui::InvisibleButton(strId, ImVec2{ size.x, size.y }, GuiButtonFlagsCommonToImGui(flags));
	}
	bool Gui::Selectable(const char* label, bool selected)
	{
		return ImGui::Selectable(label, selected);
	}
	void Gui::Image(uintptr_t textureID, const Float2& imageSize, const Float2& uv0, const Float2& uv1)
	{
		ImGui::Image(static_cast<ImTextureID>(textureID), ImVec2{ imageSize.x, imageSize.y }, ImVec2{ uv0.x, uv0.y }, ImVec2{ uv1.x, uv1.y });
	}



	// Private methods:
	void Gui::ReleaseStaleMouseButtons()
	{
		if (m_pIo == nullptr || m_pSdlWindow == nullptr)
			return;

		float mouseX = 0.0f;
		float mouseY = 0.0f;
		SDL_MouseButtonFlags mouseButtons = SDL_GetMouseState(&mouseX, &mouseY);
		struct MouseButtonMapping
		{
			int imguiButton;
			SDL_MouseButtonFlags sdlMask;
			Uint8 sdlButton;
		};
		const MouseButtonMapping mouseButtonMappings[] =
		{
			{ ImGuiMouseButton_Left, SDL_BUTTON_LMASK, SDL_BUTTON_LEFT },
			{ ImGuiMouseButton_Right, SDL_BUTTON_RMASK, SDL_BUTTON_RIGHT },
			{ ImGuiMouseButton_Middle, SDL_BUTTON_MMASK, SDL_BUTTON_MIDDLE },
			{ 3, SDL_BUTTON_X1MASK, SDL_BUTTON_X1 },
			{ 4, SDL_BUTTON_X2MASK, SDL_BUTTON_X2 }
		};

		SDL_Window* pMouseWindow = SDL_GetMouseFocus();
		SDL_WindowID windowID = (pMouseWindow == nullptr) ? SDL_GetWindowID(m_pSdlWindow) : SDL_GetWindowID(pMouseWindow);
		if (windowID == 0)
			return;

		for (const MouseButtonMapping& mapping : mouseButtonMappings)
		{
			if (!m_pIo->MouseDown[mapping.imguiButton] || (mouseButtons & mapping.sdlMask) != 0)
				continue;

			SDL_Event releaseEvent = {};
			releaseEvent.type = SDL_EVENT_MOUSE_BUTTON_UP;
			releaseEvent.button.type = SDL_EVENT_MOUSE_BUTTON_UP;
			releaseEvent.button.timestamp = SDL_GetTicksNS();
			releaseEvent.button.windowID = windowID;
			releaseEvent.button.button = mapping.sdlButton;
			releaseEvent.button.down = false;
			releaseEvent.button.clicks = 1;
			releaseEvent.button.x = mouseX;
			releaseEvent.button.y = mouseY;
			ImGui_ImplSDL3_ProcessEvent(&releaseEvent);
		}
	}
	void Gui::ShowDockSpace()
	{
		static bool dockspaceOpen = true;
		static bool optionFullscreen = true;
		static ImGuiDockNodeFlags dockspaceFlags = 0;

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