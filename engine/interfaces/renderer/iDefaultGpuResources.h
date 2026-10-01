#pragma once
#include <memory>



namespace emberBackendInterface
{
	// Forward declarations:
	class IComputeShader;



	class IDefaultGpuResources
	{
	public: // Methods:
		// Virtual destructor for v-table:
		virtual ~IDefaultGpuResources() = default;

		// Built-in compute shaders:
		virtual void InitializeBuiltInComputeShaders(
			std::unique_ptr<IComputeShader> pGammaCorrectionComputeShader,
			std::unique_ptr<IComputeShader> pOutlineCompositeComputeShader,
			std::unique_ptr<IComputeShader> pOutlineHorizontalMaskExpansionComputeShader,
			std::unique_ptr<IComputeShader> pOutlineVerticalMaskExpansionComputeShader) = 0;
	};
}