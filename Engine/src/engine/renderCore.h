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
		// Render resources:
		std::vector<vulkanBackend::CommandPool> m_commandPools;

		// Sync objects:
		std::vector<VkFence> m_preRenderComputeFences;
		std::vector<VkFence> m_shadowFences;
		std::vector<VkFence> m_forwardFences;
		std::vector<VkFence> m_postRenderComputeFences;
		std::vector<VkFence> m_presentFences;
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

		// Wait for fence:
		void WaitForPreRenderComputeFence();
		void WaitForShadowFence();
		void WaitForForwardFence();
		void WaitForPostRenderComputeFence();
		void WaitForPresentFence();
		
		// Record commands:
		void RecordPreRenderComputeCommands();
		void RecordShadowCommands();
		void RecordForwardCommands();
		void RecordForwardCommandsParallel();
		void RecordPostRenderComputeCommands();
		void RecordPresentCommands();

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
		vulkanBackend::CommandPool& GetCommandPool(int frameIndex, RenderStage renderStage);
		vulkanBackend::CommandPool& GetCommandPool(int frameIndex, int renderStage);
	};
}



#endif // __INCLUDE_GUARD_renderCore_h__