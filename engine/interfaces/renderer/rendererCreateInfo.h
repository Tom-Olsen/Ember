#pragma once
#include "msaaSampleCount.h"



// Forward decleration:
namespace emberBackendInterface
{
	class IDearImGui;
	class IWindow;
}



namespace emberEngine
{
	struct RendererCreateInfo
	{
		emberBackendInterface::IWindow* pIWindow = nullptr;
		emberBackendInterface::IDearImGui* pIDearImGui = nullptr;
		bool vSyncEnabled = true;
		uint32_t framesInFlight = 2;
		MsaaSampleCount msaaSampleCount = MsaaSampleCount::sampleCount04;
		uint32_t renderWidth = 2560;
		uint32_t renderHeight = 1440;
		bool enableDearImGui = true;
		bool enableDockSpace = false;
		uint32_t maxDirectionalLights = 8;
		uint32_t maxPositionalLights = 30;
		uint32_t shadowMapResolution = 2048;
	};
}