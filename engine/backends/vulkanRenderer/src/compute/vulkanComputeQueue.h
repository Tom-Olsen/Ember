#pragma once
#include "iCompute.h"
#include "vulkanRendererExport.h"
#include <cstdint>
#include <vector>



namespace vulkanRendererBackend
{
	// Forward declarations:
	struct ComputeCall;
	class SceneColorPair;



	class VULKAN_RENDERER_API ComputeQueue : public emberBackendInterface::ICompute::IQueue
	{
	private: // Members:
		std::vector<ComputeCall> m_computeCalls;
		bool m_supportsSceneColor;
		bool m_isQueuing;

	public: // Methods:
		// Constructor/Destructor:
		explicit ComputeQueue(bool supportsSceneColor = false);
		~ComputeQueue() override;

		// Non-copyable:
		ComputeQueue(const ComputeQueue&) = delete;
		ComputeQueue& operator=(const ComputeQueue&) = delete;

		// Movable:
		ComputeQueue(ComputeQueue&& other) noexcept;
		ComputeQueue& operator=(ComputeQueue&& other) noexcept;

		// Workload dispatch:
		emberBackendInterface::IDescriptorSetBinding* DispatchComputeShader(emberBackendInterface::IComputeShader* pComputeShader, Uint3 threadCount) override;
		void DispatchBufferCopy(emberBackendInterface::IBuffer* pSource, emberBackendInterface::IBuffer* pDestination, uint64_t size, uint64_t sourceOffset = 0, uint64_t destinationOffset = 0) override;
		void DispatchBufferBarrier(emberBackendInterface::IBuffer* pBuffer, emberCommon::PipelineStage sourceStages, emberCommon::ComputeBarrierFlag sourceAccess, emberCommon::PipelineStage destinationStages, emberCommon::ComputeBarrierFlag destinationAccess, uint64_t offset = 0, uint64_t size = UINT64_MAX) override;
		void DispatchBarrier(emberCommon::ComputeBarrierFlag srcBarrierFlags, emberCommon::ComputeBarrierFlag dstBarrierFlags) override;

		// Management:
		void Close();
		void Reset();
		void UpdateShaderData(uint32_t frameIndex, SceneColorPair* pSceneColorPair = nullptr);

		// Getters:
		const std::vector<ComputeCall>& GetComputeCalls() const;
		bool IsEmpty() const;

	private: // Methods:
		void ValidateQueuing() const;
	};
}