#pragma once
#include "dearImGuiFlags.h"
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
	class IDearImGui;
}



namespace emberEngine
{
	class DearImGui
	{
		// Friends:
		friend class Renderer;

	private: // Members:
		static std::unique_ptr<emberBackendInterface::IDearImGui> s_pIDearImGui;
		static emberBackendInterface::IDearImGui* GetInterfaceHandle();

	public: // Methods:
		// Initialization/Cleanup:
		static void Init(void* pSdlWindow, bool enableDockSpace);
		static void Clear();

		// Render Logic:
		static void Update();									// Must be called in main update loop of the engine.
		static void ProcessEvent(const void* pEvent);			// Must be called in from sdl window.
		static void Render(VkCommandBuffer vkCommandBuffer);	// Must be called in a render pass.

		// Getters:
		static bool WantCaptureKeyboard();
		static bool WantCaptureMouse();
		static uintptr_t GetTextureID(Texture* pTexture);

		// Wrappers:
		static bool IsWindowFocused(emberEngine::DearImGuiFocusedFlags flags);
		static bool Begin(const char* name, bool* pOpen, emberEngine::DearImGuiWindowFlags flags);
		static void End();
		static void PushID(const char* str_id);
		static void PopID();
		static Float2 GetWindowSize();
		static Float2 GetContentRegionalAvail();
		static Float2 GetCursorPos();
		static void SetCursorPos(float localPosX, float localPosY);
		static void Image(uintptr_t textureID, float imageWidth, float imageHeight, Float2 uv0, Float2 uv1);

	private: // Methods
		// Delete all constructors:
		DearImGui() = delete;
		DearImGui(const DearImGui&) = delete;
		DearImGui& operator=(const DearImGui&) = delete;
		DearImGui(DearImGui&&) = delete;
		DearImGui& operator=(DearImGui&&) = delete;
		~DearImGui() = delete;
	};
}