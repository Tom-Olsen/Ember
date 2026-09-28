#include "imGuiSdlVulkan.h"
#include "iRenderer.h"
#include "iTexture.h"
#include "iVulkanRenderer.h"
#include "iVulkanTexture.h"
#include "iWindow.h"
#include "imGuiConvertGuiFlags.h"
#include "imGuiConvertGuiStyle.h"
#include <stdexcept>
#include <utility>
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
		// Invalid input:
		if (pIWindow == nullptr)
			throw std::invalid_argument("imGuiSdlVulkanBackend::Gui::Gui(...) failed. pIWindow is nullptr.");
		if (pIRenderer == nullptr)
			throw std::invalid_argument("imGuiSdlVulkanBackend::Gui::Gui(...) failed. pIRenderer is nullptr.");

		// Check if renderer is vulkan renderer:
		m_pIRenderer = pIRenderer;
		m_pIVulkanRenderer = dynamic_cast<emberBackendInterface::IVulkanRenderer*>(pIRenderer);
		if (m_pIVulkanRenderer == nullptr)
			throw std::runtime_error("imGuiSdlVulkanBackend::Gui::Gui(...) failed. Renderer backend does not implement IVulkanRenderer.");

		m_pSdlWindow = static_cast<SDL_Window*>(pIWindow->GetNativeHandle());
		m_vkDevice = m_pIVulkanRenderer->GetVkDevice();
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

		// Init ImGui vulkan implementation:
		ImGui_ImplVulkan_InitInfo initInfo = {};
		initInfo.Instance = m_pIVulkanRenderer->GetVkInstance();
		initInfo.PhysicalDevice = m_pIVulkanRenderer->GetVkPhysicalDevice();
		initInfo.Device = m_vkDevice;
		initInfo.Queue = m_pIVulkanRenderer->GetGraphicsVkQueue();
		initInfo.QueueFamily = m_pIVulkanRenderer->GetGraphicsVkQueueFamilyIndex();
		initInfo.PipelineInfoMain.RenderPass = m_pIVulkanRenderer->GetPresentVkRenderPass();
		initInfo.DescriptorPoolSize = 8 * m_pIVulkanRenderer->GetFramesInFlight();	// ImGui needs at least 8 descriptor sets per frame. If you use more than 8 textures in a single frame, increase this value.
		initInfo.MinImageCount = 2;
		initInfo.ImageCount = m_pIVulkanRenderer->GetSwapchainImageCount();
		initInfo.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;      		// same as renderer present pass, which is hardcoded to 1.
		#ifdef VALIDATION_LAYERS_ACTIVE
		initInfo.MinAllocationSize = 1024 * 1024;
		#endif
		ImGui_ImplVulkan_Init(&initInfo);
	}
	Gui::~Gui()
	{
		if (m_pIo)
		{
			for (const auto& [_, descriptorSet] : m_vkImageViewToDescriptorMap)
				ImGui_ImplVulkan_RemoveTexture(descriptorSet);
			ImGui_ImplVulkan_Shutdown();
			ImGui_ImplSDL3_Shutdown();
			ImGui::DestroyContext();
		}
		m_vkImageViewToDescriptorMap.clear();
	}



	// Move semantics:
	Gui::Gui(Gui&& other) noexcept
	{
		// Transfer resources: other->this
		m_pIRenderer = other.m_pIRenderer;
		m_pIVulkanRenderer = other.m_pIVulkanRenderer;
		m_vkDevice = other.m_vkDevice;
		m_pIo = other.m_pIo;
		m_pSdlWindow = other.m_pSdlWindow;
		m_wantCaptureKeyboard = other.m_wantCaptureKeyboard;
		m_wantCaptureMouse = other.m_wantCaptureMouse;
		m_enableDockSpace = other.m_enableDockSpace;
		m_vkImageViewToDescriptorMap = std::move(other.m_vkImageViewToDescriptorMap);
		m_renderEditorCallback = std::move(other.m_renderEditorCallback);
		m_focusedWindowWantCaptureEventsCallback = std::move(other.m_focusedWindowWantCaptureEventsCallback);
		m_hoveredWindowWantCaptureEventsCallback = std::move(other.m_hoveredWindowWantCaptureEventsCallback);

		// Invalidate other:
		other.m_pIRenderer = nullptr;
		other.m_pIVulkanRenderer = nullptr;
		other.m_vkDevice = VK_NULL_HANDLE;
		other.m_pIo = nullptr;
		other.m_pSdlWindow = nullptr;
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
			if (m_pIo)
			{
				for (const auto& [_, descriptorSet] : m_vkImageViewToDescriptorMap)
					ImGui_ImplVulkan_RemoveTexture(descriptorSet);
				ImGui_ImplVulkan_Shutdown();
				ImGui_ImplSDL3_Shutdown();
				ImGui::DestroyContext();
			}
			m_vkImageViewToDescriptorMap.clear();

			// Transfer resources: other->this
			m_pIRenderer = other.m_pIRenderer;
			m_pIVulkanRenderer = other.m_pIVulkanRenderer;
			m_vkDevice = other.m_vkDevice;
			m_pIo = other.m_pIo;
			m_pSdlWindow = other.m_pSdlWindow;
			m_wantCaptureKeyboard = other.m_wantCaptureKeyboard;
			m_wantCaptureMouse = other.m_wantCaptureMouse;
			m_enableDockSpace = other.m_enableDockSpace;
			m_vkImageViewToDescriptorMap = std::move(other.m_vkImageViewToDescriptorMap);
			m_renderEditorCallback = std::move(other.m_renderEditorCallback);
			m_focusedWindowWantCaptureEventsCallback = std::move(other.m_focusedWindowWantCaptureEventsCallback);
			m_hoveredWindowWantCaptureEventsCallback = std::move(other.m_hoveredWindowWantCaptureEventsCallback);

			// Invalidate other:
			other.m_pIRenderer = nullptr;
			other.m_pIVulkanRenderer = nullptr;
			other.m_vkDevice = VK_NULL_HANDLE;
			other.m_pIo = nullptr;
			other.m_pSdlWindow = nullptr;
			other.m_wantCaptureKeyboard = false;
			other.m_wantCaptureMouse = false;
			other.m_enableDockSpace = false;
			other.m_vkImageViewToDescriptorMap.clear();
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
	Float2 Gui::GetMousePos()
	{
		ImVec2 mousePos = ImGui::GetMousePos();
		return Float2{ mousePos.x, mousePos.y };
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
	void Gui::FocusCurrentWindow()
	{
		ImGui::SetWindowFocus();
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
	bool Gui::IsMouseClicked(emberCommon::GuiMouseButton button)
	{
		return ImGui::IsMouseClicked(GuiMouseButtonCommonToImGui(button));
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
	bool Gui::ColorEdit(const char* label, float* color)
	{
		// NoInputs => only the color swatch is shown, clicking it opens the full picker popup.
		ImGuiColorEditFlags flags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreviewHalf;
		return ImGui::ColorEdit4(label, color, flags);
	}
	bool Gui::InputInt(const char* label, int* value, int step, int stepFast, emberCommon::GuiInputTextFlags flags)
	{
		return ImGui::InputInt(label, value, step, stepFast, GuiInputTextFlagsCommonToImGui(flags));
	}
	bool Gui::InputUint(const char* label, uint32_t* value, uint32_t step, uint32_t stepFast, emberCommon::GuiInputTextFlags flags)
	{
		return ImGui::InputScalar(label, ImGuiDataType_U32, value, step > 0 ? &step : nullptr, step > 0 ? &stepFast : nullptr, "%u", GuiInputTextFlagsCommonToImGui(flags));
	}
	bool Gui::InputFloat(const char* label, float* value, float step, float stepFast, const char* format, emberCommon::GuiInputTextFlags flags)
	{
		return ImGui::InputFloat(label, value, step, stepFast, format, GuiInputTextFlagsCommonToImGui(flags));
	}
	void Gui::TextUnformatted(const char* text, const char* textEnd)
	{
		return ImGui::TextUnformatted(text, textEnd);
	}
	void Gui::SeparatorText(const char* label)
	{
		ImGui::SeparatorText(label);
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
	void Gui::Image(emberBackendInterface::ITexture* pTexture, const Float2& imageSize, const Float2& uv0, const Float2& uv1)
	{
		uintptr_t textureID = GetTextureID(pTexture);
		if (textureID == 0)
			return;
		ImGui::Image(static_cast<ImTextureID>(textureID), ImVec2{ imageSize.x, imageSize.y }, ImVec2{ uv0.x, uv0.y }, ImVec2{ uv1.x, uv1.y });
	}



	// Private methods:
	uintptr_t Gui::GetTextureID(emberBackendInterface::ITexture* pTexture)
	{
		// Invalid input:
		if (!pTexture)
			return 0;
		emberBackendInterface::IVulkanTexture* pVulkanTexture = dynamic_cast<emberBackendInterface::IVulkanTexture*>(pTexture);
		if (!pVulkanTexture)
			throw std::runtime_error("imGuiSdlVulkanBackend::Gui::GetTextureID(...) failed. Texture backend does not implement IVulkanTexture.");

		// Get bulkan imageView/Layout:
		const uint32_t frameIndex = m_pIRenderer->GetFrameIndex();
		VkImageView imageView = pVulkanTexture->GetVkImageView(frameIndex);
		VkImageLayout imageLayout = pVulkanTexture->GetVkImageLayout(frameIndex);
		if (imageView == VK_NULL_HANDLE)
			throw std::runtime_error("imGuiSdlVulkanBackend::Gui::GetTextureID(...) failed. Texture image view is null.");
		if (imageLayout != VK_IMAGE_LAYOUT_GENERAL && imageLayout != VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
			throw std::runtime_error("imGuiSdlVulkanBackend::Gui::GetTextureID(...) failed. Texture is not in a shader-readable image layout.");

		// Return cached descriptor set if it exists:
		auto it = m_vkImageViewToDescriptorMap.find(imageView);
		if (it != m_vkImageViewToDescriptorMap.end())
			return reinterpret_cast<uintptr_t>(it->second);

		// Create descriptor set for this vkImageView:
		VkDescriptorSet descriptorSet = ImGui_ImplVulkan_AddTexture(imageView, imageLayout);

		// Cache and return:
		m_vkImageViewToDescriptorMap[imageView] = descriptorSet;
		return reinterpret_cast<uintptr_t>(descriptorSet);
	}
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
}