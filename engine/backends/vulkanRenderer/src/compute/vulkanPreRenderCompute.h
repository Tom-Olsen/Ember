#pragma once
#include "iCompute.h"
#include "commonAccessMask.h"
#include "emberMath.h"
#include "vulkanAccessMask.h"
#include <vector>



// Forward declarations:
namespace emberBackendInterface
{
	class IComputeShader;
	class IShaderProperties;
}



namespace vulkanRendererBackend
{
	// Forward declarations:
	struct ComputeCall;



	class PreRender : public emberBackendInterface::ICompute::IPreRender
	{
	private: // Members:
		bool m_isInitialized;
		uint32_t m_callIndex;
		std::vector<ComputeCall> m_staticComputeCalls;
		std::vector<ComputeCall> m_dynamicComputeCalls;
		std::vector<ComputeCall*> m_computeCallPointers;

	public: // Methods:
		// Constructor/Destructor:
		PreRender();
		~PreRender();

		// Non-copyable:
		PreRender(const PreRender&) = delete;
		PreRender& operator=(const PreRender&) = delete;

		// Movable:
		PreRender(PreRender&& other) noexcept = default;
		PreRender& operator=(PreRender&& other) noexcept = default;

		// Workload recording:
		void RecordComputeShader(emberBackendInterface::IComputeShader* pComputeShader, emberBackendInterface::IShaderProperties* pShaderProperties, Uint3 threadCount) override;
		emberBackendInterface::IShaderProperties* RecordComputeShader(emberBackendInterface::IComputeShader* pComputeShader, Uint3 threadCount) override;
		void RecordBarrier(emberCommon::ComputeShaderAccessMask srcAccessMask, emberCommon::ComputeShaderAccessMask dstAccessMask) override;

		// Management:
		std::vector<ComputeCall*>& GetComputeCallPointers();
		void ResetComputeCalls();
	};
}