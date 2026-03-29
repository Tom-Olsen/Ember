#include "vulkanComputeSession.h"
#include "emberMath.h"
#include "logger.h"
#include "vulkanAccessMask.h"
#include "vulkanComputePushConstant.h"
#include "vulkanComputeShader.h"
#include "vulkanContext.h"
#include "vulkanDescriptorSetBinding.h"
#include "vulkanMacros.h"
#include "vulkanLogicalDevice.h"
#include "vulkanPipeline.h"
#include "vulkanPipelineStage.h"
#include "vulkanRenderer.h"
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Public methods:
	void ComputeSession::RecordComputeCall(const ComputeCall& computeCall)
	{
		m_computeCalls.push_back(computeCall);
	}
	void ComputeSession::Dispatch(VkCommandBuffer commandBuffer, VkFence fence, float time, float deltaTime)
	{
		// Record command buffer:
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		VKA(vkBeginCommandBuffer(commandBuffer, &beginInfo));
		{
			ComputeShader* pComputeShader = nullptr;
			VkPipeline pipeline = VK_NULL_HANDLE;
			VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
			ComputePushConstant pushConstant(Uint3::one, time, deltaTime);

			for (ComputeCall& computeCall : m_computeCalls)
			{
				// Compute call is a barrier:
				if (computeCall.pComputeShader == nullptr)
				{
					VkMemoryBarrier2 memoryBarrier = { VK_STRUCTURE_TYPE_MEMORY_BARRIER_2 };
					memoryBarrier.srcStageMask = PipelineStages::computeShader;
					memoryBarrier.dstStageMask = PipelineStages::computeShader;
					memoryBarrier.srcAccessMask = computeCall.srcAccessMask;
					memoryBarrier.dstAccessMask = computeCall.dstAccessMask;

					VkDependencyInfo dependencyInfo = { VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
					dependencyInfo.memoryBarrierCount = 1;
					dependencyInfo.pMemoryBarriers = &memoryBarrier;

					vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);
				}
				// Compute call is a dispatch:
				else
				{
					// Update shader specific data:
					computeCall.pDescriptorSetBinding->UpdateShaderData(0);

					// Change pipeline if compute shader has changed:
					pComputeShader = computeCall.pComputeShader;
					if (pComputeShader != computeCall.pComputeShader)
					{
						pComputeShader = computeCall.pComputeShader;
						pipeline = pComputeShader->GetPipeline()->GetVkPipeline();
						pipelineLayout = pComputeShader->GetVkPipelineLayout();
						vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
					}

					// Push constant:
					pushConstant.threadCount = computeCall.threadCount;
					vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(ComputePushConstant), &pushConstant);

			        // Compute group count:
					Uint3 blockSize = pComputeShader->GetBlockSize();
					uint32_t groupCountX = (computeCall.threadCount.x + blockSize.x - 1) / blockSize.x;
					uint32_t groupCountY = (computeCall.threadCount.y + blockSize.y - 1) / blockSize.y;
					uint32_t groupCountZ = (computeCall.threadCount.z + blockSize.z - 1) / blockSize.z;

                    // Dispatch compute shader:
                    VkDescriptorSet globalSet = Context::GetRenderer()->GetStaticDescriptorSets(0)[GLOBAL_SET_INDEX];
			        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, GLOBAL_SET_INDEX, 1, &globalSet, 0, nullptr);
			        if (VkDescriptorSet vkDescriptorSet = pComputeShader->GetDescriptorSetBinding()->GetVkDescriptorSet(0); vkDescriptorSet != VK_NULL_HANDLE)
			        	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, SHADER_SET_INDEX, 1, &vkDescriptorSet, 0, nullptr);
			        vkCmdDispatch(commandBuffer, groupCountX, groupCountY, groupCountZ);
				}
			}
		}
		VKA(vkEndCommandBuffer(commandBuffer));

		// Submit command buffer:
		VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
		submitInfo.waitSemaphoreCount = 0;
		submitInfo.pWaitSemaphores = VK_NULL_HANDLE;
		submitInfo.pWaitDstStageMask = nullptr;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;
		submitInfo.signalSemaphoreCount = 0;
		submitInfo.pSignalSemaphores = nullptr;
		VKA(vkResetFences(Context::GetVkDevice(), 1, &fence));
		VKA(vkQueueSubmit(Context::GetLogicalDevice()->GetComputeQueue().queue, 1, &submitInfo, fence));
	}
	void ComputeSession::ResetComputeCalls()
	{
		m_computeCalls.clear();
	}
	std::vector<ComputeCall>& ComputeSession::GetComputeCalls()
	{
		return m_computeCalls;
	}
}