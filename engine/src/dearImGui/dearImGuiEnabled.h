#ifndef __INCLUDE_GUARD_dearImGuiEnabled_h__
#define __INCLUDE_GUARD_dearImGuiEnabled_h__
#include "dearImGuiFlags.h"
#include "emberMath.h"
#include <imgui.h>
#include <unordered_map>
#include <vector>
#include <SDL3/SDL.h>
#include <vulkan/vulkan.h>



// Forward declarations:
struct ImGuiIO;



namespace emberEngine
{
	// Forward declarations:
	class Texture2d;



	class DearImGui
	{
	public: // Members

	private: // Members
		static bool s_showDemoWindow;
		static bool s_wantCaptureKeyboard;
		static bool s_wantCaptureMouse;
		static ImGuiIO* s_pIo;
		static VkDescriptorSetLayout s_descriptorSetLayout;
		static std::unordered_map<Texture2d*, VkDescriptorSet> s_textureToDescriptorMap;

	public: // Methods
		static void Init();
		static void Clear();

		// Render Logic:
		static void Update();								// Must be called in main update loop of the engine.
		static void ProcessEvent(const SDL_Event& event);	// Must be called in SdlWindow::HandleEvents() before handing event over to event system.
		static void Render(VkCommandBuffer& commandBuffer);	// Must be called in main render loop before vkCmdEndRenderPass(...).

		// Getters:
		static bool WantCaptureKeyboard();
		static bool WantCaptureMouse();
		static uintptr_t GetTextureID(Texture2d* pTexture);

		// Helper functions:
		static void AddImGuiInstanceExtensions(std::vector<const char*>& instanceExtensions);

		// Wrappers:
		static bool IsWindowFocused(DearImGuiFocusedFlags flags);
		static bool Begin(const char* name, bool* pOpen, DearImGuiWindowFlags flags);
		static void End();
		static void PushID(const char* str_id);
		static void PopID();
		static Float2 GetWindowSize();
		static Float2 GetContentRegionalAvail();
		static Float2 GetCursorPos();
		static void SetCursorPos(const Float2& localPos);
		static void Image(uintptr_t textureID, const Float2& imageSize, const Float2& uv0 = Float2::zero, const Float2& uv1 = Float2::one);

	private: // Methods:
		static void ShowDockSpace();
		static void CreateDescriptorSetLayout();
		static VkDescriptorSet CreateDescriptorSet();
		static void UpdateDescriptor(VkDescriptorSet descriptorSet, VkImageView imageView, VkSampler sampler);

		// Delete all constructors:
		DearImGui() = delete;
		DearImGui(const DearImGui&) = delete;
		DearImGui& operator=(const DearImGui&) = delete;
		~DearImGui() = delete;
	};
}



#endif // __INCLUDE_GUARD_dearImGuiEnabled_h__