#ifndef __INCLUDE_GUARD_vulkanRenderer_h__
#define __INCLUDE_GUARD_vulkanRenderer_h__
#include <array>
#include <memory>
#include <vector>
#include <vulkan/vulkan.h>



namespace emberEngine
{
	// Forward declarations:
	struct ComputeCall;
	struct DrawCall;
	class Material;
	class Scene;
	struct VulkanContext;
	class VulkanCommand;
	class RenderTexture2d;
	class ShaderProperties;



	class VulkanRenderer
	{
	private: // Members:
		VulkanContext* m_pContext;
		Material* m_pShadowMaterial;

		// Render resources:
		std::unique_ptr<RenderTexture2d> m_renderTexture;	// maybe move somewhere else? Maybe as input to the renderer?
		std::vector<VulkanCommand> m_syncComputeCommands;	// synced to run before any graphics commands, runs on graphics queue.
		std::vector<VulkanCommand> m_shadowCommands;
		std::vector<VulkanCommand> m_forwardCommands;

		// Sync objects:
		std::vector<VkFence> m_fences;
		std::vector<VkSemaphore> m_acquireSemaphores;
		std::vector<VkSemaphore> m_syncComputeToShadowSemaphores;
		std::vector<VkSemaphore> m_shadowToForwardSemaphores;
		std::vector<VkSemaphore> m_releaseSemaphores;

		// Render management:
		uint32_t m_imageIndex;
		bool m_rebuildSwapchain;
		std::vector<DrawCall*>* m_pDrawCalls;
		std::vector<ComputeCall*>* m_pSyncComputeCalls;

	public: // Methods:
		VulkanRenderer(VulkanContext* pContext);
		~VulkanRenderer();
		bool RenderFrame(Scene* pScene);
		const VulkanContext* const GetContext() const;

	private: // Methods:
		void RebuildSwapchain();
		bool AcquireImage();
		void RecordComputeShaders(Scene* pScene);
		void RecordShadowCommandBuffer(Scene* pScene);
		void RecordForwardCommandBuffer(Scene* pScene);
		void SubmitCommandBuffers();
		bool PresentImage();
		void SetViewportAndScissor(VkCommandBuffer& commandBuffer);
		void CreateFences();
		void CreateSemaphores();
		void DestroyFences();
		void DestroySemaphores();
	};
}



#endif // __INCLUDE_GUARD_vulkanRenderer_h__