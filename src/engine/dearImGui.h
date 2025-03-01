#ifndef __INCLUDE_GUARD_dearImGui_h__
#define __INCLUDE_GUARD_dearImGui_h__
#include <memory>
#include <vector>
#include <SDL3/SDL.h>
#include <vulkan/vulkan.h>



// Forward declarations:
struct ImGuiIO;



namespace emberEngine
{
	// Forward declarations:
	class VulkanContext;



	class DearImGui
	{
	public: // Members

	private: // Members
		static bool s_showDemoWindow;
		static bool s_wantCaptureKeyboard;
		static bool s_wantCaptureMouse;
		static ImGuiIO* s_io;

	public: // Methods
		static void Init(VulkanContext* pContext);
		static void Clear();
		static void Update();
		static void ProcessEvent(const SDL_Event& event);
		static void Render(VkCommandBuffer& commandBuffer);

		// Getters:
		static bool WantCaptureKeyboard();
		static bool WantCaptureMouse();

		static void AddImGuiInstanceExtensions(std::vector<const char*>& instanceExtensions);

	private: // Methods
		// Delete all constructors:
		DearImGui() = delete;
		DearImGui(const DearImGui&) = delete;
		DearImGui& operator=(const DearImGui&) = delete;
		~DearImGui() = delete;
	};
}



#endif // __INCLUDE_GUARD_dearImGui_h__