#pragma once
#include "emberCoreExport.h"
#include <memory>



// Forward declarations:
namespace emberBackendInterface
{
	class IComputeShader;
	class IDefaultGpuResources;
}



namespace emberCore
{
	// Forward declarations:
	class ComputeShaderManager;
	class Core;



	class EMBER_CORE_API DefaultGpuResources
	{
		// Friends:
		friend class ComputeShaderManager;
		friend class Core;

	private: // Members:
		static bool s_isInitialized;
		static emberBackendInterface::IDefaultGpuResources* s_pIDefaultGpuResources;

	private: // Methods:
		// Initialization/Cleanup:
		static void Init(emberBackendInterface::IDefaultGpuResources* pDefaultGpuResources);
		static void Clear();

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