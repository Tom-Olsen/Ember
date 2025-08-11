#ifndef __INCLUDE_GUARD_dearImGuiDisabled_h__
#define __INCLUDE_GUARD_dearImGuiDisabled_h__
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
		static void Init() {}
		static void Clear() {}

		// Render Logic:
		static void Update() {}									// Must be called in main update loop of the engine.
		static void ProcessEvent(const SDL_Event& event) {}		// Must be called in SdlWindow::HandleEvents() before handing event over to event system.
		static void Render(VkCommandBuffer& commandBuffer) {}	// Must be called in main render loop before vkCmdEndRenderPass(...).

		// Getters:
		static bool WantCaptureKeyboard() { return false; }
		static bool WantCaptureMouse() { return false; }
		static uintptr_t GetTextureID(Texture2d* pTexture) { return 0; }

		// Helper functions:
		static void AddImGuiInstanceExtensions(std::vector<const char*>& instanceExtensions) {}

		// Wrappers:
		static Float2 GetContentRegionalAvail() { return Float2::zero; }
		static Float2 GetCursorPos() { return Float2::zero; }
		static void SetCursorPos(const Float2& localPos) {}
		static void Image(uintptr_t textureID, const Float2& imageSize, const Float2& uv0 = Float2::zero, const Float2& uv1 = Float2::one) {}
	};
}



#endif // __INCLUDE_GUARD_dearImGuiDisabled_h__