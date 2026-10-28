#include "vulkanComputeStage.h"
#include "descriptorSetMacros.h"
#include "logger.h"
#include "profiler.h"
#include "vmaBuffer.h"
#include "vmaImage.h"
#include "vulkanAccessMask.h"
#include "vulkanBuffer.h"
#include "vulkanCommandPool.h"
#include "vulkanComputePushConstant.h"
#include "vulkanComputeShader.h"
#include "vulkanDescriptorSetBinding.h"
#include "vulkanFrameContext.h"
#include "vulkanFrameResources.h"
#include "vulkanGBufferTexture2d.h"
#include "vulkanMacros.h"
#include "vulkanPipeline.h"
#include "vulkanRenderTargetResources.h"
#include "vulkanSceneColorPair.h"
#include <stdexcept>
#include <string>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Public methods:
	template<RenderStage stage>
	void ComputeStage::Record(const FrameContext& frameContext, std::span<const ComputeCall> computeCalls)
	{
		static_assert(stage == RenderStage::preRenderCompute || stage == RenderStage::postOutlineCompute || stage == RenderStage::postGeometryCompute || stage == RenderStage::postLightingCompute || stage == RenderStage::postRenderCompute);
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
			if constexpr(stage == RenderStage::postLightingCompute || stage == RenderStage::postRenderCompute)
				frameContext.renderTargets.GetSceneColorPair().TransitionLayoutForCompute(commandBuffer, frameContext.frameExecutionData.frameIndex);

			if constexpr(stage == RenderStage::postGeometryCompute)
				if (!computeCalls.empty())
					RecordInitialPostGeometryComputeBarrier(commandBuffer, frameContext);

			RecordCommands(commandBuffer, computeCalls, frameContext.frameExecutionData.frameIndex,
				frameContext.frameExecutionData.time, frameContext.frameExecutionData.deltaTime,
				frameContext.resources.staticDescriptorSets, renderStageNames[static_cast<size_t>(stage)]);

			if constexpr(stage == RenderStage::postGeometryCompute)
				if (!computeCalls.empty())
					RecordFinalPostGeometryComputeBarrier(commandBuffer, frameContext);

			// Dispatch:
			if constexpr(stage == RenderStage::preRenderCompute)
				if (!computeCalls.empty())
					RecordFinalPreRenderComputeBarrier(commandBuffer);
		}
		VKA(vkEndCommandBuffer(commandBuffer));
	}
	void ComputeStage::Record(VkCommandBuffer commandBuffer, std::span<const ComputeCall> computeCalls, uint32_t frameIndex, float time, float deltaTime, std::span<const VkDescriptorSet> staticDescriptorSets)
	{
		VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		VKA(vkBeginCommandBuffer(commandBuffer, &beginInfo));
		RecordCommands(commandBuffer, computeCalls, frameIndex, time, deltaTime, staticDescriptorSets, "AsyncCompute");
		VKA(vkEndCommandBuffer(commandBuffer));
	}



	// Private methods:
	void ComputeStage::RecordBufferCopy(VkCommandBuffer commandBuffer, const ComputeCall::BufferCopy& copy)
	{
		if (!copy.source.IsValid() || !copy.destination.IsValid())
			return;

		VkBufferCopy region{};
		region.srcOffset = copy.sourceOffset;
		region.dstOffset = copy.destinationOffset;
		region.size = copy.size;
		vkCmdCopyBuffer(commandBuffer, copy.source.Get()->GetVmaBuffer()->GetVkBuffer(), copy.destination.Get()->GetVmaBuffer()->GetVkBuffer(), 1, &region);
	}
	void ComputeStage::RecordBufferBarrier(VkCommandBuffer commandBuffer, const ComputeCall::BufferBarrier& bufferBarrier)
	{
		if (!bufferBarrier.buffer.IsValid())
			return;

		VkBufferMemoryBarrier2 barrier{ VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2 };
		barrier.srcStageMask = bufferBarrier.sourceStages;
		barrier.srcAccessMask = bufferBarrier.sourceAccess;
		barrier.dstStageMask = bufferBarrier.destinationStages;
		barrier.dstAccessMask = bufferBarrier.destinationAccess;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.buffer = bufferBarrier.buffer.Get()->GetVmaBuffer()->GetVkBuffer();
		barrier.offset = bufferBarrier.offset;
		barrier.size = bufferBarrier.size;
		VkDependencyInfo dependencyInfo{ VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
		dependencyInfo.bufferMemoryBarrierCount = 1;
		dependencyInfo.pBufferMemoryBarriers = &barrier;
		vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);
	}
	void ComputeStage::RecordBarrier(VkCommandBuffer commandBuffer, const ComputeCall& computeCall, size_t computeCallIndex, const char* debugName)
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
		DEBUG_LOG_TRACE("{} barrier, call = {}", debugName, computeCallIndex);
	}
	void ComputeStage::RecordComputeToComputeShaderBarrier(VkCommandBuffer commandBuffer, size_t computeCallIndex, const char* debugName)
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
		DEBUG_LOG_TRACE("{} barrier, call = {}", debugName, computeCallIndex);
	}
	void ComputeStage::RecordFinalPreRenderComputeBarrier(VkCommandBuffer commandBuffer)
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
	void ComputeStage::RecordInitialPostGeometryComputeBarrier(VkCommandBuffer commandBuffer, const FrameContext& frameContext)
	{
		VmaImage* pImage = frameContext.renderTargets.GetSurfacePropertiesTexture(frameContext.frameExecutionData.frameIndex).GetVmaImage();
		pImage->TransitionLayout(
			commandBuffer,
			VK_IMAGE_LAYOUT_GENERAL,
			VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
			VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
			AccessMasks::ColorAttachmentOutput::colorAttachmentWrite,
			AccessMasks::ComputeShader::shaderRead | AccessMasks::ComputeShader::shaderWrite);
	}
	void ComputeStage::RecordFinalPostGeometryComputeBarrier(VkCommandBuffer commandBuffer, const FrameContext& frameContext)
	{
		VmaImage* pImage = frameContext.renderTargets.GetSurfacePropertiesTexture(frameContext.frameExecutionData.frameIndex).GetVmaImage();
		pImage->TransitionLayout(
			commandBuffer,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
			VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
			AccessMasks::ComputeShader::shaderRead | AccessMasks::ComputeShader::shaderWrite,
			AccessMasks::FragmentShader::sampledRead);
	}
	void ComputeStage::RecordCommands(VkCommandBuffer commandBuffer, std::span<const ComputeCall> computeCalls, uint32_t frameIndex, float time, float deltaTime, std::span<const VkDescriptorSet> staticDescriptorSets, const char* debugName)
	{
		// Pipeline:
		VkPipeline pipeline = VK_NULL_HANDLE;
		VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
		bool staticDescriptorSetsBound = false;

		// Compute calls:
		for (size_t computeCallIndex = 0; computeCallIndex < computeCalls.size(); computeCallIndex++)
		{
			const ComputeCall& computeCall = computeCalls[computeCallIndex];
			if (computeCall.IsBufferCopy())
			{
				RecordBufferCopy(commandBuffer, computeCall.GetBufferCopy());
				continue;
			}
			if (computeCall.IsBufferBarrier())
			{
				RecordBufferBarrier(commandBuffer, computeCall.GetBufferBarrier());
				continue;
			}
			if (computeCall.IsBarrier())
			{
				RecordBarrier(commandBuffer, computeCall, computeCallIndex, debugName);
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
				if (!staticDescriptorSetsBound && !staticDescriptorSets.empty())
				{
					vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, static_cast<uint32_t>(staticDescriptorSets.size()), staticDescriptorSets.data(), 0, nullptr);
					staticDescriptorSetsBound = true;
				}

				// Bind per shader descriptor set:
				if (VkDescriptorSet descriptorSet = pComputeShader->GetDescriptorSetBinding()->GetVkDescriptorSet(frameIndex); descriptorSet != VK_NULL_HANDLE)
					vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, SHADER_SET_INDEX, 1, &descriptorSet, 0, nullptr);
			}

			// Bind per compute call descriptor set:
			if (VkDescriptorSet descriptorSet = computeCall.callDescriptorSetBindingHandle.Get()->GetVkDescriptorSet(frameIndex); descriptorSet != VK_NULL_HANDLE)
				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, CALL_SET_INDEX, 1, &descriptorSet, 0, nullptr);

			// Push constant:
			ComputePushConstant pushConstant(computeCall.threadCount, time, deltaTime, computeCall.sceneColorIndex);
			vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(ComputePushConstant), &pushConstant);

			// Group counts:
			Uint3 blockSize = pComputeShader->GetBlockSize();
			uint32_t groupCountX = (computeCall.threadCount.x + blockSize.x - 1) / blockSize.x;
			uint32_t groupCountY = (computeCall.threadCount.y + blockSize.y - 1) / blockSize.y;
			uint32_t groupCountZ = (computeCall.threadCount.z + blockSize.z - 1) / blockSize.z;
			vkCmdDispatch(commandBuffer, groupCountX, groupCountY, groupCountZ);
			DEBUG_LOG_TRACE("{} shader {}, call = {}", debugName, pComputeShader->GetDebugName(), computeCallIndex);

			// Automatically synchronize scene-color writes before subsequent compute calls:
			if (pComputeShader->GetFeatures().WritesSceneColor())
				RecordComputeToComputeShaderBarrier(commandBuffer, computeCallIndex, debugName);
		}
	}



    // Explicit template instantiation:
	template void ComputeStage::Record<RenderStage::preRenderCompute>(const FrameContext& frameContext, std::span<const ComputeCall> computeCalls);
	template void ComputeStage::Record<RenderStage::postOutlineCompute>(const FrameContext& frameContext, std::span<const ComputeCall> computeCalls);
	template void ComputeStage::Record<RenderStage::postGeometryCompute>(const FrameContext& frameContext, std::span<const ComputeCall> computeCalls);
	template void ComputeStage::Record<RenderStage::postLightingCompute>(const FrameContext& frameContext, std::span<const ComputeCall> computeCalls);
	template void ComputeStage::Record<RenderStage::postRenderCompute>(const FrameContext& frameContext, std::span<const ComputeCall> computeCalls);
}