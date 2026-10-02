#pragma once
#include <memory>



namespace emberBackendInterface
{
	// Forward declarations:
	class IComputeShader;
	class IMaterial;



	class IDefaultGpuResources
	{
	public: // Methods:
		// Virtual destructor for v-table:
		virtual ~IDefaultGpuResources() = default;

		// Built-in materials:
		virtual void InitializeBuiltInMaterials(
			std::unique_ptr<IMaterial> pOutlineMaterial,
			std::unique_ptr<IMaterial> pDefaultShadowMaterial,
			std::unique_ptr<IMaterial> pDeferredLightingMaterial,
			std::unique_ptr<IMaterial> pPresentMaterial) = 0;

		// Built-in compute shaders:
		virtual void InitializeBuiltInComputeShaders(
			std::unique_ptr<IComputeShader> pGammaCorrectionComputeShader,
			std::unique_ptr<IComputeShader> pOutlineCompositeComputeShader,
			std::unique_ptr<IComputeShader> pOutlineHorizontalMaskExpansionComputeShader,
			std::unique_ptr<IComputeShader> pOutlineVerticalMaskExpansionComputeShader) = 0;
	};
}