#pragma once
#include "vulkanRenderStage.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Forward declarations:
	struct FrameContext;



	/// <summary>
	/// Coordinates render stage recording and owns the synchronization that connects the stages.
	/// Acquire ──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┐
	/// ResourceUpdate                                                                                                                                                        │
	/// ├> Gizmo ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┼> (Skip)Present ─> FrameFence
	/// └> PreRenderCompute ┬> Outline ──────────> PostOutlineCompute  ──────────────────────────────────────────────────────────────────────────────────┬> PostRenderCompute ┘
	///                     ├> Shadow ─────────────────────────────────┬> DeferredLighting ─> PostLightingCompute ─> ForwardOpaque ─> ForwardTransparent ┘
	///                     └> DeferredGeometry ─> PostGeometryCompute ┘
	///
	/// PresentStage with image: Acquire ─> Record ─> Release ─> QueuePresent
	/// PresentStage without image: SkipPresent (no acquisition wait or release signal)
	/// Both paths wait for Gizmo and PostRenderCompute and signal FrameFence.
	/// </summary>
	class RenderGraph
	{
	private: // Types:
		enum class Dependency : uint8_t
		{
			acquireToPresent,
			resourceUpdateToGizmo,
			resourceUpdateToPreRenderCompute,
			gizmoToPresent,
			preRenderComputeToOutline,
			preRenderComputeToShadow,
			preRenderComputeToDeferredGeometry,
			outlineToPostOutlineCompute,
			postOutlineComputeToPostRenderCompute,
			shadowToDeferredLighting,
			deferredGeometryToPostGeometryCompute,
			postGeometryComputeToDeferredLighting,
			deferredLightingToPostLightingCompute,
			postLightingComputeToForwardOpaque,
			forwardOpaqueToForwardTransparent,
			forwardTransparentToPostRenderCompute,
			postRenderComputeToPresent,
			dependencyCount
		};
		struct FrameSyncObjects
		{
			VkFence frameFence = VK_NULL_HANDLE;
			std::array<VkSemaphore, static_cast<size_t>(Dependency::dependencyCount)> dependencySemaphores{};
		};

	private: // Members:
		// Synchronization:
		std::vector<FrameSyncObjects> m_frameSyncObjects;	// one per frame in flight.
		std::vector<VkSemaphore> m_releaseSemaphores;		// one per swapchain image.

	public: // Methods:
		// Constructor/Destructor:
		RenderGraph();
		~RenderGraph();

		// Non-copyable:
		RenderGraph(const RenderGraph&) = delete;
		RenderGraph& operator=(const RenderGraph&) = delete;

		// Movable:
		RenderGraph(RenderGraph&& other) noexcept = default;
		RenderGraph& operator=(RenderGraph&& other) noexcept = default;

		// Frame lifecycle:
		VkResult AcquireImage(uint32_t frameIndex, uint32_t& imageIndex) const;
		void RecordAndSubmit(const FrameContext& frameContext, bool hasSwapchainImage);
		VkResult Present(uint32_t imageIndex) const;
		void RecreateSyncObjects();
		void ResetFrameFence(uint32_t frameIndex) const;
		void WaitForFrame(uint32_t frameIndex) const;

		// Getters:
		bool IsFrameFinished(uint32_t frameIndex) const;

	private: // Methods:
		// Submission:
		VkSemaphoreSubmitInfo CreateSemaphoreSubmitInfo(VkSemaphore semaphore, VkPipelineStageFlags2 stageMask) const;
		void SubmitStage(const FrameContext& frameContext, RenderStage renderStage, VkQueue queue, std::span<const VkSemaphoreSubmitInfo> waitSemaphoreInfos, std::span<const VkSemaphoreSubmitInfo> signalSemaphoreInfos, VkFence fence = VK_NULL_HANDLE) const;

		// Sync object creation/destruction:
		void CreateSyncObjects();
		void DestroySyncObjects();

		// Getters:
		VkSemaphore GetDependencySemaphore(uint32_t frameIndex, Dependency dependency) const;
		const char* GetDependencyName(Dependency dependency) const;
	};
}