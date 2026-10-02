#pragma once
#include "emberCoreExport.h"
#include <memory>



// Forward declarations:
namespace emberBackendInterface
{
	class IComputeShader;
	class IDefaultGpuResources;
	class IMaterial;
}



namespace emberCore
{
	// Forward declarations:
	class ComputeShaderManager;
	class Core;
	class MaterialManager;



	class EMBER_CORE_API DefaultGpuResources
	{
		// Friends:
		friend class ComputeShaderManager;
		friend class Core;
		friend class MaterialManager;

	private: // Members:
		static bool s_isInitialized;
		static emberBackendInterface::IDefaultGpuResources* s_pIDefaultGpuResources;

	private: // Methods:
		// Initialization/Cleanup:
		static void Init(emberBackendInterface::IDefaultGpuResources* pDefaultGpuResources);
		static void Clear();

		// Built-in materials:
		static void InitializeBuiltInMaterials(
			std::unique_ptr<emberBackendInterface::IMaterial> pOutlineMaterial,
			std::unique_ptr<emberBackendInterface::IMaterial> pDefaultShadowMaterial,
			std::unique_ptr<emberBackendInterface::IMaterial> pDeferredLightingMaterial,
			std::unique_ptr<emberBackendInterface::IMaterial> pPresentMaterial);

		// Built-in compute shaders:
		static void InitializeBuiltInComputeShaders(
			std::unique_ptr<emberBackendInterface::IComputeShader> pGammaCorrectionComputeShader,
			std::unique_ptr<emberBackendInterface::IComputeShader> pOutlineCompositeComputeShader,
			std::unique_ptr<emberBackendInterface::IComputeShader> pOutlineHorizontalMaskExpansionComputeShader,
			std::unique_ptr<emberBackendInterface::IComputeShader> pOutlineVerticalMaskExpansionComputeShader);

		// Delete all constructors:
		DefaultGpuResources() = delete;
		DefaultGpuResources(const DefaultGpuResources&) = delete;
		DefaultGpuResources& operator=(const DefaultGpuResources&) = delete;
		DefaultGpuResources(DefaultGpuResources&&) = delete;
		DefaultGpuResources& operator=(DefaultGpuResources&&) = delete;
		~DefaultGpuResources() = delete;
	};
}