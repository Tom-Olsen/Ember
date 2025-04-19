#ifndef __INCLUDE_GUARD_renderCore_h__
#define __INCLUDE_GUARD_renderCore_h__
#include <array>
#include <memory>
#include <vector>
#include <vulkan/vulkan.h>



namespace emberEngine
{
	// Forward declarations:
	struct DrawCall;
	namespace vulkanBackend
	{
		class CommandPool;
	}



	class RenderCore
	{
	private: // Enums:
		enum RenderStage
		{
			preRenderCompute = 0,
			shadow = 1,
			forward = 2,
			postRenderCompute = 3,
			present = 4,
			stageCount = 5
		};

	private: // Members:
		// Render resources:
		std::vector<vulkanBackend::CommandPool> m_commandPools;

		// Sync objects:
		std::vector<VkFence> m_fences;
		std::vector<VkSemaphore> m_acquireSemaphores;
		std::vector<VkSemaphore> m_preRenderComputeToShadowSemaphores;
		std::vector<VkSemaphore> m_shadowToForwardSemaphores;
		std::vector<VkSemaphore> m_forwardToPostRenderComputeSemaphores;
		std::vector<VkSemaphore> m_postRenderToPresentSemaphores;
		std::vector<VkSemaphore> m_releaseSemaphores;

		// Render management:
		uint32_t m_imageIndex;
		bool m_rebuildSwapchain;

		// Game engine data injection:
		std::vector<DrawCall*>* m_pDrawCalls;

	public: // Methods:
		RenderCore();
		~RenderCore();
		bool RenderFrame();

	private: // Methods:
		void RebuildSwapchain();
		bool AcquireImage();
		
		void RecordPreRenderComputeCommandBuffer();
		void RecordShadowCommandBuffer();
		void RecordForwardCommandBuffer();
		void RecordPostRenderComputeCommandBuffer();
		void RecordPresentCommandBuffer();

		void SubmitCommandBuffers();
		bool PresentImage();

		void CreateFences();
		void CreateSemaphores();
		void DestroyFences();
		void DestroySemaphores();
	};
}



#endif // __INCLUDE_GUARD_renderCore_h__