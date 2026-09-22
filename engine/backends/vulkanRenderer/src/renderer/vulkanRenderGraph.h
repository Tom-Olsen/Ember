#pragma once
#include "vulkanComputeStage.h"
#include "vulkanDeferredGeometryStage.h"
#include "vulkanDeferredLightingStage.h"
#include "vulkanForwardStage.h"
#include "vulkanGizmoStage.h"
#include "vulkanOutlineStage.h"
#include "vulkanPresentStage.h"
#include "vulkanResourceUpdateStage.h"
#include "vulkanShadowStage.h"
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
	/// Owns the render stages and the synchronization that connects them.
	/// Aquire
	/// └>ResourceUpdate
	///   ├> Gizmo ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┬> Present ─> Release
	///   └> PreRenderCompute ┬> Outline ──────────> MidRenderCompute  ────────────────────────────────────────────────────────────┬> PostRenderCompute ┘
	///                       ├> Shadow ──────────┬> DeferredLighting ─> ScreenSpaceCompute ─> ForwardOpaque ─> ForwardTransparent ┘
	///                       └> DeferredGeometry ┘
	/// </summary>
	class RenderGraph
	{
	private: // Types:
		enum class Dependency : uint8_t
		{
			aquireToResourceUpdate,
			resourceUpdateToGizmo,
			resourceUpdateToPreRenderCompute,
			gizmoToPresent,
			preRenderComputeToOutline,
			preRenderComputeToShadow,
			preRenderComputeToDeferredGeometry,
			outlineToMidRenderCompute,
			midRenderComputeToPostRenderCompute,
			shadowToDeferredLighting,
			deferredGeometryToDeferredLighting,
			deferredLightingToScreenSpaceCompute,
			screenSpaceComputeToForwardOpaque,
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
		// Render stages:
		ResourceUpdateStage m_resourceUpdateStage;
		GizmoStage m_gizmoStage;
		ComputeStage<RenderStage::preRenderCompute> m_preRenderComputeStage;
		OutlineStage m_outlineStage;
		ComputeStage<RenderStage::midRenderCompute> m_midRenderComputeStage;
		ShadowStage m_shadowStage;
		DeferredGeometryStage m_deferredGeometryStage;
		DeferredLightingStage m_deferredLightingStage;
		ComputeStage<RenderStage::screenSpaceCompute> m_screenSpaceComputeStage;
		ForwardStage<RenderStage::forwardOpaque> m_forwardOpaqueStage;
		ForwardStage<RenderStage::forwardTransparent> m_forwardTransparentStage;
		ComputeStage<RenderStage::postRenderCompute> m_postRenderComputeStage;
		PresentStage m_presentStage;

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
		void RecordAndSubmit(const FrameContext& frameContext);
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