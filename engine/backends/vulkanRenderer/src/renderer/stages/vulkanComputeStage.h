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



	template<RenderStage stage>
	class ComputeStage
	{
	public: // Methods:
		void Record(const FrameContext& frameContext, std::span<const ComputeCall> computeCalls);

	private: // Methods:
		void RecordBarrier(VkCommandBuffer commandBuffer, const ComputeCall& computeCall, size_t computeCallIndex) const;
		void RecordFinalPostRenderComputeBarrier(VkCommandBuffer commandBuffer, size_t computeCallIndex) const;
		void RecordFinalPreRenderComputeBarrier(VkCommandBuffer commandBuffer) const;
	};
}