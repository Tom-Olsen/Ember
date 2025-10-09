#pragma once
#include "commonGuiFlags.h"
#include "emberMath.h"
#include <array>
#include <cstdint>
#include <vector>



// Forward decleration:
typedef struct VkCommandBuffer_T* VkCommandBuffer;
typedef struct VkImageView_T* VkImageView;
typedef struct VkSampler_T* VkSampler;



namespace emberBackendInterface
{
	// Forward decleration:
	class ITexture;



    class IGui
    {
    public:
		// Virtual destructor for v-table:
        virtual ~IGui() = default;

		// Render Logic:
		virtual void Update() = 0;									// Must be called in main update loop of the engine.
		virtual void ProcessEvent(const void* pWindowEvent) = 0;			// Must be called in from window.
		virtual void Render(VkCommandBuffer vkCommandBuffer) = 0;	// Must be called in a render pass.

		// Getters:
		virtual bool WantCaptureKeyboard() = 0;
		virtual bool WantCaptureMouse() = 0;
		virtual uintptr_t GetTextureID(ITexture* pITexture) = 0;

		// Wrappers:
		virtual bool IsWindowFocused(emberCommon::GuiFocusedFlags flags) = 0;
		virtual bool Begin(const char* name, bool* pOpen, emberCommon::GuiWindowFlags flags) = 0;
		virtual void End() = 0;
		virtual void PushID(const char* str_id) = 0;
		virtual void PopID() = 0;
		virtual Float2 GetWindowSize() = 0;
		virtual Float2 GetContentRegionalAvail() = 0;
		virtual Float2 GetCursorPos() = 0;
		virtual void SetCursorPos(const Float2& localPos) = 0;
		virtual void Image(uintptr_t textureID, float imageWidth, float imageHeight, float u0, float v0, float u1, float v1) = 0;
    };
}