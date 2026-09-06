#include "vulkanGizmoStage.h"
#include "descriptorSetMacros.h"
#include "logger.h"
#include "profiler.h"
#include "vmaBuffer.h"
#include "vmaImage.h"
#include "vulkanCommandPool.h"
#include "vulkanConvertMaterialRenderState.h"
#include "vulkanDefaultPushConstant.h"
#include "vulkanDescriptorSetBinding.h"
#include "vulkanFrameContext.h"
#include "vulkanFrameRenderData.h"
#include "vulkanFrameResources.h"
#include "vulkanGizmoDrawCall.h"
#include "vulkanGizmoRenderPass.h"
#include "vulkanIndexBuffer.h"
#include "vulkanMacros.h"
#include "vulkanMaterial.h"
#include "vulkanMesh.h"
#include "vulkanPipeline.h"
#include "vulkanRenderPassManager.h"
#include "vulkanRenderTexture2d.h"
#include <algorithm>
#include <array>
#include <cstdint>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Public methods:
	void GizmoStage::Record(const FrameContext& frameContext)
	{
		PROFILE_FUNCTION();

		// Prepare command recording:
		CommandPool& commandPool = frameContext.resources.GetCommandPool(RenderStage::gizmo);
		VkCommandBuffer& commandBuffer = commandPool.GetPrimaryVkCommandBuffer();
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		GizmoRenderPass* pGizmoRenderPass = RenderPassManager::GetGizmoRenderPass();

		// Record gizmo commands:
		VKA(vkBeginCommandBuffer(commandBuffer, &beginInfo));
		{
			// Viewport and scissor:
			VkViewport viewport = {};
			viewport.width = pGizmoRenderPass->GetRenderTexture(frameContext.frameIndex)->GetWidth();
			viewport.height = pGizmoRenderPass->GetRenderTexture(frameContext.frameIndex)->GetHeight();
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
			VkRect2D scissor = {};
			scissor.extent.width = viewport.width;
			scissor.extent.height = viewport.height;
			vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
			vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

			// Render pass info:
			std::array<VkClearValue, 2> clearValues;
			clearValues[0].color = { 0.0f, 0.0f, 0.0f, 0.0f };
			clearValues[1].depthStencil = { 1.0f, 0 };
			VkRenderPassBeginInfo renderPassBeginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
			renderPassBeginInfo.renderPass = pGizmoRenderPass->GetVkRenderPass();
			renderPassBeginInfo.framebuffer = pGizmoRenderPass->GetFramebuffer(frameContext.frameIndex);
			renderPassBeginInfo.renderArea.offset = { 0, 0 };
			renderPassBeginInfo.renderArea.extent.width = viewport.width;
			renderPassBeginInfo.renderArea.extent.height = viewport.height;
			renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassBeginInfo.pClearValues = clearValues.data();

			// Begin render pass:
			vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
			if (!frameContext.frameRenderData.sortedGizmoDrawCallPointers.empty())
			{
				// Pipeline:
				VkPipeline pipeline = VK_NULL_HANDLE;
				VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
				VkDescriptorSet shaderDescriptorSet = VK_NULL_HANDLE;
				bool staticDescriptorSetsBound = false;

				// Draw calls:
				for (GizmoDrawCall* drawCall : frameContext.frameRenderData.sortedGizmoDrawCallPointers)
				{
					// Pipeline swap:
					Material* pGizmoMaterial = drawCall->pMaterial;
					VkPipeline newPipeline = pGizmoMaterial->GetPipeline<RenderStage::gizmo>(drawCall->pMesh)->GetVkPipeline();
					bool pipelineLayoutChanged = false;
					if (pipeline != newPipeline)
					{
						// Bind Pipeline:
						pipeline = newPipeline;
						vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

						// Pipeline layout swap:
						VkPipelineLayout newPipelineLayout = pGizmoMaterial->GetVkPipelineLayout();
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
					VkDescriptorSet newShaderDescriptorSet = pGizmoMaterial->GetDescriptorSetBinding()->GetVkDescriptorSet(frameContext.frameIndex);
					if (newShaderDescriptorSet != VK_NULL_HANDLE && (pipelineLayoutChanged || shaderDescriptorSet != newShaderDescriptorSet))
					{
						shaderDescriptorSet = newShaderDescriptorSet;
						vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, SHADER_SET_INDEX, 1, &shaderDescriptorSet, 0, nullptr);
					}

					// Push constant:
					DefaultPushConstant pushConstant(0, drawCall->instanceCount, false, frameContext.time, frameContext.deltaTime);
					vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(DefaultPushConstant), &pushConstant);

					// Cull mode:
					vkCmdSetCullMode(commandBuffer, CullModeCommonToVulkan(drawCall->cullMode));

					// Bind per draw call descriptor set:
					if (VkDescriptorSet vkDescriptorSet = drawCall->descriptorSetBindingHandle.Get()->GetVkDescriptorSet(frameContext.frameIndex); vkDescriptorSet != VK_NULL_HANDLE)
						vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, CALL_SET_INDEX, 1, &vkDescriptorSet, 0, nullptr);

					// Bind mesh data:
					vkCmdBindVertexBuffers(commandBuffer, 0, drawCall->pMesh->GetVertexBindingCount(), drawCall->pMesh->GetVkBuffers(), drawCall->pMesh->GetOffsets());
					vkCmdBindIndexBuffer(commandBuffer, drawCall->pMesh->GetIndexBuffer()->GetVmaBuffer()->GetVkBuffer(), 0, drawCall->pMesh->GetVkIndexType());

					// Dispatch:
					vkCmdDrawIndexed(commandBuffer, drawCall->pMesh->GetIndexCount(), std::max(drawCall->instanceCount, (uint32_t)1), 0, 0, 0);
					DEBUG_LOG_TRACE("Gizmo draw call, mesh = {}, material = {}", drawCall->pMesh->GetName(), pGizmoMaterial->GetDebugName());
				}
			}
			vkCmdEndRenderPass(commandBuffer);
		}
		VKA(vkEndCommandBuffer(commandBuffer));

		// Gizmo render pass's color resolve finalLayout is VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL. Reflect this in the image layout:
		pGizmoRenderPass->GetRenderTexture(frameContext.frameIndex)->GetVmaImage()->SetLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}
}