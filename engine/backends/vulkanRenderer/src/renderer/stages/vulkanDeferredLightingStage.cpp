#include "vulkanDeferredLightingStage.h"
#include "profiler.h"
#include "vmaImage.h"
#include "vulkanCommandPool.h"
#include "vulkanDefaultGpuResources.h"
#include "vulkanDeferredLightingRenderPass.h"
#include "vulkanDescriptorSetBinding.h"
#include "vulkanFrameContext.h"
#include "vulkanFrameResources.h"
#include "vulkanMacros.h"
#include "vulkanMaterial.h"
#include "vulkanPipeline.h"
#include "vulkanRenderPassManager.h"
#include "vulkanRenderTexture2d.h"
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Public methods:
	void DeferredLightingStage::Record(const FrameContext& frameContext)
	{
		PROFILE_FUNCTION();

		// Prepare command recording:
		CommandPool& commandPool = frameContext.resources.GetCommandPool(RenderStage::deferredLighting);
		VkCommandBuffer& commandBuffer = commandPool.GetPrimaryVkCommandBuffer();
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		DeferredLightingRenderPass* pDeferredLightingRenderPass = RenderPassManager::GetDeferredLightingRenderPass();

		// Record deferred lighting commands:
		VKA(vkBeginCommandBuffer(commandBuffer, &beginInfo));
		{
			// Viewport and scissor:
			VkViewport viewport = {};
			viewport.width = pDeferredLightingRenderPass->GetSceneColorTexture(frameContext.frameIndex)->GetWidth();
			viewport.height = pDeferredLightingRenderPass->GetSceneColorTexture(frameContext.frameIndex)->GetHeight();
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
			VkRect2D scissor = {};
			scissor.extent.width = viewport.width;
			scissor.extent.height = viewport.height;
			vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
			vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

			// Render pass info:
			VkClearValue clearValue = {};
			clearValue.color = { 0.0f, 0.0f, 0.0f, 1.0f };
			VkRenderPassBeginInfo renderPassBeginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
			renderPassBeginInfo.renderPass = pDeferredLightingRenderPass->GetVkRenderPass();
			renderPassBeginInfo.framebuffer = pDeferredLightingRenderPass->GetFramebuffer(frameContext.frameIndex);
			renderPassBeginInfo.renderArea.offset = { 0, 0 };
			renderPassBeginInfo.renderArea.extent.width = viewport.width;
			renderPassBeginInfo.renderArea.extent.height = viewport.height;
			renderPassBeginInfo.clearValueCount = 1;
			renderPassBeginInfo.pClearValues = &clearValue;

			// Begin render pass:
			Material* pMaterial = DefaultGpuResources::GetDefaultDeferredLightingMaterial();
			vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
			{
				// Bind Pipeline:
				vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pMaterial->GetFullscreenPipeline<RenderStage::deferredLighting>()->GetVkPipeline());

				// Bind descriptorSets:
				VkDescriptorSet descriptorSets[4] =
				{
					frameContext.resources.staticDescriptorSets[0],
					frameContext.resources.staticDescriptorSets[1],
					frameContext.resources.staticDescriptorSets[2],
					pMaterial->GetDescriptorSetBinding()->GetVkDescriptorSet(frameContext.frameIndex)
				};
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pMaterial->GetVkPipelineLayout(), 0, 4, descriptorSets, 0, nullptr);

				// Dispatch:
				vkCmdDraw(commandBuffer, 3, 1, 0, 0);
			}
			vkCmdEndRenderPass(commandBuffer);

			pDeferredLightingRenderPass->GetSceneColorTexture(frameContext.frameIndex)->GetVmaImage()->SetLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
		}
		VKA(vkEndCommandBuffer(commandBuffer));
	}
}