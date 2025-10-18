#pragma once
#include "commonGuiFlags.h"
#include "commonGuiStyle.h"
#include "emberMath.h"
#include <array>
#include <cstdarg>
#include <cstdint>
#include <functional>
#include <vector>



// Forward decleration:
typedef struct VkCommandBuffer_T* VkCommandBuffer;
typedef struct VkImageView_T* VkImageView;
typedef struct VkSampler_T* VkSampler;



namespace emberBackendInterface
{
	// Forward decleration:
	class IEditor;
	class ITexture;



	// Callbacks for editor integration:
	using EditorRenderCallback = std::function<void()>;
	using EditorCaptureQueryCallback = std::function<bool()>;



    class IGui
    {
    public:
		// Virtual destructor for v-table:
        virtual ~IGui() = default;

		// Render Logic:
		virtual void Update() = 0;									// Must be called in main update loop of the engine.
		virtual void ProcessEvent(const void* pWindowEvent) = 0;	// Must be called in from window.
		virtual void Render(VkCommandBuffer vkCommandBuffer) = 0;	// Must be called in a render pass.

		// Getters:
		virtual bool WantCaptureKeyboard() = 0;
		virtual bool WantCaptureMouse() = 0;
		virtual uintptr_t GetTextureID(ITexture* pITexture) = 0;
		virtual Float2 GetWindowSize() = 0;
		virtual Float2 GetContentRegionAvail() = 0;
		virtual Float2 GetCursorPos() = 0;
		virtual Float2 GetCursorScreenPos() = 0;
		virtual Float2 GetMouseDragDelta(emberCommon::GuiMouseButton button = emberCommon::GuiMouseButton::GuiMouseButton_Left, float lockThreshold = -1.0f) = 0;
		virtual emberCommon::GuiStyle GetStyle() const = 0;

		// Setters:
		virtual void SetEditorCallbacks(EditorRenderCallback renderCallback, EditorCaptureQueryCallback captureCallback) = 0;
		virtual void SetCursorPos(const Float2& localPos) = 0;
		virtual void SetCursorScreenPos(const Float2& pos) = 0;
		virtual void ResetMouseDragDelta(emberCommon::GuiMouseButton button = emberCommon::GuiMouseButton::GuiMouseButton_Left) = 0;

		// Window management:
		virtual bool Begin(const char* name, bool* pOpen = nullptr, emberCommon::GuiWindowFlags flags = 0) = 0;
		virtual void End() = 0;
		virtual void PushID(const char* strID) = 0;
		virtual void PopID() = 0;
		virtual bool IsWindowFocused(emberCommon::GuiFocusedFlags flags = 0) = 0;

		// Layout:
		virtual void SameLine(float offsetFromStartX = 0.0f, float spacingW = -1.0f) = 0;
		virtual void SetNextItemWidth(float itemWidth) = 0;
		virtual Float2 CalcTextSize(const char* text, const char* textEnd = nullptr, bool hideTextAfterDoubleHash = false, float wrapWidth = -1.0f) = 0;
		
		// State checks:
		virtual bool IsItemActive() = 0;
		virtual bool IsItemActivated() = 0;
		virtual bool IsMouseDragging(emberCommon::GuiMouseButton button = emberCommon::GuiMouseButton::GuiMouseButton_Left, float lockThreshold = -1.0f) = 0;

		// Widgets:
		virtual bool Checkbox(const char* label, bool* value) = 0;
		virtual bool InputInt(const char* label, int* value, int step = 1, int stepFast = 100, emberCommon::GuiInputTextFlags flags = 0) = 0;
		virtual bool InputFloat(const char* label, float* value, float step = 0.0f, float stepFast = 0.0f, const char* format = "%.3f", emberCommon::GuiInputTextFlags flags = 0) = 0;
		virtual void TextUnformatted(const char* text, const char* textEnd = nullptr) = 0;
		virtual void TextV(const char* format, va_list args) = 0; 	// mostly for internal use.
		inline void Text(const char* format, ...)
		{
			va_list args;
			va_start(args, format);
			TextV(format, args);
			va_end(args);
		}
		virtual bool Button(const char* label, const Float2& size = Float2::zero) = 0;
		virtual bool InvisibleButton(const char* strID, const Float2& size, emberCommon::GuiButtonFlags flags = 0) = 0;
		virtual void Image(uintptr_t textureID, const Float2& imageSize, const Float2& uv0 = Float2::zero, const Float2& uv1 = Float2::one) = 0;
    };
}