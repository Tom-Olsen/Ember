#pragma once
#include "emberCoreExport.h"
#include <memory>



// Forward declarations:
namespace emberBackendInterface
{
	class ICompute;
	class IDefaultGpuResources;
	class IGpuBackend;
	class IGpuResourceFactory;
	class IRenderer;
}



namespace emberCore
{
	// Forward declarations:
	class Core;

	class EMBER_CORE_API GpuBackend
	{
		// Friends:
		friend class Core;

	private: // Members:
		static std::unique_ptr<emberBackendInterface::IGpuBackend> s_pIGpuBackend;

	private: // Methods:
		// Initialization/Cleanup:
		static void Init(emberBackendInterface::IGpuBackend* pIGpuBackend);
		static void Clear();

		// Getters:
		static emberBackendInterface::ICompute* GetComputeInterface();
		static emberBackendInterface::IDefaultGpuResources* GetDefaultGpuResourcesInterface();
		static emberBackendInterface::IGpuResourceFactory* GetGpuResourceFactoryInterface();
		static emberBackendInterface::IRenderer* GetRendererInterface();

		// Delete all constructors:
		GpuBackend() = delete;
		GpuBackend(const GpuBackend&) = delete;
		GpuBackend& operator=(const GpuBackend&) = delete;
		GpuBackend(GpuBackend&&) = delete;
		GpuBackend& operator=(GpuBackend&&) = delete;
		~GpuBackend() = delete;
	};
}