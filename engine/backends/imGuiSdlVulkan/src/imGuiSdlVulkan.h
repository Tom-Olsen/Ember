#pragma once
#include "commonGuiFlags.h"
#include "commonGuiStyle.h"
#include "emberMath.h"
#include "iGui.h"
#include "imGuiSdlVulkanExport.h"
#include <unordered_map>



// Forward declarations:
struct ImGuiIO;
typedef struct VkDevice_T* VkDevice;
typedef struct VkDescriptorPool_T* VkDescriptorPool;
typedef struct VkCommandBuffer_T* VkCommandBuffer;
typedef struct VkImageView_T* VkImageView;
typedef struct VkSampler_T* VkSampler;
typedef struct VkDescriptorSet_T* VkDescriptorSet;
typedef struct VkDescriptorSetLayout_T* VkDescriptorSetLayout;
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
		VkDescriptorPool m_vkDescriptorPool;
		VkDescriptorSetLayout m_descriptorSetLayout;
		VkSampler m_vkColorSampler;
		ImGuiIO* m_pIo;
		bool m_wantCaptureKeyboard;
		bool m_wantCaptureMouse;
		bool m_enableDockSpace;
		std::unordered_map<VkImageView, VkDescriptorSet> m_vkImageViewToDescriptorMap;
		emberBackendInterface::EditorRenderCallback m_renderCallback;
		emberBackendInterface::EditorCaptureQueryCallback m_captureCallback;
		

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
		void Update() override;									// Must be called in main update loop of the engine.
		void ProcessEvent(const void* pWindowEvent) override;	// Must be called in from window.
		void Render(VkCommandBuffer vkCommandBuffer) override;	// Must be called in a render pass.

		// Getters:
		bool WantCaptureKeyboard() override;
		bool WantCaptureMouse() override;
		uintptr_t GetTextureID(emberBackendInterface::ITexture* pITexture) override;
		Float2 GetWindowSize() override;
		Float2 GetContentRegionAvail() override;
		Float2 GetCursorPos() override;
		Float2 GetCursorScreenPos() override;
		Float2 GetMouseDragDelta(emberCommon::GuiMouseButton button = emberCommon::GuiMouseButton::GuiMouseButton_Left, float lockThreshold = -1.0f) override;
		emberCommon::GuiStyle GetStyle() const override;

		// Setters:
		void SetEditorCallbacks(emberBackendInterface::EditorRenderCallback renderCallback, emberBackendInterface::EditorCaptureQueryCallback captureCallback) override;
		void SetCursorPos(const Float2& localPos) override;
		void SetCursorScreenPos(const Float2& pos) override;
		void ResetMouseDragDelta(emberCommon::GuiMouseButton button = emberCommon::GuiMouseButton::GuiMouseButton_Left) override;

		// Window management:
		bool Begin(const char* name, bool* pOpen = nullptr, emberCommon::GuiWindowFlags flags = 0) override;
		void End() override;
		void PushID(const char* strID) override;
		void PopID() override;
		bool IsWindowFocused(emberCommon::GuiFocusedFlags flags = 0) override;

		// Layout:
		void SameLine(float offsetFromStartX = 0.0f, float spacingW = -1.0f) override;
		void SetNextItemWidth(float itemWidth) override;
		Float2 CalcTextSize(const char* text, const char* textEnd = nullptr, bool hideTextAfterDoubleHash = false, float wrapWidth = -1.0f) override;

		// State checks:
		bool IsItemActive() override;
		bool IsItemActivated() override;
		bool IsMouseDragging(emberCommon::GuiMouseButton button = emberCommon::GuiMouseButton::GuiMouseButton_Left, float lockThreshold = -1.0f) override;

		// Widgets:
		bool Checkbox(const char* label, bool* value) override;
		bool InputInt(const char* label, int* value, int step = 1, int stepFast = 100, emberCommon::GuiInputTextFlags flags = 0) override;
		bool InputFloat(const char* label, float* value, float step = 0.0f, float stepFast = 0.0f, const char* format = "%.3f", emberCommon::GuiInputTextFlags flags = 0) override;
		void TextUnformatted(const char* text, const char* textEnd = nullptr) override;
		void TextV(const char* format, va_list args) override;
		bool Button(const char* label, const Float2& size = Float2::zero) override;
		bool InvisibleButton(const char* strID, const Float2& size, emberCommon::GuiButtonFlags flags = 0) override;
		void Image(uintptr_t textureID, const Float2& imageSize, const Float2& uv0 = Float2::zero, const Float2& uv1 = Float2::one) override;

	private: // Methods:
		void ShowDockSpace();
		void CreateDescriptorSetLayout();
		VkDescriptorSet CreateDescriptorSet();
		void UpdateDescriptor(VkDescriptorSet vkDescriptorSet, VkImageView vkImageView, VkSampler vkSampler);
	};
}