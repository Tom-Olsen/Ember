#pragma once
#include "vulkanComputeCall.h"
#include "vulkanRenderStage.h"
#include <cstddef>
#include <span>
#include <vulkan/vulkan.h>



namespace vulkanRendererBackend
{
	// Forward declarations:
	struct FrameContext;



	class ComputeStage
	{
	public: // Methods:
		ComputeStage() = delete;

		template<RenderStage stage>
		static void Record(const FrameContext& frameContext, std::span<const ComputeCall> computeCalls);
		static void Record(VkCommandBuffer commandBuffer, std::span<const ComputeCall> computeCalls, uint32_t frameIndex, float time, float deltaTime, std::span<const VkDescriptorSet> staticDescriptorSets);

	private: // Methods:
		static void RecordBufferCopy(VkCommandBuffer commandBuffer, const ComputeCall::BufferCopy& copy);
		static void RecordBufferBarrier(VkCommandBuffer commandBuffer, const ComputeCall::BufferBarrier& bufferBarrier);
		static void RecordBarrier(VkCommandBuffer commandBuffer, const ComputeCall& computeCall, size_t computeCallIndex, const char* debugName);
		static void RecordComputeToComputeShaderBarrier(VkCommandBuffer commandBuffer, size_t computeCallIndex, const char* debugName);
		static void RecordFinalPreRenderComputeBarrier(VkCommandBuffer commandBuffer);
		static void RecordInitialPostGeometryComputeBarrier(VkCommandBuffer commandBuffer, const FrameContext& frameContext);
		static void RecordFinalPostGeometryComputeBarrier(VkCommandBuffer commandBuffer, const FrameContext& frameContext);
		static void RecordCommands(VkCommandBuffer commandBuffer, std::span<const ComputeCall> computeCalls, uint32_t frameIndex, float time, float deltaTime, std::span<const VkDescriptorSet> staticDescriptorSets, const char* debugName);
	};
}