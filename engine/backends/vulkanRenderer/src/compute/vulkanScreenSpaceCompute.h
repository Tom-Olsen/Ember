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
	class VULKAN_RENDERER_API ScreenSpace : public emberBackendInterface::ICompute::IScreenSpace
	{
	private: // Members:
		ComputeCallQueue m_computeCallQueue;

	public: // Methods:
		// Constructor/Destructor:
		ScreenSpace();
		~ScreenSpace();

		// Non-copyable:
		ScreenSpace(const ScreenSpace&) = delete;
		ScreenSpace& operator=(const ScreenSpace&) = delete;

		// Movable:
		ScreenSpace(ScreenSpace&& other) noexcept;
		ScreenSpace& operator=(ScreenSpace&& other) noexcept;

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