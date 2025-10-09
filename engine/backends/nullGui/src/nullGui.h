#pragma once
#include "commonGuiFlags.h"
#include "emberMath.h"
#include "iGui.h"
#include "nullGuiExport.h"



// Forward declarations:
typedef struct VkCommandBuffer_T* VkCommandBuffer;
namespace emberBackendInterface
{
	class ITexture;
}



namespace nullGuiBackend
{
	class NULL_GUI_API Gui : public emberBackendInterface::IGui
	{
	public: // Methods
		// Constructor/Destructor:
		Gui();
		~Gui();

		// Non-copyable:
		Gui(const Gui&) = delete;
		Gui& operator=(const Gui&) = delete;

		// Movable:
		Gui(Gui&& other) noexcept = default;
		Gui& operator=(Gui&& other) noexcept = default;

		// Render Logic:
		void Update() override;									// Must be called in main update loop of the engine.
		void ProcessEvent(const void* pWindowEvent) override;	// Must be called in from window.
		void Render(VkCommandBuffer vkCommandBuffer) override;	// Must be called in a render pass.

		// Getters:
		bool WantCaptureKeyboard() override;
		bool WantCaptureMouse() override;
		uintptr_t GetTextureID(emberBackendInterface::ITexture* pITexture) override;

		// Wrappers:
		bool IsWindowFocused(emberCommon::GuiFocusedFlags flags) override;
		bool Begin(const char* name, bool* pOpen, emberCommon::GuiWindowFlags flags) override;
		void End() override;
		void PushID(const char* str_id) override;
		void PopID() override;
		Float2 GetWindowSize() override;
		Float2 GetContentRegionalAvail() override;
		Float2 GetCursorPos() override;
		void SetCursorPos(const Float2& localPos) override;
		void Image(uintptr_t textureID, float imageWidth, float imageHeight, float u0, float v0, float u1, float v1) override;
	};
}