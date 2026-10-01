#pragma once
#include "emberCoreExport.h"



// Forward declarations:
namespace emberBackendInterface
{
	class IGpuBackend;
	class IGui;
	class IWindow;
}



namespace emberCore
{
	class EMBER_CORE_API Core
	{
	public: // Methods:
		// Initialization/Cleanup:
		static void Init(emberBackendInterface::IWindow* pIWindow, emberBackendInterface::IGpuBackend* pIGpuBackend, emberBackendInterface::IGui* pIGui);
		static void Clear();

	private: // Methods:
		// Initialization:
		static void InitBackends(emberBackendInterface::IWindow* pIWindow, emberBackendInterface::IGpuBackend* pIGpuBackend, emberBackendInterface::IGui* pIGui);
		static void InitManagers();
		static void InitOther();

		// Cleanup:
		static void ClearBackends();
		static void ClearManagers();
		static void ClearOther();
	};
}