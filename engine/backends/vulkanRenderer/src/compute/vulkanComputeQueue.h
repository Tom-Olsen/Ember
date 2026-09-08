#pragma once
#include "iCompute.h"
#include "emberMath.h"
#include "vulkanComputeCallQueue.h"
#include "vulkanRendererExport.h"
#include <cstdint>
#include <vector>



// Forward declarations:
namespace emberBackendInterface
{
	class IComputeShader;
	class IDescriptorSetBinding;
}



namespace vulkanRendererBackend
{
	/// <summary>
	/// Base class for different compute queues: preRender, midRender, screenSpace, postRender.
	/// </summary>
	class VULKAN_RENDERER_API ComputeQueue : public virtual emberBackendInterface::ICompute::IQueue
	{
	private: // Members:
		ComputeCallQueue m_computeCallQueue;

	public: // Methods:
		// Constructor/Destructor:
		ComputeQueue();
		~ComputeQueue() override;

		// Non-copyable:
		ComputeQueue(const ComputeQueue&) = delete;
		ComputeQueue& operator=(const ComputeQueue&) = delete;

		// Movable:
		ComputeQueue(ComputeQueue&& other) noexcept;
		ComputeQueue& operator=(ComputeQueue&& other) noexcept;

		// Workload recording:
		emberBackendInterface::IDescriptorSetBinding* RecordComputeShader(emberBackendInterface::IComputeShader* pComputeShader, Uint3 threadCount) override;
		void RecordBarrier(emberBackendInterface::ComputeBarrierFlag srcBarrierFlags, emberBackendInterface::ComputeBarrierFlag dstBarrierFlags) override;

		// Management:
		void CommitComputeCalls(uint32_t frameIndex);
		void RetireComputeCalls(uint32_t frameIndex);
		void RetireAllComputeCalls();
		std::vector<ComputeCall>& GetComputeCalls();
		void ResetComputeCalls();
		void UpdateShaderData(uint32_t frameIndex);

	protected: // Methods:
		emberBackendInterface::IDescriptorSetBinding* RecordComputeCall(ComputeCall computeCall);
	};
}