#pragma once
#include "iCompute.h"
#include "emberMath.h"
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
	// Forward declarations:
	struct ComputeCall;



	class VULKAN_RENDERER_API PreRender : public emberBackendInterface::ICompute::IPreRender
	{
	private: // Members:
		bool m_isInitialized;
		std::vector<ComputeCall> m_computeCalls;
		std::vector<std::vector<ComputeCall>> m_submittedComputeCalls;

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
		void CompleteComputeCalls(uint32_t frameIndex);
		void CompleteAllComputeCalls();
		std::vector<ComputeCall>& GetComputeCalls();
		void ResetComputeCalls();

	private: // Methods:
		void ReleaseComputeCalls(std::vector<ComputeCall>& computeCalls);
	};
}