#ifndef __INCLUDE_GUARD_dearImGui_h__
#define __INCLUDE_GUARD_dearImGui_h__
#include <imgui.h>
#include <memory>
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
		static ImTextureID GetTextureID(Texture2d* pTexture);

		// Helper functions:
		static void AddImGuiInstanceExtensions(std::vector<const char*>& instanceExtensions);

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



#endif // __INCLUDE_GUARD_dearImGui_h__