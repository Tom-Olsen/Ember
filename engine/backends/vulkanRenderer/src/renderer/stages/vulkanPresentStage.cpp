#include "vulkanPresentStage.h"
#include "emberMath.h"
#include "iGui.h"
#include "logger.h"
#include "profiler.h"
#include "vulkanCommandPool.h"
#include "vulkanContext.h"
#include "vulkanDefaultGpuResources.h"
#include "vulkanDescriptorSetBinding.h"
#include "vulkanFrameContext.h"
#include "vulkanFrameResources.h"
#include "vulkanMacros.h"
#include "vulkanMaterial.h"
#include "vulkanPipeline.h"
#include "vulkanPresentRenderPass.h"
#include "vulkanRenderPassManager.h"
#include "vulkanRenderTargetResources.h"
#include "vulkanRenderTexture2d.h"
#include "vulkanSceneColorTexture2dPair.h"
#include "vulkanSwapchain.h"
#include <vulkan/vulkan.h>


namespace vulkanRendererBackend
{
	// Public methods:
	void PresentStage::Record(const FrameContext& frameContext)
	{
		if (!Context::DockSpaceEnabled())
			RecordFullScreenPresent(frameContext);
		else
			RecordImGuiPresent(frameContext);
	}



	// Private methods:
	void PresentStage::RecordFullScreenPresent(const FrameContext& frameContext)
	{
		PROFILE_FUNCTION();

		// Prepare command recording:
		CommandPool& commandPool = frameContext.resources.GetCommandPool(RenderStage::present);
		VkCommandBuffer& commandBuffer = commandPool.GetPrimaryVkCommandBuffer();
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		// Record present commands:
		VKA(vkBeginCommandBuffer(commandBuffer, &beginInfo));
		{
			// Publish the final scene color for editor sampling:
			SceneColorTexture2dPair& sceneColorTexturePair = frameContext.renderTargets.GetSceneColorTexturePair();
			sceneColorTexturePair.TransitionLayoutOfCurrenForSampling(commandBuffer, frameContext.frameExecutionData.frameIndex);

			DescriptorSetBinding* pPresentShaderDescriptorSetBinding = DefaultGpuResources::GetDefaultPresentMaterial()->GetDescriptorSetBinding();
			pPresentShaderDescriptorSetBinding->SetTexture("renderTexture", sceneColorTexturePair.GetCurrentTexture(frameContext.frameExecutionData.frameIndex));
			pPresentShaderDescriptorSetBinding->SetTexture("gizmoTexture", &frameContext.renderTargets.GetGizmoTexture(frameContext.frameExecutionData.frameIndex));
			pPresentShaderDescriptorSetBinding->UpdateShaderData(frameContext.frameExecutionData.frameIndex);

			// Viewport and scissor:
			Uint2 swapchainExtent = Context::GetSwapchain()->GetExtent();
			VkViewport viewport = {};
			viewport.width = (float)swapchainExtent.x;
			viewport.height = (float)swapchainExtent.y;
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
			VkRect2D scissor = {};
			scissor.extent.width = viewport.width;
			scissor.extent.height = viewport.height;
			vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
			vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

			// Render pass info:
			PresentRenderPass* presentRenderPass = RenderPassManager::GetPresentRenderPass();
			VkRenderPassBeginInfo renderPassBeginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
			renderPassBeginInfo.renderPass = presentRenderPass->GetVkRenderPass();
			renderPassBeginInfo.framebuffer = presentRenderPass->GetFramebuffer(frameContext.frameExecutionData.imageIndex);
			renderPassBeginInfo.renderArea.offset = { 0, 0 };
			renderPassBeginInfo.renderArea.extent = VkExtent2D{swapchainExtent.x, swapchainExtent.y};

			// Begin render pass:
			Material* pMaterial = DefaultGpuResources::GetDefaultPresentMaterial();
			vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
			{
				// Bind Pipeline:
				vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pMaterial->GetFullscreenPipeline<RenderStage::present>()->GetVkPipeline());

				// Bind descriptorSets:
				VkDescriptorSet descriptorSets[4] =
				{
					frameContext.resources.staticDescriptorSets[0],
					frameContext.resources.staticDescriptorSets[1],
					frameContext.resources.staticDescriptorSets[2],
					pMaterial->GetDescriptorSetBinding()->GetVkDescriptorSet(frameContext.frameExecutionData.frameIndex)
				};
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pMaterial->GetVkPipelineLayout(), 0, 4, descriptorSets, 0, nullptr);

				// Dispatch:
				vkCmdDraw(commandBuffer, 3, 1, 0, 0);
				if (frameContext.pGui)
					frameContext.pGui->Render(commandBuffer);
				DEBUG_LOG_INFO("Render renderTexture with fullscreen triangle, material = {}", pMaterial->GetDebugName());
			}
			vkCmdEndRenderPass(commandBuffer);

		}
		VKA(vkEndCommandBuffer(commandBuffer));
	}
	void PresentStage::RecordImGuiPresent(const FrameContext& frameContext)
	{
		PROFILE_FUNCTION();

		// Prepare command recording:
		CommandPool& commandPool = frameContext.resources.GetCommandPool(RenderStage::present);
		VkCommandBuffer& commandBuffer = commandPool.GetPrimaryVkCommandBuffer();
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		// Record present commands:
		VKA(vkBeginCommandBuffer(commandBuffer, &beginInfo));
		{
			// Publish the final scene color for editor sampling:
			frameContext.renderTargets.GetSceneColorTexturePair().TransitionLayoutOfCurrenForSampling(commandBuffer, frameContext.frameExecutionData.frameIndex);

			// Render pass info:
			Uint2 swapchainExtent = Context::GetSwapchain()->GetExtent();
			PresentRenderPass* presentRenderPass = RenderPassManager::GetPresentRenderPass();
			VkRenderPassBeginInfo renderPassBeginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
			renderPassBeginInfo.renderPass = presentRenderPass->GetVkRenderPass();
			renderPassBeginInfo.framebuffer = presentRenderPass->GetFramebuffer(frameContext.frameExecutionData.imageIndex);
			renderPassBeginInfo.renderArea.offset = { 0, 0 };
			renderPassBeginInfo.renderArea.extent = VkExtent2D{swapchainExtent.x, swapchainExtent.y};

			// Begin render pass:
			vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
			{
				if (frameContext.pGui)
					frameContext.pGui->Render(commandBuffer);
			}
			vkCmdEndRenderPass(commandBuffer);

		}
		VKA(vkEndCommandBuffer(commandBuffer));
	}
}