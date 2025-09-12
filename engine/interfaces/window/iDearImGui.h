#ifndef __INCLUDE_GUARD_iDearImGui_h__
#define __INCLUDE_GUARD_iDearImGui_h__
#include "dearImGuiFlags.h"
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
	class IDearImGuiInstanceExtensionsLoader
	{
	public:
		virtual void AddExtensions(std::vector<const char*>& instanceExtensions) const = 0;
	};



    class IDearImGui
    {
    public:
        IDearImGui() = default;
        virtual ~IDearImGui() = default;

        // Non-copyable:
        IDearImGui(const IDearImGui&) = delete;
        IDearImGui& operator=(const IDearImGui&) = delete;

        // Movable: (must be implemented by derived classes)
        IDearImGui(IDearImGui&&) noexcept = default;
        IDearImGui& operator=(IDearImGui&&) noexcept = default;

		// Render Logic:
		virtual void Update() = 0;									// Must be called in main update loop of the engine.
		virtual void ProcessEvent(void* pEvent) = 0;				// Must be called in from sdl window.
		virtual void Render(VkCommandBuffer vkCommandBuffer) = 0;	// Must be called in a render pass.

		// Getters:
		virtual bool WantCaptureKeyboard() = 0;
		virtual bool WantCaptureMouse() = 0;
		virtual uintptr_t GetTextureID(VkImageView vkImageView, VkSampler vkSampler) = 0;

		// Wrappers:
		virtual bool IsWindowFocused(emberEngine::DearImGuiFocusedFlags flags) = 0;
		virtual bool Begin(const char* name, bool* pOpen, emberEngine::DearImGuiWindowFlags flags) = 0;
		virtual void End() = 0;
		virtual void PushID(const char* str_id) = 0;
		virtual void PopID() = 0;
		virtual Float2 GetWindowSize() = 0;
		virtual Float2 GetContentRegionalAvail() = 0;
		virtual Float2 GetCursorPos() = 0;
		virtual void SetCursorPos(float localPosX, float localPosY) = 0;
		virtual void Image(uintptr_t textureID, float imageWidth, float imageHeight, float u0, float v0, float u1, float v1) = 0;
    };
}



#endif // __INCLUDE_GUARD_iDearImGui_h__