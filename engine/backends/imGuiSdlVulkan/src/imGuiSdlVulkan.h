#pragma once
#include "commonGuiFlags.h"
#include "commonGuiStyle.h"
#include "emberMath.h"
#include "iGui.h"
#include "imGuiSdlVulkanExport.h"
#include <unordered_map>



// Forward declarations:
struct ImGuiIO;
struct SDL_Window;
typedef struct VkDevice_T* VkDevice;
typedef struct VkCommandBuffer_T* VkCommandBuffer;
typedef struct VkImageView_T* VkImageView;
typedef struct VkDescriptorSet_T* VkDescriptorSet;
namespace emberBackendInterface
{
	class IRenderer;
	class ITexture;
	class IWindow;
}



namespace imGuiSdlVulkanBackend
{
	class IMGUI_SDL_VULKAN_API Gui : public emberBackendInterface::IGui
	{
	private: // Members
		VkDevice m_vkDevice;
		SDL_Window* m_pSdlWindow;
		ImGuiIO* m_pIo;
		bool m_wantCaptureKeyboard;
		bool m_wantCaptureMouse;
		bool m_enableDockSpace;
		std::unordered_map<VkImageView, VkDescriptorSet> m_vkImageViewToDescriptorMap;
		emberBackendInterface::EditorRenderCallback m_renderEditorCallback;
		emberBackendInterface::EditorCaptureQueryCallback m_focusedWindowWantCaptureEventsCallback;
		emberBackendInterface::EditorCaptureQueryCallback m_hoveredWindowWantCaptureEventsCallback;
		

	public: // Methods
		// Constructor/Destructor:
		Gui(emberBackendInterface::IWindow* pIWindow, emberBackendInterface::IRenderer* pIRenderer, bool enableDockSpace);
		~Gui();

		// Non-copyable:
		Gui(const Gui&) = delete;
		Gui& operator=(const Gui&) = delete;

		// Movable:
		Gui(Gui&& other) noexcept;
		Gui& operator=(Gui&& other) noexcept;

		// Render Logic:
		void Update() override;									// must be called in main update loop of the engine.
		void ProcessEvent(const void* pWindowEvent) override;	// must be called in from window.
		void Render(VkCommandBuffer vkCommandBuffer) override;	// must be called in a render pass.

		// Getters:
		bool WantCaptureKeyboard() override;
		bool WantCaptureMouse() override;
		uintptr_t GetTextureID(emberBackendInterface::ITexture* pITexture) override;
		Float2 GetWindowSize() override;
		Float2 GetContentRegionAvail() override;
        Float2 GetCursorPos() override;         // layout cursor for widgets, relative window space.
		Float2 GetCursorScreenPos() override;   // same layout cursor, absolute screen space (due to ImGuiConfigFlags_ViewportsEnable enabled => global OS-desktop space).
		Float2 GetMousePos() override;          // mouse pointer, absolute screen space (due to ImGuiConfigFlags_ViewportsEnable enabled => global OS-desktop space).
		Float2 GetMouseDragDelta(emberCommon::GuiMouseButton button = emberCommon::GuiMouseButton::left, float lockThreshold = -1.0f) override;
		emberCommon::GuiStyle GetStyle() const override;

		// Setters:
		void SetEditorCallbacks(emberBackendInterface::EditorRenderCallback renderEditorCallback, emberBackendInterface::EditorCaptureQueryCallback focusedWindowWantCaptureEventsCallback, emberBackendInterface::EditorCaptureQueryCallback hoveredWindowWantCaptureEventsCallback) override;
		void SetCursorPos(const Float2& localPos) override;
		void SetCursorScreenPos(const Float2& pos) override;
		void ResetMouseDragDelta(emberCommon::GuiMouseButton button = emberCommon::GuiMouseButton::left) override;

		// Window management:
		bool Begin(const char* name, bool* pOpen = nullptr, emberCommon::GuiWindowFlags flags = emberCommon::GuiWindowFlags::none) override;
		void End() override;
		void PushID(const char* strID) override;
		void PopID() override;
		void FocusCurrentWindow() override;
		bool IsWindowFocused(emberCommon::GuiFocusedFlags flags = emberCommon::GuiFocusedFlags::none) override;
		bool IsWindowHovered(emberCommon::GuiHoveredFlags flags = emberCommon::GuiHoveredFlags::none) override;

		// Layout:
		void SameLine(float offsetFromStartX = 0.0f, float spacingW = -1.0f) override;
		void SetNextItemWidth(float itemWidth) override;
		Float2 CalcTextSize(const char* text, const char* textEnd = nullptr, bool hideTextAfterDoubleHash = false, float wrapWidth = -1.0f) override;

		// State checks:
		bool IsItemActive() override;
		bool IsItemActivated() override;
		bool IsMouseClicked(emberCommon::GuiMouseButton button = emberCommon::GuiMouseButton::left) override;
		bool IsMouseDragging(emberCommon::GuiMouseButton button = emberCommon::GuiMouseButton::left, float lockThreshold = -1.0f) override;

		// Widgets:
		bool Checkbox(const char* label, bool* value) override;
		bool ColorEdit(const char* label, float* color) override;
		bool InputInt(const char* label, int* value, int step = 1, int stepFast = 100, emberCommon::GuiInputTextFlags flags = emberCommon::GuiInputTextFlags::none) override;
		bool InputFloat(const char* label, float* value, float step = 0.0f, float stepFast = 0.0f, const char* format = "%.3f", emberCommon::GuiInputTextFlags flags = emberCommon::GuiInputTextFlags::none) override;
		void TextUnformatted(const char* text, const char* textEnd = nullptr) override;
		void SeparatorText(const char* label) override;
		void TextV(const char* format, va_list args) override;
		bool Button(const char* label, const Float2& size = Float2::zero) override;
		bool InvisibleButton(const char* strID, const Float2& size, emberCommon::GuiButtonFlags flags = emberCommon::GuiButtonFlags::none) override;
		bool Selectable(const char* label, bool selected) override;
		void Image(uintptr_t textureID, const Float2& imageSize, const Float2& uv0 = Float2::zero, const Float2& uv1 = Float2::one) override;

	private: // Methods:
		void ReleaseStaleMouseButtons();
		void ShowDockSpace();
	};
}