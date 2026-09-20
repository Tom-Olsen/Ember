#include "vulkanShadowStage.h"
#include "descriptorSetMacros.h"
#include "logger.h"
#include "profiler.h"
#include "vmaBuffer.h"
#include "vmaImage.h"
#include "vulkanCommandPool.h"
#include "vulkanDefaultPushConstant.h"
#include "vulkanDepthTexture2dArray.h"
#include "vulkanDescriptorSetBinding.h"
#include "vulkanFrameContext.h"
#include "vulkanFrameRenderData.h"
#include "vulkanFrameResources.h"
#include "vulkanIndexBuffer.h"
#include "vulkanMacros.h"
#include "vulkanMaterial.h"
#include "vulkanMesh.h"
#include "vulkanPipeline.h"
#include "vulkanRenderPassManager.h"
#include "vulkanRenderTargetResources.h"
#include "vulkanShadowDrawCall.h"
#include "vulkanShadowRenderPass.h"
#include <algorithm>
#include <cstdint>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Public methods:
	void ShadowStage::Record(const FrameContext& frameContext)
	{
		PROFILE_FUNCTION();

		// Prepare command recording:
		CommandPool& commandPool = frameContext.resources.GetCommandPool(RenderStage::shadow);
		VkCommandBuffer& commandBuffer = commandPool.GetPrimaryVkCommandBuffer();
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		ShadowRenderPass* pShadowRenderPass = RenderPassManager::GetShadowRenderPass();

		// Record shadow commands:
		VKA(vkBeginCommandBuffer(commandBuffer, &beginInfo));
		{
			// Render pass info:
			VkClearValue clearValues = {};
			clearValues.depthStencil = { 1.0f, 0 };
			VkRenderPassBeginInfo renderPassBeginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
			renderPassBeginInfo.renderPass = pShadowRenderPass->GetVkRenderPass();
			renderPassBeginInfo.framebuffer = pShadowRenderPass->GetFramebuffer(0);
			renderPassBeginInfo.renderArea.offset = { 0, 0 };
			renderPassBeginInfo.renderArea.extent = VkExtent2D{ frameContext.shadowMapResolution, frameContext.shadowMapResolution };
			renderPassBeginInfo.clearValueCount = 1;
			renderPassBeginInfo.pClearValues = &clearValues;

			// Begin render pass:
			vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
			if (!frameContext.frameRenderData.shadowDrawCalls.empty())
			{
				// Pipeline:
				VkPipeline pipeline = VK_NULL_HANDLE;
				VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
				VkDescriptorSet shaderDescriptorSet = VK_NULL_HANDLE;
				bool staticDescriptorSetsBound = false;

				// Lights:
				const uint32_t shadowLightCount = frameContext.shadowMapCount;
				if (shadowLightCount > 0)
				{
					// Depth bias:
					vkCmdSetDepthBias(commandBuffer, frameContext.depthBiasConstantFactor, frameContext.depthBiasClamp, frameContext.depthBiasSlopeFactor);

					// Draw calls:
					for (ShadowDrawCall& drawCall : frameContext.frameRenderData.shadowDrawCalls)
					{
						// Pipeline swap:
						const Material* pShadowMaterial = drawCall.pMaterial;
						VkPipeline newPipeline = pShadowMaterial->GetPipeline<RenderStage::shadow>(drawCall.pMesh)->GetVkPipeline();
						bool pipelineLayoutChanged = false;
						if (pipeline != newPipeline)
						{
							// Bind Pipeline:
							pipeline = newPipeline;
							vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

							// Pipeline layout swap:
							VkPipelineLayout newPipelineLayout = pShadowMaterial->GetVkPipelineLayout();
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
						VkDescriptorSet newShaderDescriptorSet = pShadowMaterial->GetDescriptorSetBinding()->GetVkDescriptorSet(frameContext.frameExecutionData.frameIndex);
						if (newShaderDescriptorSet != VK_NULL_HANDLE && (pipelineLayoutChanged || shaderDescriptorSet != newShaderDescriptorSet))
						{
							shaderDescriptorSet = newShaderDescriptorSet;
							vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, SHADER_SET_INDEX, 1, &shaderDescriptorSet, 0, nullptr);
						}

						// Bind per draw call descriptor set:
						if (VkDescriptorSet vkDescriptorSet = drawCall.descriptorSetBindingHandle.Get()->GetVkDescriptorSet(frameContext.frameExecutionData.frameIndex); vkDescriptorSet != VK_NULL_HANDLE)
							vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, CALL_SET_INDEX, 1, &vkDescriptorSet, 0, nullptr);

						// Bind mesh data:
						vkCmdBindVertexBuffers(commandBuffer, 0, drawCall.pMesh->GetVertexBindingCount(), drawCall.pMesh->GetVkBuffers(), drawCall.pMesh->GetOffsets());
						vkCmdBindIndexBuffer(commandBuffer, drawCall.pMesh->GetIndexBuffer()->GetVmaBuffer()->GetVkBuffer(), 0, drawCall.pMesh->GetVkIndexType());

						for (uint32_t shadowMapIndex = 0; shadowMapIndex < shadowLightCount; shadowMapIndex++)
						{
							// Push constant:
							DefaultPushConstant pushConstant(shadowMapIndex, drawCall.instanceCount, false, frameContext.frameExecutionData.time, frameContext.frameExecutionData.deltaTime);
							vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(DefaultPushConstant), &pushConstant);

							// Dispatch:
							vkCmdDrawIndexed(commandBuffer, drawCall.pMesh->GetIndexCount(), std::max(drawCall.instanceCount, (uint32_t)1), 0, 0, 0);
							DEBUG_LOG_INFO("Light {}, mesh = {}", shadowMapIndex, drawCall.pMesh->GetName());
						}
					}
				}
			}
			vkCmdEndRenderPass(commandBuffer);
			// Align shadowMaps layout with final renderPass layout: 
			frameContext.renderTargets.GetShadowMaps().GetVmaImage()->SetLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		}
		VKA(vkEndCommandBuffer(commandBuffer));
	}
}