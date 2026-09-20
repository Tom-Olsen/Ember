#include "vulkanForwardStage.h"
#include "descriptorSetMacros.h"
#include "logger.h"
#include "profiler.h"
#include "vmaBuffer.h"
#include "vmaImage.h"
#include "vulkanAccessMask.h"
#include "vulkanCommandPool.h"
#include "vulkanConvertMaterialRenderState.h"
#include "vulkanDefaultPushConstant.h"
#include "vulkanDescriptorSetBinding.h"
#include "vulkanForwardDrawCall.h"
#include "vulkanForwardOpaqueRenderPass.h"
#include "vulkanForwardTransparentRenderPass.h"
#include "vulkanFrameContext.h"
#include "vulkanFrameRenderData.h"
#include "vulkanFrameResources.h"
#include "vulkanIndexBuffer.h"
#include "vulkanMacros.h"
#include "vulkanMaterial.h"
#include "vulkanMesh.h"
#include "vulkanPipeline.h"
#include "vulkanRenderPass.h"
#include "vulkanRenderPassManager.h"
#include "vulkanRenderTargetResources.h"
#include "vulkanRenderTexture2d.h"
#include "vulkanSceneColorTexture2dPair.h"
#include <algorithm>
#include <array>
#include <cstdint>
#include <vector>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Public methods:
	template<RenderStage stage>
	void ForwardStage<stage>::Record(const FrameContext& frameContext)
	{
		static_assert(stage == RenderStage::forwardOpaque || stage == RenderStage::forwardTransparent);
		PROFILE_FUNCTION();

		RenderPass* pRenderPass;
		RenderTexture2d* pRenderTexture;
		VkFramebuffer framebuffer;
		const std::vector<ForwardDrawCall*>* pDrawCallPointers;
		if constexpr (stage == RenderStage::forwardOpaque)
		{
			ForwardOpaqueRenderPass* pForwardOpaqueRenderPass = RenderPassManager::GetForwardOpaqueRenderPass();
			pRenderPass = pForwardOpaqueRenderPass;
			// Forward opaque rendering always renders to 0th sceneColor texture:
			pRenderTexture = &frameContext.renderTargets.GetSceneColorTexturePair().GetRenderTargetTexture(frameContext.frameExecutionData.frameIndex, 0);
			framebuffer = pForwardOpaqueRenderPass->GetFramebuffer(frameContext.frameExecutionData.frameIndex);
			pDrawCallPointers = &frameContext.frameRenderData.sortedForwardOpaqueDrawCallPointers;
		}
		else
		{
			ForwardTransparentRenderPass* pForwardTransparentRenderPass = RenderPassManager::GetForwardTransparentRenderPass();
			pRenderPass = pForwardTransparentRenderPass;
			// Forward transparent rendering renders to renderTexture 0 or 1, depending on sceneColor swaps in screen-space compute stage:
			pRenderTexture = &frameContext.renderTargets.GetSceneColorTexturePair().GetRenderTargetTexture(frameContext.frameExecutionData.frameIndex, frameContext.frameExecutionData.transparentSceneColorIndex);
			framebuffer = pForwardTransparentRenderPass->GetFramebuffer(frameContext.frameExecutionData.frameIndex, frameContext.frameExecutionData.transparentSceneColorIndex);
			pDrawCallPointers = &frameContext.frameRenderData.sortedForwardTransparentDrawCallPointers;
		}

		// Prepare command recording:
		CommandPool& commandPool = frameContext.resources.GetCommandPool(stage);
		VkCommandBuffer& commandBuffer = commandPool.GetPrimaryVkCommandBuffer();
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		// Record forward commands:
		VKA(vkBeginCommandBuffer(commandBuffer, &beginInfo));
		{
			// Viewport and scissor:
			VkViewport viewport = {};
			viewport.width = pRenderTexture->GetWidth();
			viewport.height = pRenderTexture->GetHeight();
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
			VkRect2D scissor = {};
			scissor.extent.width = viewport.width;
			scissor.extent.height = viewport.height;
			vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
			vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

			// Render pass info:
			std::array<VkClearValue, 2> clearValues;
			clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
			clearValues[1].depthStencil = { 1.0f, 0 };
			VkRenderPassBeginInfo renderPassBeginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
			renderPassBeginInfo.renderPass = pRenderPass->GetVkRenderPass();
			renderPassBeginInfo.framebuffer = framebuffer;
			renderPassBeginInfo.renderArea.offset = { 0, 0 };
			renderPassBeginInfo.renderArea.extent.width = viewport.width;
			renderPassBeginInfo.renderArea.extent.height = viewport.height;
			renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassBeginInfo.pClearValues = clearValues.data();

			// Begin render pass:
			vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
			if (!pDrawCallPointers->empty())
			{
				// Pipeline:
				VkPipeline pipeline = VK_NULL_HANDLE;
				VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
				VkDescriptorSet shaderDescriptorSet = VK_NULL_HANDLE;
				bool staticDescriptorSetsBound = false;

				// Draw calls:
				for (ForwardDrawCall* drawCall : *pDrawCallPointers)
				{
					// Pipeline swap:
					Material* pForwardMaterial = drawCall->pMaterial;
					VkPipeline newPipeline = pForwardMaterial->GetPipeline<stage>(drawCall->pMesh)->GetVkPipeline();
					bool pipelineLayoutChanged = false;
					if (pipeline != newPipeline)
					{
						// Bind Pipeline:
						pipeline = newPipeline;
						vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

						// Pipeline layout swap:
						VkPipelineLayout newPipelineLayout = pForwardMaterial->GetVkPipelineLayout();
						pipelineLayoutChanged = pipelineLayout != newPipelineLayout;
						if (pipelineLayoutChanged)
						{
							pipelineLayout = newPipelineLayout;

							// Bind static descriptor sets:
							if (!staticDescriptorSetsBound)
							{
								vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 3, frameContext.resources.staticDescriptorSets.data(), 0, nullptr);
								staticDescriptorSetsBound = true;
							}
						}
					}

					// Bind per shader descriptor set:
					VkDescriptorSet newShaderDescriptorSet = pForwardMaterial->GetDescriptorSetBinding()->GetVkDescriptorSet(frameContext.frameExecutionData.frameIndex);
					if (newShaderDescriptorSet != VK_NULL_HANDLE && (pipelineLayoutChanged || shaderDescriptorSet != newShaderDescriptorSet))
					{
						shaderDescriptorSet = newShaderDescriptorSet;
						vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, SHADER_SET_INDEX, 1, &shaderDescriptorSet, 0, nullptr);
					}

					// Push constant:
					DefaultPushConstant pushConstant(0, drawCall->instanceCount, drawCall->receiveShadows, frameContext.frameExecutionData.time, frameContext.frameExecutionData.deltaTime);
					vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(DefaultPushConstant), &pushConstant);

					// Cull mode:
					vkCmdSetCullMode(commandBuffer, CullModeCommonToVulkan(drawCall->cullMode));

					// Bind per draw call descriptor set:
					if (VkDescriptorSet vkDescriptorSet = drawCall->descriptorSetBindingHandle.Get()->GetVkDescriptorSet(frameContext.frameExecutionData.frameIndex); vkDescriptorSet != VK_NULL_HANDLE)
						vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, CALL_SET_INDEX, 1, &vkDescriptorSet, 0, nullptr);

					// Bind mesh data:
					vkCmdBindVertexBuffers(commandBuffer, 0, drawCall->pMesh->GetVertexBindingCount(), drawCall->pMesh->GetVkBuffers(), drawCall->pMesh->GetOffsets());
					vkCmdBindIndexBuffer(commandBuffer, drawCall->pMesh->GetIndexBuffer()->GetVmaBuffer()->GetVkBuffer(), 0, drawCall->pMesh->GetVkIndexType());

					// Dispatch:
					vkCmdDrawIndexed(commandBuffer, drawCall->pMesh->GetIndexCount(), std::max(drawCall->instanceCount, (uint32_t)1), 0, 0, 0);
					DEBUG_LOG_TRACE("Forward draw call, mesh = {}, material = {}", drawCall->pMesh->GetName(), pForwardMaterial->GetDebugName());
				}
			}
			vkCmdEndRenderPass(commandBuffer);

			// Make the completed transparent color attachment available to post render compute shaders:
			if constexpr (stage == RenderStage::forwardTransparent)
			{
				VkMemoryBarrier2 memoryBarrier = { VK_STRUCTURE_TYPE_MEMORY_BARRIER_2 };
				memoryBarrier.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
				memoryBarrier.dstStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
				memoryBarrier.srcAccessMask = AccessMasks::ColorAttachmentOutput::colorAttachmentWrite;
				memoryBarrier.dstAccessMask = AccessMasks::ComputeShader::shaderRead | AccessMasks::ComputeShader::shaderWrite;

				VkDependencyInfo dependencyInfo = { VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
				dependencyInfo.memoryBarrierCount = 1;
				dependencyInfo.pMemoryBarriers = &memoryBarrier;

				vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);
				DEBUG_LOG_TRACE("Memory Barrier: forward transparent color attachment to post compute");
			}
		}
		VKA(vkEndCommandBuffer(commandBuffer));

		// Align renderTexture layout with final renderPass layout:
		pRenderTexture->GetVmaImage()->SetLayout(VK_IMAGE_LAYOUT_GENERAL);
	}



    // Explicit template instantiation:
	template class ForwardStage<RenderStage::forwardOpaque>;
	template class ForwardStage<RenderStage::forwardTransparent>;
}