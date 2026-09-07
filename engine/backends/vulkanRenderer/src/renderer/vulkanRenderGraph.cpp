#include "vulkanRenderGraph.h"
#include "logger.h"
#include "profiler.h"
#include "vulkanCommandPool.h"
#include "vulkanContext.h"
#include "vulkanFrameContext.h"
#include "vulkanFrameResources.h"
#include "vulkanLogicalDevice.h"
#include "vulkanMacros.h"
#include "vulkanSwapchain.h"
#include <array>
#include <stdexcept>
#include <string>



namespace vulkanRendererBackend
{
	// Constructor/Destructor:
	RenderGraph::RenderGraph()
	{
		CreateSyncObjects();
	}
	RenderGraph::~RenderGraph()
	{
		DestroySyncObjects();
	}



	// Frame lifecycle:
	VkResult RenderGraph::AcquireImage(uint32_t frameIndex, uint32_t& imageIndex) const
	{
		if (frameIndex >= m_frameSyncObjects.size())
			throw std::out_of_range("RenderGraph::AcquireImage(...) failed. frameIndex is out of range.");
		return vkAcquireNextImageKHR(Context::GetVkDevice(), Context::GetVkSwapchainKHR(), UINT64_MAX, GetDependencySemaphore(frameIndex, Dependency::aquireToResourceUpdate), VK_NULL_HANDLE, &imageIndex);
	}
	void RenderGraph::RecordAndSubmit(const FrameContext& frameContext)
	{
		PROFILE_FUNCTION();
		DEBUG_LOG_TRACE("Recording frame {}", frameContext.frameIndex);

		const uint32_t frameIndex = frameContext.frameIndex;
		if (frameIndex >= m_frameSyncObjects.size())
			throw std::out_of_range("RenderGraph::RecordAndSubmit(...) failed. frameIndex is out of range.");
		if (frameContext.imageIndex >= m_releaseSemaphores.size())
			throw std::out_of_range("RenderGraph::RecordAndSubmit(...) failed. imageIndex is out of range.");

		// Queues:
		const VkQueue graphicsQueue = Context::GetLogicalDevice()->GetGraphicsQueue().queue;
		const VkQueue computeQueue = Context::GetLogicalDevice()->GetComputeQueue().queue;

		// Aquire -> ResourceUpdate -> PreRenderCompute + Gizmo:
		m_resourceUpdateStage.Record(frameContext);
		VkSemaphoreSubmitInfo resourceUpdateWait = CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::aquireToResourceUpdate), VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT);
		std::array<VkSemaphoreSubmitInfo, 2> resourceUpdateSignals =
		{
			CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::resourceUpdateToPreRenderCompute), VK_PIPELINE_STAGE_2_TRANSFER_BIT),
			CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::resourceUpdateToGizmo), VK_PIPELINE_STAGE_2_TRANSFER_BIT)
		};
		SubmitStage(frameContext, RenderStage::resourceUpdate, graphicsQueue, std::span<const VkSemaphoreSubmitInfo>(&resourceUpdateWait, 1), resourceUpdateSignals);

		// ResourceUpdate -> Gizmo -> Present:
		m_gizmoStage.Record(frameContext);
		VkSemaphoreSubmitInfo gizmoWait = CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::resourceUpdateToGizmo), VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT | VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT | VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT);
		VkSemaphoreSubmitInfo gizmoSignal = CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::gizmoToPresent), VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT);
		SubmitStage(frameContext, RenderStage::gizmo, graphicsQueue, std::span<const VkSemaphoreSubmitInfo>(&gizmoWait, 1), std::span<const VkSemaphoreSubmitInfo>(&gizmoSignal, 1));

		// ResourceUpdate -> PreRenderCompute -> Outline + Shadow + DeferredGeometry:
		m_preRenderComputeStage.Record(frameContext, frameContext.preRenderComputeCalls);
		VkSemaphoreSubmitInfo preRenderComputeWait = CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::resourceUpdateToPreRenderCompute), VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT);
		std::array<VkSemaphoreSubmitInfo, 3> preRenderComputeSignals =
		{
			CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::preRenderComputeToShadow), VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT),
			CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::preRenderComputeToDeferredGeometry), VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT),
			CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::preRenderComputeToOutline), VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT)
		};
		SubmitStage(frameContext, RenderStage::preRenderCompute, graphicsQueue, std::span<const VkSemaphoreSubmitInfo>(&preRenderComputeWait, 1), preRenderComputeSignals);

		// PreRenderCompute -> Outline -> MidRenderCompute:
		m_outlineStage.Record(frameContext);
		VkSemaphoreSubmitInfo outlineWait = CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::preRenderComputeToOutline), VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT | VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT | VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT);
		VkSemaphoreSubmitInfo outlineSignal = CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::outlineToMidRenderCompute), VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT);
		SubmitStage(frameContext, RenderStage::outline, graphicsQueue, std::span<const VkSemaphoreSubmitInfo>(&outlineWait, 1), std::span<const VkSemaphoreSubmitInfo>(&outlineSignal, 1));

		// Outline -> MidRenderCompute -> PostRenderCompute:
		m_midRenderComputeStage.Record(frameContext, frameContext.midRenderComputeCalls);
		VkSemaphoreSubmitInfo midRenderComputeWait = CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::outlineToMidRenderCompute), VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT);
		VkSemaphoreSubmitInfo midRenderComputeSignal = CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::midRenderComputeToPostRenderCompute), VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT);
		SubmitStage(frameContext, RenderStage::midRenderCompute, computeQueue, std::span<const VkSemaphoreSubmitInfo>(&midRenderComputeWait, 1), std::span<const VkSemaphoreSubmitInfo>(&midRenderComputeSignal, 1));

		// PreRenderCompute -> Shadow -> DeferredLighting:
		m_shadowStage.Record(frameContext);
		VkSemaphoreSubmitInfo shadowWait = CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::preRenderComputeToShadow), VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT | VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT | VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT);
		VkSemaphoreSubmitInfo shadowSignal = CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::shadowToDeferredLighting), VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT);
		SubmitStage(frameContext, RenderStage::shadow, graphicsQueue, std::span<const VkSemaphoreSubmitInfo>(&shadowWait, 1), std::span<const VkSemaphoreSubmitInfo>(&shadowSignal, 1));

		// PreRenderCompute -> DeferredGeometry -> DeferredLighting:
		m_deferredGeometryStage.Record(frameContext);
		VkSemaphoreSubmitInfo deferredGeometryWait = CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::preRenderComputeToDeferredGeometry), VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT | VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT | VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT);
		VkSemaphoreSubmitInfo deferredGeometrySignal = CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::deferredGeometryToDeferredLighting), VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT);
		SubmitStage(frameContext, RenderStage::deferredGeometry, graphicsQueue, std::span<const VkSemaphoreSubmitInfo>(&deferredGeometryWait, 1), std::span<const VkSemaphoreSubmitInfo>(&deferredGeometrySignal, 1));

		// Shadow + DeferredGeometry -> DeferredLighting -> ForwardOpaque:
		m_deferredLightingStage.Record(frameContext);
		std::array<VkSemaphoreSubmitInfo, 2> deferredLightingWaits =
		{
			CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::shadowToDeferredLighting), VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT),
			CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::deferredGeometryToDeferredLighting), VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT)
		};
		VkSemaphoreSubmitInfo deferredLightingSignal = CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::deferredLightingToForwardOpaque), VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT);
		SubmitStage(frameContext, RenderStage::deferredLighting, graphicsQueue, deferredLightingWaits, std::span<const VkSemaphoreSubmitInfo>(&deferredLightingSignal, 1));

		// DeferredLighting -> ForwardOpaque -> ScreenSpaceCompute:
		m_forwardOpaqueStage.Record(frameContext);
		VkSemaphoreSubmitInfo forwardOpaqueWait = CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::deferredLightingToForwardOpaque), VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT | VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT | VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT);
		VkSemaphoreSubmitInfo forwardOpaqueSignal = CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::forwardOpaqueToScreenSpaceCompute), VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT);
		SubmitStage(frameContext, RenderStage::forwardOpaque, graphicsQueue, std::span<const VkSemaphoreSubmitInfo>(&forwardOpaqueWait, 1), std::span<const VkSemaphoreSubmitInfo>(&forwardOpaqueSignal, 1));

		// ForwardOpaque -> ScreenSpaceCompute -> ForwardTransparent:
		m_screenSpaceComputeStage.Record(frameContext, frameContext.screenSpaceComputeCalls);
		VkSemaphoreSubmitInfo screenSpaceComputeWait = CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::forwardOpaqueToScreenSpaceCompute), VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT);
		VkSemaphoreSubmitInfo screenSpaceComputeSignal = CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::screenSpaceComputeToForwardTransparent), VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT);
		SubmitStage(frameContext, RenderStage::screenSpaceCompute, graphicsQueue, std::span<const VkSemaphoreSubmitInfo>(&screenSpaceComputeWait, 1), std::span<const VkSemaphoreSubmitInfo>(&screenSpaceComputeSignal, 1));

		// ScreenSpaceCompute -> ForwardTransparent -> PostRenderCompute:
		m_forwardTransparentStage.Record(frameContext);
		VkSemaphoreSubmitInfo forwardTransparentWait = CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::screenSpaceComputeToForwardTransparent), VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT | VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT | VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT);
		VkSemaphoreSubmitInfo forwardTransparentSignal = CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::forwardTransparentToPostRenderCompute), VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT);
		SubmitStage(frameContext, RenderStage::forwardTransparent, graphicsQueue, std::span<const VkSemaphoreSubmitInfo>(&forwardTransparentWait, 1), std::span<const VkSemaphoreSubmitInfo>(&forwardTransparentSignal, 1));

		// MidRenderCompute + ForwardTransparent -> PostRenderCompute  -> Present:
		m_postRenderComputeStage.Record(frameContext, frameContext.postRenderComputeCalls);
		std::array<VkSemaphoreSubmitInfo, 2> postRenderComputeWaits =
		{
			CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::midRenderComputeToPostRenderCompute), VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT),
			CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::forwardTransparentToPostRenderCompute), VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT)
		};
		VkSemaphoreSubmitInfo postRenderComputeSignal = CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::postRenderComputeToPresent), VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT);
		SubmitStage(frameContext, RenderStage::postRenderCompute, graphicsQueue, postRenderComputeWaits, std::span<const VkSemaphoreSubmitInfo>(&postRenderComputeSignal, 1));

		// Gizmo + PostRenderCompute -> Present -> Release:
		m_presentStage.Record(frameContext);
		std::array<VkSemaphoreSubmitInfo, 2> presentWaits =
		{
			CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::postRenderComputeToPresent), VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT),
			CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::gizmoToPresent), VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT)
		};
		VkSemaphoreSubmitInfo presentSignal = CreateSemaphoreSubmitInfo(m_releaseSemaphores[frameContext.imageIndex], VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT);
		SubmitStage(frameContext, RenderStage::present, graphicsQueue, presentWaits, std::span<const VkSemaphoreSubmitInfo>(&presentSignal, 1), m_frameSyncObjects[frameIndex].frameFence);
	}
	VkResult RenderGraph::Present(uint32_t imageIndex) const
	{
		PROFILE_FUNCTION();
		if (imageIndex >= m_releaseSemaphores.size())
			throw std::out_of_range("RenderGraph::Present(...) failed. imageIndex is out of range.");

		VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &m_releaseSemaphores[imageIndex];
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &Context::GetVkSwapchainKHR();
		presentInfo.pImageIndices = &imageIndex;
		return vkQueuePresentKHR(Context::GetLogicalDevice()->GetPresentQueue().queue, &presentInfo);
	}
	void RenderGraph::RecreateSyncObjects()
	{
		DestroySyncObjects();
		CreateSyncObjects();
	}
	void RenderGraph::ResetFrameFence(uint32_t frameIndex) const
	{
		if (frameIndex >= m_frameSyncObjects.size())
			throw std::out_of_range("RenderGraph::ResetFrameFence(...) failed. frameIndex is out of range.");
		VKA(vkResetFences(Context::GetVkDevice(), 1, &m_frameSyncObjects[frameIndex].frameFence));
	}
	void RenderGraph::WaitForFrame(uint32_t frameIndex) const
	{
		if (frameIndex >= m_frameSyncObjects.size())
		{
			LOG_ERROR("RenderGraph::WaitForFrame(...) failed. frameIndex '{}' out of range.", frameIndex);
			return;
		}
		VKA(vkWaitForFences(Context::GetVkDevice(), 1, &m_frameSyncObjects[frameIndex].frameFence, VK_TRUE, UINT64_MAX));
	}



	// Getters:
	bool RenderGraph::IsFrameFinished(uint32_t frameIndex) const
	{
		if (frameIndex >= m_frameSyncObjects.size())
		{
			LOG_ERROR("RenderGraph::IsFrameFinished(...) failed. frameIndex '{}' out of range.", frameIndex);
			return true;
		}

		VkResult result = vkGetFenceStatus(Context::GetVkDevice(), m_frameSyncObjects[frameIndex].frameFence);
		if (result == VK_SUCCESS)
			return true;
		if (result == VK_NOT_READY)
			return false;
		VKA(result);
		return false;
	}



	// Private methods:
	// Submission:
	VkSemaphoreSubmitInfo RenderGraph::CreateSemaphoreSubmitInfo(VkSemaphore semaphore, VkPipelineStageFlags2 stageMask) const
	{
		VkSemaphoreSubmitInfo semaphoreSubmitInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO };
		semaphoreSubmitInfo.semaphore = semaphore;
		semaphoreSubmitInfo.stageMask = stageMask;
		return semaphoreSubmitInfo;
	}
	void RenderGraph::SubmitStage(const FrameContext& frameContext, RenderStage renderStage, VkQueue queue, std::span<const VkSemaphoreSubmitInfo> waitSemaphoreInfos, std::span<const VkSemaphoreSubmitInfo> signalSemaphoreInfos, VkFence fence) const
	{
		CommandPool& commandPool = frameContext.resources.GetCommandPool(renderStage);
		VkCommandBufferSubmitInfo commandBufferInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO };
		commandBufferInfo.commandBuffer = commandPool.GetPrimaryVkCommandBuffer();

		VkSubmitInfo2 submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO_2 };
		submitInfo.waitSemaphoreInfoCount = static_cast<uint32_t>(waitSemaphoreInfos.size());
		submitInfo.pWaitSemaphoreInfos = waitSemaphoreInfos.data();
		submitInfo.commandBufferInfoCount = 1;
		submitInfo.pCommandBufferInfos = &commandBufferInfo;
		submitInfo.signalSemaphoreInfoCount = static_cast<uint32_t>(signalSemaphoreInfos.size());
		submitInfo.pSignalSemaphoreInfos = signalSemaphoreInfos.data();

		VKA(vkQueueSubmit2(queue, 1, &submitInfo, fence));
	}



	// Sync object creation/destruction:
	void RenderGraph::CreateSyncObjects()
	{
		// Per frame in flight sync objects:
		m_frameSyncObjects.resize(Context::GetFramesInFlight());
		for (uint32_t frameIndex = 0; frameIndex < m_frameSyncObjects.size(); frameIndex++)
		{
			FrameSyncObjects& syncObjects = m_frameSyncObjects[frameIndex];

			// Frame fence:
			{
				VkFenceCreateInfo fenceCreateInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
				fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
				VKA(vkCreateFence(Context::GetVkDevice(), &fenceCreateInfo, nullptr, &syncObjects.frameFence));
				NAME_VK_OBJECT(syncObjects.frameFence, "Fence_Frame" + std::to_string(frameIndex));
			}

			// Inter stage semaphores:
			for (size_t dependencyIndex = 0; dependencyIndex < static_cast<size_t>(Dependency::dependencyCount); dependencyIndex++)
			{
				VkSemaphoreCreateInfo semaphoreCreateInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
				VkSemaphore& semaphore = syncObjects.dependencySemaphores[dependencyIndex];
				VKA(vkCreateSemaphore(Context::GetVkDevice(), &semaphoreCreateInfo, nullptr, &semaphore));
				std::string dependencyName = GetDependencyName(static_cast<Dependency>(dependencyIndex));
				NAME_VK_OBJECT(semaphore, "Semaphore_" + dependencyName + "_Frame" + std::to_string(frameIndex));
			}
		}

		// Per swapchain image release semaphore:
		m_releaseSemaphores.resize(Context::GetSwapchain()->GetImageCount());
		for (uint32_t imageIndex = 0; imageIndex < m_releaseSemaphores.size(); imageIndex++)
		{
			VkSemaphoreCreateInfo semaphoreCreateInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
			VkSemaphore& semaphore = m_releaseSemaphores[imageIndex];
			VKA(vkCreateSemaphore(Context::GetVkDevice(), &semaphoreCreateInfo, nullptr, &semaphore));
			NAME_VK_OBJECT(semaphore, "Semaphore_Release_SwapchainImage" + std::to_string(imageIndex));
		}
	}
	void RenderGraph::DestroySyncObjects()
	{
		// Per frame in flight:
		for (FrameSyncObjects& syncObjects : m_frameSyncObjects)
		{
			for (VkSemaphore semaphore : syncObjects.dependencySemaphores)
				vkDestroySemaphore(Context::GetVkDevice(), semaphore, nullptr);
			vkDestroyFence(Context::GetVkDevice(), syncObjects.frameFence, nullptr);
		}
		// Per swapchain image:
		for (VkSemaphore semaphore : m_releaseSemaphores)
			vkDestroySemaphore(Context::GetVkDevice(), semaphore, nullptr);

		m_frameSyncObjects.clear();
		m_releaseSemaphores.clear();
	}



	// Getters:
	VkSemaphore RenderGraph::GetDependencySemaphore(uint32_t frameIndex, Dependency dependency) const
	{
		if (frameIndex >= m_frameSyncObjects.size())
			throw std::out_of_range("RenderGraph::GetDependencySemaphore(...) failed. frameIndex is out of range.");
		size_t dependencyIndex = static_cast<size_t>(dependency);
		if (dependencyIndex >= static_cast<size_t>(Dependency::dependencyCount))
			throw std::out_of_range("RenderGraph::GetDependencySemaphore(...) failed. dependency is out of range.");
		return m_frameSyncObjects[frameIndex].dependencySemaphores[dependencyIndex];
	}
	const char* RenderGraph::GetDependencyName(Dependency dependency) const
	{
		switch (dependency)
		{
			case Dependency::aquireToResourceUpdate: return "AquireToResourceUpdate";
			case Dependency::resourceUpdateToGizmo: return "ResourceUpdateToGizmo";
			case Dependency::resourceUpdateToPreRenderCompute: return "ResourceUpdateToPreRenderCompute";
			case Dependency::gizmoToPresent: return "GizmoToPresent";
			case Dependency::preRenderComputeToOutline: return "PreRenderComputeToOutline";
			case Dependency::preRenderComputeToShadow: return "PreRenderComputeToShadow";
			case Dependency::preRenderComputeToDeferredGeometry: return "PreRenderComputeToDeferredGeometry";
			case Dependency::outlineToMidRenderCompute: return "OutlineToMidRenderCompute";
			case Dependency::midRenderComputeToPostRenderCompute: return "MidRenderComputeToPostRenderCompute";
			case Dependency::shadowToDeferredLighting: return "ShadowToDeferredLighting";
			case Dependency::deferredGeometryToDeferredLighting: return "DeferredGeometryToDeferredLighting";
			case Dependency::deferredLightingToForwardOpaque: return "DeferredLightingToForwardOpaque";
			case Dependency::forwardOpaqueToScreenSpaceCompute: return "ForwardOpaqueToScreenSpaceCompute";
			case Dependency::screenSpaceComputeToForwardTransparent: return "ScreenSpaceComputeToForwardTransparent";
			case Dependency::forwardTransparentToPostRenderCompute: return "ForwardTransparentToPostRenderCompute";
			case Dependency::postRenderComputeToPresent: return "PostRenderComputeToPresent";
			default: throw std::out_of_range("RenderGraph::GetDependencyName(...) failed. dependency is out of range.");
		}
	}
}