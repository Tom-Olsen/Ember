#pragma once
#include "iCompute.h"
#include "emberMath.h"
#include "vulkanComputeCallQueue.h"
#include "vulkanRendererExport.h"
#include <cstdint>



// Forward declarations:
namespace emberBackendInterface
{
	class IComputeShader;
	class IDescriptorSetBinding;
}



namespace vulkanRendererBackend
{
	class VULKAN_RENDERER_API Render : public emberBackendInterface::ICompute::IRender
	{
	private: // Members:
		ComputeCallQueue m_computeCallQueue;

	public: // Methods:
		// Constructor/Destructor:
		Render();
		~Render();

		// Non-copyable:
		Render(const Render&) = delete;
		Render& operator=(const Render&) = delete;

		// Movable:
		Render(Render&& other) noexcept;
		Render& operator=(Render&& other) noexcept;

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
	};
}