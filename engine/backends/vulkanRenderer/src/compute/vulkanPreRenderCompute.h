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
	class VULKAN_RENDERER_API PreRender : public emberBackendInterface::ICompute::IPreRender
	{
	private: // Members:
		ComputeCallQueue m_computeCallQueue;

	public: // Methods:
		// Constructor/Destructor:
		PreRender();
		~PreRender();

		// Non-copyable:
		PreRender(const PreRender&) = delete;
		PreRender& operator=(const PreRender&) = delete;

		// Movable:
		PreRender(PreRender&& other) noexcept;
		PreRender& operator=(PreRender&& other) noexcept;

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