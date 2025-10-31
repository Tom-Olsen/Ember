#pragma once
#include "commonGuiFlags.h"
#include "commonGuiStyle.h"
#include "emberCoreExport.h"
#include "emberMath.h"
#include <memory>



// Forward decleration:
struct SDL_Window;
typedef struct VkCommandBuffer_T* VkCommandBuffer;
namespace emberEngine
{
	class Texture;
}
namespace emberBackendInterface
{
	class IGui;
}



namespace emberEngine
{
	class EMBER_CORE_API Gui
	{
		// Friends:
		friend class EditorWindow;

	private: // Members:
		static bool s_isInitialized;
		static float s_intDragSensitivityBase;
		static float s_floatDragSensitivityBase;
		static float s_dragSensitivityExponent;
		static float s_minWidgetWidth;
		static float s_labelPercentile;			// percentile of window width used for labels.
		static std::unique_ptr<emberBackendInterface::IGui> s_pIGui;
		static emberBackendInterface::IGui* GetInterfaceHandle();

	public: // Methods:
		// Initialization/Cleanup:
		static void Init(emberBackendInterface::IGui* pIGui);
		static void Clear();

		// Render Logic:
		static void Update();									// Must be called in main update loop of the engine.
		static void ProcessEvent(const void* pWindowEvent);		// Must be called in from sdl window.
		static void Render(VkCommandBuffer vkCommandBuffer);	// Must be called in a render pass.

		// Getters:
		static bool WantCaptureKeyboard();
		static bool WantCaptureMouse();
		static uintptr_t GetTextureID(Texture& texture);
		static Float2 GetWindowSize();
		static Float2 GetContentRegionAvail();
		static Float2 GetCursorPos();
		static Float2 GetCursorScreenPos();
		static Float2 GetMouseDragDelta(emberCommon::GuiMouseButton button = emberCommon::GuiMouseButton::GuiMouseButton_Left, float lockThreshold= -1.0f);
		static emberCommon::GuiStyle GetStyle();

		// Setters:
		static void SetCursorPos(const Float2& localPos);
		static void SetCursorScreenPos(const Float2& pos);
		static void ResetMouseDragDelta(emberCommon::GuiMouseButton button = emberCommon::GuiMouseButton::GuiMouseButton_Left);
		
		// Window management:
		static bool Begin(const char* name, bool* pOpen = nullptr, emberCommon::GuiWindowFlags flags = 0);
		static void End();
		static void PushID(const char* strID);
		static void PopID();
		static bool IsWindowFocused(emberCommon::GuiFocusedFlags flags);

		// Layout:
		static void SameLine(float offsetFromStartX = 0.0f, float spacingW = -1.0f);
		static void SetNextItemWidth(float itemWidth);
		static Float2 CalcTextSize(const char* text, const char* textEnd = nullptr, bool hideTextAfterDoubleHash = false, float wrapWidth = -1.0f);

		// State checks:
		static bool IsItemActive();
		static bool IsItemActivated();
		static bool IsMouseDragging(emberCommon::GuiMouseButton button = emberCommon::GuiMouseButton::GuiMouseButton_Left, float lockThreshold = -1.0f);

		// Widgets:
		static void TextUnformatted(const char* text, const char* textEnd = nullptr);
		//static void TextV(const char* format, va_list args); 	// not sure if i want to expose this yet.
		static void Text(const char* format, ...);
		static bool Button(const char* label, const Float2& size = Float2::zero);
		static bool InvisibleButton(const char* strID, const Float2& size, emberCommon::GuiButtonFlags flags = 0);
		static void Image(uintptr_t textureID, const Float2& imageSize, const Float2& uv0 = Float2::zero, const Float2& uv1 = Float2::one);

		// Custom Widgets:
		static bool Checkbox(const std::string& label, bool* value);
		static bool InputInt(const std::string& label, int* value, int step = 1, int stepFast = 100, emberCommon::GuiWindowFlags flags = 0);
		static bool DragInt(const std::string& label, int* value, int step = 1, int stepFast = 100, emberCommon::GuiWindowFlags flags = 0);
		static bool InputFloat(const std::string& label, float* value, float step = 0.0f, float stepFast = 0.0f, const char* format = "%.3f", emberCommon::GuiWindowFlags flags = 0);
		static bool DragFloat(const std::string& label, float* value, float step = 0.0f, float stepFast = 0.0f, const char* format = "%.3f", emberCommon::GuiWindowFlags flags = 0);

	private: // Methods
		// Delete all constructors:
		Gui() = delete;
		Gui(const Gui&) = delete;
		Gui& operator=(const Gui&) = delete;
		Gui(Gui&&) = delete;
		Gui& operator=(Gui&&) = delete;
		~Gui() = delete;
	};
}