#ifndef __INCLUDE_GUARD_sdlDearImGui_h__
#define __INCLUDE_GUARD_sdlDearImGui_h__
#include "dearImGuiFlags.h"
#include "emberMath.h"
#include "iDearImGui.h"
#include "sdlWindowExport.h"
#include <tuple>
#include <unordered_map>
#include <vector>



// Forward declarations:
struct ImGuiIO;
struct SDL_Window;
typedef struct VkInstance_T* VkInstance;
typedef struct VkPhysicalDevice_T* VkPhysicalDevice;
typedef struct VkDevice_T* VkDevice;
typedef struct VkRenderPass_T* VkRenderPass;
typedef struct VkDescriptorPool_T* VkDescriptorPool;
typedef struct VkQueue_T* VkQueue;
typedef struct VkCommandBuffer_T* VkCommandBuffer;
typedef struct VkImageView_T* VkImageView;
typedef struct VkSampler_T* VkSampler;
typedef struct VkDescriptorSet_T* VkDescriptorSet;
typedef struct VkDescriptorSetLayout_T* VkDescriptorSetLayout;



namespace sdlWindowBackend
{
	class SDL_WINDOW_API SdlDearImGui : public emberBackendInterface::IDearImGui
	{
	private: // Members
		VkDevice m_vkDevice;
		VkDescriptorPool m_vkDescriptorPool;
		VkDescriptorSetLayout m_descriptorSetLayout;
		ImGuiIO* m_pIo;
		bool m_wantCaptureKeyboard;
		bool m_wantCaptureMouse;
		bool m_enableDockSpace;
		std::unordered_map<VkImageView, VkDescriptorSet> m_vkImageViewToDescriptorMap;

	public: // Methods
		// Constructor/Destructor:
		SdlDearImGui(void* pSdlWindow, VkInstance vkInstance, VkPhysicalDevice vkPhysicalDevice, VkDevice vkDevice, VkRenderPass vkRenderpass, VkDescriptorPool vkDescriptorPool, VkQueue vkQueue, uint32_t queueFamilyIndex, uint32_t framesInFlight, uint32_t spwachainImageCount, bool enableDockSpace);
		~SdlDearImGui();

		// Non-copyable:
		SdlDearImGui(const SdlDearImGui&) = delete;
		SdlDearImGui& operator=(const SdlDearImGui&) = delete;

		// Movable:
		SdlDearImGui(SdlDearImGui&& other) noexcept;
		SdlDearImGui& operator=(SdlDearImGui&& other) noexcept;

		// Render Logic:
		void Update() override;									// Must be called in main update loop of the engine.
		void ProcessEvent(void* pEvent) override;			// Must be called in from sdl window.
		void Render(VkCommandBuffer vkCommandBuffer) override;	// Must be called in a render pass.

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
		Float2 GetWindowSize() override;
		Float2 GetContentRegionalAvail() override;
		Float2 GetCursorPos() override;
		void SetCursorPos(float localPosX, float localPosY) override;
		void Image(uintptr_t textureID, float imageWidth, float imageHeight, float u0, float v0, float u1, float v1) override;

	private: // Methods:
		void ShowDockSpace();
		void CreateDescriptorSetLayout();
		VkDescriptorSet CreateDescriptorSet();
		void UpdateDescriptor(VkDescriptorSet vkDescriptorSet, VkImageView vkImageView, VkSampler vkSampler);
	};
}



#endif // __INCLUDE_GUARD_sdlDearImGui_h__