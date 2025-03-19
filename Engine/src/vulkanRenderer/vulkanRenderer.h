#ifndef __INCLUDE_GUARD_vulkanRenderer_h__
#define __INCLUDE_GUARD_vulkanRenderer_h__
#include <array>
#include <memory>
#include <vector>
#include <vulkan/vulkan.h>



namespace emberEngine
{
	// Forward declarations:
	struct DrawCall;
	class VulkanCommand;



	class VulkanRenderer
	{
	private: // Members:
		// Render resources:
		std::vector<VulkanCommand> m_syncComputeCommands;			// executes before any graphics commands, runs on graphics queue.
		std::vector<VulkanCommand> m_shadowCommands;
		std::vector<VulkanCommand> m_forwardCommands;
		std::vector<VulkanCommand> m_postProcessComputeCommands;	// executes after all graphics commands, runs on graphics queue.
		std::vector<VulkanCommand> m_presentCommands;

		// Sync objects:
		std::vector<VkFence> m_fences;
		std::vector<VkSemaphore> m_acquireSemaphores;
		std::vector<VkSemaphore> m_syncComputeToShadowSemaphores;
		std::vector<VkSemaphore> m_shadowToForwardSemaphores;
		std::vector<VkSemaphore> m_forwardToPostProcessSemaphores;
		std::vector<VkSemaphore> m_postProcessToPresentSemaphores;
		std::vector<VkSemaphore> m_releaseSemaphores;

		// Render management:
		uint32_t m_imageIndex;
		bool m_rebuildSwapchain;

		// Game engine data injection:
		std::vector<DrawCall*>* m_pDrawCalls;

	public: // Methods:
		VulkanRenderer();
		~VulkanRenderer();
		bool RenderFrame();

	private: // Methods:
		void RebuildSwapchain();
		bool AcquireImage();
		
		void RecordComputeCommandBuffer();
		void RecordShadowCommandBuffer();
		void RecordForwardCommandBuffer();
		void RecordPostProcessComputeCommandBuffer();
		void RecordPresentCommandBuffer();

		void SubmitCommandBuffers();
		bool PresentImage();

		void CreateFences();
		void CreateSemaphores();
		void DestroyFences();
		void DestroySemaphores();
	};
}



#endif // __INCLUDE_GUARD_vulkanRenderer_h__