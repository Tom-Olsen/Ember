#include "vulkanRenderGraph.h"
#include "logger.h"
#include "profiler.h"
#include "vulkanCommandPool.h"
#include "vulkanComputeStage.h"
#include "vulkanContext.h"
#include "vulkanDeferredGeometryStage.h"
#include "vulkanDeferredLightingStage.h"
#include "vulkanDeviceQueue.h"
#include "vulkanForwardStage.h"
#include "vulkanFrameContext.h"
#include "vulkanFrameResources.h"
#include "vulkanGizmoStage.h"
#include "vulkanLogicalDevice.h"
#include "vulkanMacros.h"
#include "vulkanOutlineStage.h"
#include "vulkanPresentStage.h"
#include "vulkanResourceUpdateStage.h"
#include "vulkanShadowStage.h"
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
		return vkAcquireNextImageKHR(Context::GetVkDevice(), Context::GetVkSwapchainKHR(), UINT64_MAX, GetDependencySemaphore(frameIndex, Dependency::acquireToPresent), VK_NULL_HANDLE, &imageIndex);
	}
	void RenderGraph::RecordAndSubmit(const FrameContext& frameContext, bool hasSwapchainImage)
	{
		PROFILE_FUNCTION();
		DEBUG_LOG_TRACE("Recording frame {}", frameContext.frameExecutionData.frameIndex);

		const uint32_t frameIndex = frameContext.frameExecutionData.frameIndex;
		if (frameIndex >= m_frameSyncObjects.size())
			throw std::out_of_range("RenderGraph::RecordAndSubmit(...) failed. frameIndex is out of range.");
		if (hasSwapchainImage && frameContext.frameExecutionData.imageIndex >= m_releaseSemaphores.size())
			throw std::out_of_range("RenderGraph::RecordAndSubmit(...) failed. imageIndex is out of range.");

		// Queues:
		const VkQueue graphicsQueue = Context::GetLogicalDevice()->GetGraphicsQueue().queue;
		const VkQueue computeQueue = Context::GetLogicalDevice()->GetComputeQueues().front()->queue;

		// ResourceUpdate -> PreRenderCompute + Gizmo:
		ResourceUpdateStage::Record(frameContext);
		std::array<VkSemaphoreSubmitInfo, 2> resourceUpdateSignals =
		{
			CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::resourceUpdateToPreRenderCompute), VK_PIPELINE_STAGE_2_TRANSFER_BIT),
			CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::resourceUpdateToGizmo), VK_PIPELINE_STAGE_2_TRANSFER_BIT)
		};
		SubmitStage(frameContext, RenderStage::resourceUpdate, graphicsQueue, {}, resourceUpdateSignals);

		// ResourceUpdate -> Gizmo -> Present:
		GizmoStage::Record(frameContext);
		VkSemaphoreSubmitInfo gizmoWait = CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::resourceUpdateToGizmo), VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT | VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT);
		VkSemaphoreSubmitInfo gizmoSignal = CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::gizmoToPresent), VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT);
		SubmitStage(frameContext, RenderStage::gizmo, graphicsQueue, std::span<const VkSemaphoreSubmitInfo>(&gizmoWait, 1), std::span<const VkSemaphoreSubmitInfo>(&gizmoSignal, 1));

		// ResourceUpdate -> PreRenderCompute -> Outline + Shadow + DeferredGeometry:
		ComputeStage::Record<RenderStage::preRenderCompute>(frameContext, frameContext.preRenderComputeCalls);
		VkSemaphoreSubmitInfo preRenderComputeWait = CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::resourceUpdateToPreRenderCompute), VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_2_COPY_BIT);
		std::array<VkSemaphoreSubmitInfo, 3> preRenderComputeSignals =
		{
			CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::preRenderComputeToShadow), VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_2_COPY_BIT),
			CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::preRenderComputeToDeferredGeometry), VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_2_COPY_BIT),
			CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::preRenderComputeToOutline), VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_2_COPY_BIT)
		};
		SubmitStage(frameContext, RenderStage::preRenderCompute, graphicsQueue, std::span<const VkSemaphoreSubmitInfo>(&preRenderComputeWait, 1), preRenderComputeSignals);

		// PreRenderCompute -> Outline -> PostOutlineCompute:
		OutlineStage::Record(frameContext);
		VkSemaphoreSubmitInfo outlineWait = CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::preRenderComputeToOutline), VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT | VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT | VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT);
		VkSemaphoreSubmitInfo outlineSignal = CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::outlineToPostOutlineCompute), VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT);
		SubmitStage(frameContext, RenderStage::outline, graphicsQueue, std::span<const VkSemaphoreSubmitInfo>(&outlineWait, 1), std::span<const VkSemaphoreSubmitInfo>(&outlineSignal, 1));

		// Outline -> PostOutlineCompute -> PostRenderCompute:
		ComputeStage::Record<RenderStage::postOutlineCompute>(frameContext, frameContext.postOutlineComputeCalls);
		VkSemaphoreSubmitInfo postOutlineComputeWait = CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::outlineToPostOutlineCompute), VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_2_COPY_BIT);
		VkSemaphoreSubmitInfo postOutlineComputeSignal = CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::postOutlineComputeToPostRenderCompute), VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_2_COPY_BIT);
		SubmitStage(frameContext, RenderStage::postOutlineCompute, computeQueue, std::span<const VkSemaphoreSubmitInfo>(&postOutlineComputeWait, 1), std::span<const VkSemaphoreSubmitInfo>(&postOutlineComputeSignal, 1));

		// PreRenderCompute -> Shadow -> DeferredLighting:
		ShadowStage::Record(frameContext);
		VkSemaphoreSubmitInfo shadowWait = CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::preRenderComputeToShadow), VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT | VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT | VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT);
		VkSemaphoreSubmitInfo shadowSignal = CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::shadowToDeferredLighting), VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT);
		SubmitStage(frameContext, RenderStage::shadow, graphicsQueue, std::span<const VkSemaphoreSubmitInfo>(&shadowWait, 1), std::span<const VkSemaphoreSubmitInfo>(&shadowSignal, 1));

		// PreRenderCompute -> DeferredGeometry -> PostGeometryCompute:
		DeferredGeometryStage::Record(frameContext);
		VkSemaphoreSubmitInfo deferredGeometryWait = CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::preRenderComputeToDeferredGeometry), VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT | VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT | VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT);
		VkSemaphoreSubmitInfo deferredGeometrySignal = CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::deferredGeometryToPostGeometryCompute), VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT);
		SubmitStage(frameContext, RenderStage::deferredGeometry, graphicsQueue, std::span<const VkSemaphoreSubmitInfo>(&deferredGeometryWait, 1), std::span<const VkSemaphoreSubmitInfo>(&deferredGeometrySignal, 1));

		// DeferredGeometry -> PostGeometryCompute -> DeferredLighting:
		ComputeStage::Record<RenderStage::postGeometryCompute>(frameContext, frameContext.postGeometryComputeCalls);
		VkSemaphoreSubmitInfo postGeometryComputeWait = CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::deferredGeometryToPostGeometryCompute), VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_2_COPY_BIT | VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT);
		VkSemaphoreSubmitInfo postGeometryComputeSignal = CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::postGeometryComputeToDeferredLighting), VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_2_COPY_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT);
		SubmitStage(frameContext, RenderStage::postGeometryCompute, graphicsQueue, std::span<const VkSemaphoreSubmitInfo>(&postGeometryComputeWait, 1), std::span<const VkSemaphoreSubmitInfo>(&postGeometryComputeSignal, 1));

		// Shadow + PostGeometryCompute -> DeferredLighting -> PostLightingCompute:
		DeferredLightingStage::Record(frameContext);
		std::array<VkSemaphoreSubmitInfo, 2> deferredLightingWaits =
		{
			CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::shadowToDeferredLighting), VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT),
			CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::postGeometryComputeToDeferredLighting), VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT)
		};
		VkSemaphoreSubmitInfo deferredLightingSignal = CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::deferredLightingToPostLightingCompute), VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT);
		SubmitStage(frameContext, RenderStage::deferredLighting, graphicsQueue, deferredLightingWaits, std::span<const VkSemaphoreSubmitInfo>(&deferredLightingSignal, 1));

		// DeferredLighting -> PostLightingCompute -> ForwardOpaque:
		ComputeStage::Record<RenderStage::postLightingCompute>(frameContext, frameContext.postLightingComputeCalls);
		VkSemaphoreSubmitInfo postLightingComputeWait = CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::deferredLightingToPostLightingCompute), VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_2_COPY_BIT | VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT);
		VkSemaphoreSubmitInfo postLightingComputeSignal = CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::postLightingComputeToForwardOpaque), VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_2_COPY_BIT);
		SubmitStage(frameContext, RenderStage::postLightingCompute, graphicsQueue, std::span<const VkSemaphoreSubmitInfo>(&postLightingComputeWait, 1), std::span<const VkSemaphoreSubmitInfo>(&postLightingComputeSignal, 1));

		// PostLightingCompute -> ForwardOpaque -> ForwardTransparent:
		ForwardStage<RenderStage::forwardOpaque>::Record(frameContext);
		VkSemaphoreSubmitInfo forwardOpaqueWait = CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::postLightingComputeToForwardOpaque), VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT | VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT | VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT);
		VkSemaphoreSubmitInfo forwardOpaqueSignal = CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::forwardOpaqueToForwardTransparent), VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT);
		SubmitStage(frameContext, RenderStage::forwardOpaque, graphicsQueue, std::span<const VkSemaphoreSubmitInfo>(&forwardOpaqueWait, 1), std::span<const VkSemaphoreSubmitInfo>(&forwardOpaqueSignal, 1));

		// ForwardOpaque -> ForwardTransparent -> PostRenderCompute:
		ForwardStage<RenderStage::forwardTransparent>::Record(frameContext);
		VkSemaphoreSubmitInfo forwardTransparentWait = CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::forwardOpaqueToForwardTransparent), VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT);
		VkSemaphoreSubmitInfo forwardTransparentSignal = CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::forwardTransparentToPostRenderCompute), VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT);
		SubmitStage(frameContext, RenderStage::forwardTransparent, graphicsQueue, std::span<const VkSemaphoreSubmitInfo>(&forwardTransparentWait, 1), std::span<const VkSemaphoreSubmitInfo>(&forwardTransparentSignal, 1));

		// PostOutlineCompute + ForwardTransparent -> PostRenderCompute  -> Present:
		ComputeStage::Record<RenderStage::postRenderCompute>(frameContext, frameContext.postRenderComputeCalls);
		std::array<VkSemaphoreSubmitInfo, 2> postRenderComputeWaits =
		{
			CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::postOutlineComputeToPostRenderCompute), VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_2_COPY_BIT),
			CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::forwardTransparentToPostRenderCompute), VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_2_COPY_BIT | VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT)
		};
		VkSemaphoreSubmitInfo postRenderComputeSignal = CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::postRenderComputeToPresent), VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_2_COPY_BIT);
		SubmitStage(frameContext, RenderStage::postRenderCompute, graphicsQueue, postRenderComputeWaits, std::span<const VkSemaphoreSubmitInfo>(&postRenderComputeSignal, 1));

		// Aquire + Gizmo + PostRenderCompute -> Present -> (optional)Release:
		PresentStage::Record(frameContext, hasSwapchainImage);
		std::array<VkSemaphoreSubmitInfo, 3> presentWaits =
		{
			CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::postRenderComputeToPresent), VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT),
			CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::gizmoToPresent), VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT),
			CreateSemaphoreSubmitInfo(GetDependencySemaphore(frameIndex, Dependency::acquireToPresent), VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT)
		};
		std::span<const VkSemaphoreSubmitInfo> waits(presentWaits.data(), hasSwapchainImage ? 3 : 2);	// no swapchain image = skip acquireToPresent semaphore
		if (hasSwapchainImage)
		{
			VkSemaphoreSubmitInfo presentSignal = CreateSemaphoreSubmitInfo(m_releaseSemaphores[frameContext.frameExecutionData.imageIndex], VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT);
			SubmitStage(frameContext, RenderStage::present, graphicsQueue, waits, std::span<const VkSemaphoreSubmitInfo>(&presentSignal, 1), m_frameSyncObjects[frameIndex].frameFence);
		}
		else
			SubmitStage(frameContext, RenderStage::present, graphicsQueue, waits, {}, m_frameSyncObjects[frameIndex].frameFence);
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
			case Dependency::acquireToPresent: return "AcquireToPresent";
			case Dependency::resourceUpdateToGizmo: return "ResourceUpdateToGizmo";
			case Dependency::resourceUpdateToPreRenderCompute: return "ResourceUpdateToPreRenderCompute";
			case Dependency::gizmoToPresent: return "GizmoToPresent";
			case Dependency::preRenderComputeToOutline: return "PreRenderComputeToOutline";
			case Dependency::preRenderComputeToShadow: return "PreRenderComputeToShadow";
			case Dependency::preRenderComputeToDeferredGeometry: return "PreRenderComputeToDeferredGeometry";
			case Dependency::outlineToPostOutlineCompute: return "OutlineToPostOutlineCompute";
			case Dependency::postOutlineComputeToPostRenderCompute: return "PostOutlineComputeToPostRenderCompute";
			case Dependency::shadowToDeferredLighting: return "ShadowToDeferredLighting";
			case Dependency::deferredGeometryToPostGeometryCompute: return "DeferredGeometryToPostGeometryCompute";
			case Dependency::postGeometryComputeToDeferredLighting: return "PostGeometryComputeToDeferredLighting";
			case Dependency::deferredLightingToPostLightingCompute: return "DeferredLightingToPostLightingCompute";
			case Dependency::postLightingComputeToForwardOpaque: return "PostLightingComputeToForwardOpaque";
			case Dependency::forwardOpaqueToForwardTransparent: return "ForwardOpaqueToForwardTransparent";
			case Dependency::forwardTransparentToPostRenderCompute: return "ForwardTransparentToPostRenderCompute";
			case Dependency::postRenderComputeToPresent: return "PostRenderComputeToPresent";
			default: throw std::out_of_range("RenderGraph::GetDependencyName(...) failed. dependency is out of range.");
		}
	}
}