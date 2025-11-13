#pragma once
#include "emberCoreExport.h"



// Forward declarations:
namespace emberBackendInterface
{
	class IWindow;
	class IRenderer;
	class ICompute;
	class IGui;
}



namespace emberEngine
{
	class EMBER_CORE_API Core
	{
	public: // Methods:
		// Initialization:
		static void InitBasics();
		static void InitBackends(emberBackendInterface::IWindow* pIWindow, emberBackendInterface::IRenderer* pIRenderer, emberBackendInterface::ICompute* pICompute, emberBackendInterface::IGui* pIGui);
		static void InitManagers();
		static void InitOther();

		// Cleanup:
		static void ClearBasics();
		static void ClearBackends();
		static void ClearManagers();
		static void ClearOther();
	};
}