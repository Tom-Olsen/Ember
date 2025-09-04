#ifndef __INCLUDE_GUARD_nullDearImGui_h__
#define __INCLUDE_GUARD_nullDearImGui_h__
#include "dearImGuiFlags.h"
#include "iDearImGui.h"
#include "nullWindowExport.h"
#include <tuple>
#include <unordered_map>
#include <vector>



// Forward declarations:
struct ImGuiIO;



namespace emberBackend
{
	class NULL_WINDOW_API NullDearImGuiInstanceExtensionsLoader : public emberBackendInterface::IDearImGuiInstanceExtensionsLoader
	{
	public:
		void AddExtensions(std::vector<const char*>& instanceExtensions) const override;
	};



	class NULL_WINDOW_API NullDearImGui : public emberBackendInterface::IDearImGui
	{
	public: // Methods
		// Constructor/Destructor:
		NullDearImGui();
		~NullDearImGui();

		// Non-copyable:
		NullDearImGui(const NullDearImGui&) = delete;
		NullDearImGui& operator=(const NullDearImGui&) = delete;

		// Movable:
		NullDearImGui(NullDearImGui&& other) noexcept;
		NullDearImGui& operator=(NullDearImGui&& other) noexcept;

		// Render Logic:
		void Update() override;									// Must be called in main update loop of the engine.
		void ProcessEvent(void* pEvent) override;				// Must be called in SdlWindow::HandleEvents() before handing event over to event system.
		void Render(VkCommandBuffer vkCommandBuffer) override;	// Must be called in main render loop before vkCmdEndRenderPass(...).

		// Getters:
		bool WantCaptureKeyboard() override;
		bool WantCaptureMouse() override;
		uintptr_t GetTextureID(VkImageView vkImageView, VkSampler vkSampler) override;

		// Wrappers:
		bool IsWindowFocused(emberEngine::DearImGuiFocusedFlags flags) override;
		bool Begin(const char* name, bool* pOpen, emberEngine::DearImGuiWindowFlags flags) override;
		void End() override;
		void PushID(const char* str_id) override;
		void PopID() override;
		std::tuple<float, float> GetWindowSize() override;
		std::tuple<float, float> GetContentRegionalAvail() override;
		std::tuple<float, float> GetCursorPos() override;
		void SetCursorPos(float localPosX, float localPosY) override;
		void Image(uintptr_t textureID, float imageWidth, float imageHeight, float u0, float v0, float u1, float v1) override;
	};
}



#endif // __INCLUDE_GUARD_nullDearImGui_h__