#pragma once
#include "commonGuiFlags.h"
#include "commonEvent.h"
#include "emberMath.h"
#include <memory>



// Forward decleration:
struct SDL_Window;
typedef struct VkCommandBuffer_T* VkCommandBuffer;
namespace emberEngine
{
	class Sampler;
	class Texture;
}
namespace emberBackendInterface
{
	class IGui;
}



namespace emberEngine
{
	class Gui
	{
		// Friends:
		friend class Renderer;

	private: // Members:
		static bool s_isInitialized;
		static std::unique_ptr<emberBackendInterface::IGui> s_pIGui;
		static emberBackendInterface::IGui* GetInterfaceHandle();

	public: // Methods:
		// Initialization/Cleanup:
		static void Init(bool enableDockSpace);
		static void Clear();

		// Render Logic:
		static void Update();									// Must be called in main update loop of the engine.
		static void ProcessEvent(const void* pWindowEvent);		// Must be called in from sdl window.
		static void Render(VkCommandBuffer vkCommandBuffer);	// Must be called in a render pass.

		// Getters:
		static bool WantCaptureKeyboard();
		static bool WantCaptureMouse();
		static uintptr_t GetTextureID(Texture& texture);

		// Wrappers:
		static bool IsWindowFocused(emberCommon::GuiFocusedFlags flags);
		static bool Begin(const char* name, bool* pOpen, emberCommon::GuiWindowFlags flags);
		static void End();
		static void PushID(const char* str_id);
		static void PopID();
		static Float2 GetWindowSize();
		static Float2 GetContentRegionalAvail();
		static Float2 GetCursorPos();
		static void SetCursorPos(const Float2& localPos);
		static void Image(uintptr_t textureID, float imageWidth, float imageHeight, Float2 uv0, Float2 uv1);

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