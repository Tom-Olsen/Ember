#pragma once
#include "iRenderer.h"
#include "vulkanRendererExport.h"
#include <array>
#include <memory>
#include <vector>
#include <vulkan/vulkan.h>



// Forward declarations:
namespace emberEngine
{
	struct RendererCreateInfo;
}
namespace emberBackendInterface
{
	class IDearImGui;
}



namespace vulkanRendererBackend
{
	// Forward declarations:
	struct DrawCall;
	class Mesh;
	class Material;
	class ShaderProperties;
	class CommandPool;



	class VULKAN_RENDERER_API Renderer : public emberBackendInterface::IRenderer
	{
	private: // Enums:
		enum class RenderStage
		{
			preRenderCompute = 0,
			shadow = 1,
			forward = 2,
			postRenderCompute = 3,
			present = 4,
			stageCount = 5
		};
		inline static constexpr std::array<const char*, (int)RenderStage::stageCount> renderStageNames =
		{
			"preRenderCompute",
			"shadow",
			"forward",
			"postRenderCompute",
			"present"
		};

	private: // Members:
		// Backend hooks:
		emberBackendInterface::IDearImGui* m_pIDearImGui;
		
		// Render resources:
		std::vector<CommandPool> m_commandPools;

		// Sync objects:
		std::vector<VkFence> m_frameFences;
		std::vector<VkSemaphore> m_acquireSemaphores;
		std::vector<VkSemaphore> m_preRenderComputeToShadowSemaphores;
		std::vector<VkSemaphore> m_shadowToForwardSemaphores;
		std::vector<VkSemaphore> m_forwardToPostRenderComputeSemaphores;
		std::vector<VkSemaphore> m_postRenderToPresentSemaphores;
		std::vector<VkSemaphore> m_releaseSemaphores;

		// Shadow render pass caching:
		Material* m_pShadowMaterial;
		VkPipeline m_shadowPipeline;
		VkPipelineLayout m_shadowPipelineLayout;

		// Present render pass caching:
		Mesh* m_pPresentMesh;
		Material* m_pPresentMaterial;
		std::unique_ptr<ShaderProperties> m_pPresentShaderProperties;
		VkPipeline m_presentPipeline = VK_NULL_HANDLE;
		VkPipelineLayout m_presentPipelineLayout = VK_NULL_HANDLE;
		uint32_t m_presentBindingCount = 0;

		// Render management:
		uint32_t m_imageIndex;  // updated by vkAcquireNextImageKHR(...)
		float m_time;
		float m_deltaTime;
		bool m_rebuildSwapchain;

	public: // Methods:
		Renderer(const emberEngine::RendererCreateInfo& createInfo);
		~Renderer();

		// Non-copyable:
		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;

		// Non-movable:
		Renderer(Renderer&& other) noexcept = delete;
		Renderer& operator=(Renderer&& other) noexcept = delete;

		void RenderFrame(int windowWidth, int windowHeight, float time, float deltaTime) override;

	private: // Methods:
		void RebuildSwapchain();
		bool AcquireImage();

		// Wait for previous frame:
		void WaitForFrameFence();
		void ResetCommandPools();

		// Record commands:
		void RecordPreRenderComputeCommands();
		void RecordShadowCommands();
		void RecordForwardCommands();
		void RecordForwardCommandsParallel();
		void RecordPostRenderComputeCommands();
		void RecordPresentCommands();
		void RecordImGuiPresentCommands();

		// Submit commands:
		void SubmitPreRenderComputeCommands();
		void SubmitShadowCommands();
		void SubmitForwardCommands();
		void SubmitForwardCommandsParallel();
		void SubmitPostRenderComputeCommands();
		void SubmitPresentCommands();

		bool PresentImage();

		void CreateFences();
		void CreateSemaphores();
		void DestroyFences();
		void DestroySemaphores();
		CommandPool& GetCommandPool(int frameIndex, RenderStage renderStage);
		CommandPool& GetCommandPool(int frameIndex, int renderStage);
	};
}