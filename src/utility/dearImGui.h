#ifndef __INCLUDE_GUARD_dearImGui_h__
#define __INCLUDE_GUARD_dearImGui_h__
#include <memory>
#include <SDL3/SDL.h>
#include <vulkan/vulkan.h>



class VulkanContext;



class DearImGui
{
public: // Members

private: // Members
	static bool s_isEnabled;
	static bool s_showDemoWindow;
	static bool s_wantCaptureKeyboard;
	static bool s_wantCaptureMouse;

public: // Methods
	static void Init(VulkanContext* pContext);
	static void Clear();
	static void Update();
	static void ProcessEvent(const SDL_Event& event);
	static void Render(VkCommandBuffer& commandBuffer);
	static bool WantCaptureKeyboard();
	static bool WantCaptureMouse();

private: // Methods
	// Delete all constructors:
	DearImGui() = delete;
	DearImGui(const DearImGui&) = delete;
	DearImGui& operator=(const DearImGui&) = delete;
	~DearImGui() = delete;
};




#endif // __INCLUDE_GUARD_dearImGui_h__