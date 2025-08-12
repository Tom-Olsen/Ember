#ifndef __INCLUDE_GUARD_dearImGuiDisabled_h__
#define __INCLUDE_GUARD_dearImGuiDisabled_h__
#include "dearImGuiFlags.h"
#include "emberMath.h"
#include <vector>



// Forward declarations:
union SDL_Event;
typedef struct VkCommandBuffer_T* VkCommandBuffer;



namespace emberEngine
{
	// Forward declarations:
	class Texture2d;



	class DearImGui
	{
	public: // Methods
		inline static void Init() {}
		inline static void Clear() {}

		// Render Logic:
		inline static void Update() {}									// Must be called in main update loop of the engine.
		inline static void ProcessEvent(const SDL_Event& event) {}		// Must be called in SdlWindow::HandleEvents() before handing event over to event system.
		inline static void Render(VkCommandBuffer& commandBuffer) {}	// Must be called in main render loop before vkCmdEndRenderPass(...).

		// Getters:
		inline static bool WantCaptureKeyboard() { return false; }
		inline static bool WantCaptureMouse() { return false; }
		inline static uintptr_t GetTextureID(Texture2d* pTexture) { return 0; }

		// Helper functions:
		inline static void AddImGuiInstanceExtensions(std::vector<const char*>& instanceExtensions) {}

		// Wrappers:
		inline static bool IsWindowFocused(DearImGuiFocusedFlags flags) { return false; }
		inline static bool Begin(const char* name, bool* pOpen, DearImGuiWindowFlags flags) { return false; }
		inline static void End() {}
		inline static void PushID(const char* str_id) {}
		inline static void PopID() {}
		inline static Float2 GetWindowSize() { return Float2::zero; }
		inline static Float2 GetContentRegionalAvail() { return Float2::zero; }
		inline static Float2 GetCursorPos() { return Float2::zero; }
		inline static void SetCursorPos(const Float2& localPos) {}
		inline static void Image(uintptr_t textureID, const Float2& imageSize, const Float2& uv0 = Float2::zero, const Float2& uv1 = Float2::one) {}
	};
}



#endif // __INCLUDE_GUARD_dearImGuiDisabled_h__