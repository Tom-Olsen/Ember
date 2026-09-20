#include "vulkanComputeStage.h"
#include "descriptorSetMacros.h"
#include "logger.h"
#include "profiler.h"
#include "vulkanAccessMask.h"
#include "vulkanCommandPool.h"
#include "vulkanComputePushConstant.h"
#include "vulkanComputeShader.h"
#include "vulkanDescriptorSetBinding.h"
#include "vulkanFrameContext.h"
#include "vulkanFrameResources.h"
#include "vulkanMacros.h"
#include "vulkanPipeline.h"
#include "vulkanRenderTargetResources.h"
#include "vulkanSceneColorTexture2dPair.h"
#include <stdexcept>
#include <string>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Public methods:
	template<RenderStage stage>
	void ComputeStage<stage>::Record(const FrameContext& frameContext, std::span<const ComputeCall> computeCalls)
	{
		static_assert(stage == RenderStage::preRenderCompute || stage == RenderStage::midRenderCompute || stage == RenderStage::screenSpaceCompute || stage == RenderStage::postRenderCompute);
		PROFILE_FUNCTION();

		// Prepare command recording:
		CommandPool& commandPool = frameContext.resources.GetCommandPool(stage);
		VkCommandBuffer& commandBuffer = commandPool.GetPrimaryVkCommandBuffer();
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		// Record compute commands:
		VKA(vkBeginCommandBuffer(commandBuffer, &beginInfo));
		{
			// These stages may read or write sceneColor textures and must transition them to general layout first:
			if constexpr(stage == RenderStage::screenSpaceCompute || stage == RenderStage::postRenderCompute)
				frameContext.renderTargets.GetSceneColorTexturePair().TransitionLayoutForCompute(commandBuffer, frameContext.frameExecutionData.frameIndex);

			// Pipeline:
			VkPipeline pipeline = VK_NULL_HANDLE;
			VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
			bool staticDescriptorSetsBound = false;

			// Compute calls:
			for (size_t computeCallIndex = 0; computeCallIndex < computeCalls.size(); computeCallIndex++)
			{
				const ComputeCall& computeCall = computeCalls[computeCallIndex];
				if (computeCall.IsBarrier())
				{
					RecordBarrier(commandBuffer, computeCall, computeCallIndex);
					continue;
				}

				// Pipeline change:
				ComputeShader* pComputeShader = computeCall.GetComputeShader();
				VkPipeline newPipeline = pComputeShader->GetPipeline()->GetVkPipeline();
				if (pipeline != newPipeline)
				{
					// Bind Pipeline:
					pipeline = newPipeline;
					pipelineLayout = pComputeShader->GetVkPipelineLayout();
					vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);

					// Bind static descriptor sets:
					if (!staticDescriptorSetsBound)
					{
						vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 3, frameContext.resources.staticDescriptorSets.data(), 0, nullptr);
						staticDescriptorSetsBound = true;
					}

					// Bind per shader descriptor set:
					if (VkDescriptorSet descriptorSet = pComputeShader->GetDescriptorSetBinding()->GetVkDescriptorSet(frameContext.frameExecutionData.frameIndex); descriptorSet != VK_NULL_HANDLE)
						vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, SHADER_SET_INDEX, 1, &descriptorSet, 0, nullptr);
				}

				// Bind per compute call descriptor set:
				if (VkDescriptorSet descriptorSet = computeCall.callDescriptorSetBindingHandle.Get()->GetVkDescriptorSet(frameContext.frameExecutionData.frameIndex); descriptorSet != VK_NULL_HANDLE)
					vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, CALL_SET_INDEX, 1, &descriptorSet, 0, nullptr);

				// Push constant:
				ComputePushConstant pushConstant(computeCall.threadCount, frameContext.frameExecutionData.time, frameContext.frameExecutionData.deltaTime, computeCall.sceneColorIndex);
				vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(ComputePushConstant), &pushConstant);

				// Group counts:
				Uint3 blockSize = pComputeShader->GetBlockSize();
				uint32_t groupCountX = (computeCall.threadCount.x + blockSize.x - 1) / blockSize.x;
				uint32_t groupCountY = (computeCall.threadCount.y + blockSize.y - 1) / blockSize.y;
				uint32_t groupCountZ = (computeCall.threadCount.z + blockSize.z - 1) / blockSize.z;
				vkCmdDispatch(commandBuffer, groupCountX, groupCountY, groupCountZ);
				DEBUG_LOG_TRACE("{} shader {}, call = {}", renderStageNames[static_cast<size_t>(stage)], pComputeShader->GetDebugName(), computeCallIndex);

				// Automatically synchronize scene-color writes before subsequent compute calls:
				if (pComputeShader->GetFeatures().WritesSceneColor())
					RecordComputeToComputeShaderBarrier(commandBuffer, computeCallIndex);
			}

			// Dispatch:
			if constexpr(stage == RenderStage::preRenderCompute)
				if (!computeCalls.empty())
					RecordFinalPreRenderComputeBarrier(commandBuffer);
		}
		VKA(vkEndCommandBuffer(commandBuffer));
	}



	// Private methods:
	template<RenderStage stage>
	void ComputeStage<stage>::RecordBarrier(VkCommandBuffer commandBuffer, const ComputeCall& computeCall, size_t computeCallIndex) const
	{
		VkMemoryBarrier2 memoryBarrier = { VK_STRUCTURE_TYPE_MEMORY_BARRIER_2 };
		memoryBarrier.srcStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
		memoryBarrier.dstStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
		memoryBarrier.srcAccessMask = computeCall.srcAccessMask;
		memoryBarrier.dstAccessMask = computeCall.dstAccessMask;

		VkDependencyInfo dependencyInfo = { VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
		dependencyInfo.memoryBarrierCount = 1;
		dependencyInfo.pMemoryBarriers = &memoryBarrier;
		vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);
		DEBUG_LOG_TRACE("{} barrier, call = {}", renderStageNames[static_cast<size_t>(stage)], computeCallIndex);
	}
	template<RenderStage stage>
	void ComputeStage<stage>::RecordComputeToComputeShaderBarrier(VkCommandBuffer commandBuffer, size_t computeCallIndex) const
	{
		VkMemoryBarrier2 memoryBarrier = { VK_STRUCTURE_TYPE_MEMORY_BARRIER_2 };
		memoryBarrier.srcStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
		memoryBarrier.dstStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
		memoryBarrier.srcAccessMask = AccessMasks::ComputeShader::shaderWrite;
		memoryBarrier.dstAccessMask = AccessMasks::ComputeShader::shaderRead | AccessMasks::ComputeShader::shaderWrite;

		VkDependencyInfo dependencyInfo = { VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
		dependencyInfo.memoryBarrierCount = 1;
		dependencyInfo.pMemoryBarriers = &memoryBarrier;
		vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);
		DEBUG_LOG_TRACE("{} barrier, call = {}", renderStageNames[static_cast<size_t>(stage)], computeCallIndex);
	}
	template<RenderStage stage>
	void ComputeStage<stage>::RecordFinalPreRenderComputeBarrier(VkCommandBuffer commandBuffer) const
	{
		VkMemoryBarrier2 memoryBarrier = { VK_STRUCTURE_TYPE_MEMORY_BARRIER_2 };
		memoryBarrier.srcStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
		memoryBarrier.dstStageMask = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT;
		memoryBarrier.srcAccessMask = AccessMasks::ComputeShader::shaderWrite;
		memoryBarrier.dstAccessMask = AccessMasks::VertexShader::shaderRead;

		VkDependencyInfo dependencyInfo = { VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
		dependencyInfo.memoryBarrierCount = 1;
		dependencyInfo.pMemoryBarriers = &memoryBarrier;
		vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);
		DEBUG_LOG_TRACE("Memory barrier: pre-render compute to vertex");
	}



    // Explicit template instantiation:
	template class ComputeStage<RenderStage::preRenderCompute>;
	template class ComputeStage<RenderStage::midRenderCompute>;
	template class ComputeStage<RenderStage::screenSpaceCompute>;
	template class ComputeStage<RenderStage::postRenderCompute>;
}