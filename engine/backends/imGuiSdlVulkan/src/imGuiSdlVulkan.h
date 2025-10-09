#pragma once
#include "commonGuiFlags.h"
#include "emberMath.h"
#include "iGui.h"
#include "imGuiSdlVulkanExport.h"
#include <unordered_map>



// Forward declarations:
struct ImGuiIO;
typedef struct VkDevice_T* VkDevice;
typedef struct VkDescriptorPool_T* VkDescriptorPool;
typedef struct VkCommandBuffer_T* VkCommandBuffer;
typedef struct VkImageView_T* VkImageView;
typedef struct VkSampler_T* VkSampler;
typedef struct VkDescriptorSet_T* VkDescriptorSet;
typedef struct VkDescriptorSetLayout_T* VkDescriptorSetLayout;
namespace emberBackendInterface
{
	class IWindow;
	class IRenderer;
	class ITexture;
}



namespace imGuiSdlVulkanBackend
{
	class IMGUI_SDL_VULKAN_API Gui : public emberBackendInterface::IGui
	{
	private: // Members
		VkDevice m_vkDevice;
		VkDescriptorPool m_vkDescriptorPool;
		VkDescriptorSetLayout m_descriptorSetLayout;
		VkSampler m_vkColorSampler;
		ImGuiIO* m_pIo;
		bool m_wantCaptureKeyboard;
		bool m_wantCaptureMouse;
		bool m_enableDockSpace;
		std::unordered_map<VkImageView, VkDescriptorSet> m_vkImageViewToDescriptorMap;
		

	public: // Methods
		// Constructor/Destructor:
		Gui(emberBackendInterface::IWindow* pIWindow, emberBackendInterface::IRenderer* pIRenderer, bool enableDockSpace);
		~Gui();

		// Non-copyable:
		Gui(const Gui&) = delete;
		Gui& operator=(const Gui&) = delete;

		// Movable:
		Gui(Gui&& other) noexcept;
		Gui& operator=(Gui&& other) noexcept;

		// Render Logic:
		void Update() override;									// Must be called in main update loop of the engine.
		void ProcessEvent(const void* pWindowEvent) override;	// Must be called in from window.
		void Render(VkCommandBuffer vkCommandBuffer) override;	// Must be called in a render pass.

		// Getters:
		bool WantCaptureKeyboard() override;
		bool WantCaptureMouse() override;
		uintptr_t GetTextureID(emberBackendInterface::ITexture* pITexture) override;

		// Wrappers:
		bool IsWindowFocused(emberCommon::GuiFocusedFlags flags) override;
		bool Begin(const char* name, bool* pOpen, emberCommon::GuiWindowFlags flags) override;
		void End() override;
		void PushID(const char* str_id) override;
		void PopID() override;
		Float2 GetWindowSize() override;
		Float2 GetContentRegionalAvail() override;
		Float2 GetCursorPos() override;
		void SetCursorPos(const Float2& localPos) override;
		void Image(uintptr_t textureID, float imageWidth, float imageHeight, float u0, float v0, float u1, float v1) override;

	private: // Methods:
		void ShowDockSpace();
		void CreateDescriptorSetLayout();
		VkDescriptorSet CreateDescriptorSet();
		void UpdateDescriptor(VkDescriptorSet vkDescriptorSet, VkImageView vkImageView, VkSampler vkSampler);
	};
}