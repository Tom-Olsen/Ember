#pragma once
#include "commonMsaaSampleCount.h"
#include <cstdint>
#include <sstream>
#include <string>



namespace emberCommon
{
	struct RendererCreateInfo
	{
		bool vSyncEnabled = true;
		uint32_t framesInFlight = 2;
		emberCommon::MsaaSampleCount msaaSampleCount = emberCommon::MsaaSampleCount::sampleCount04;
		uint32_t renderWidth = 2560;
		uint32_t renderHeight = 1440;
		bool enableGui = true;
		bool enableDockSpace = false;
		uint32_t maxDirectionalLights = 8;
		uint32_t maxPositionalLights = 30;
		uint32_t shadowMapResolution = 2048;

		inline std::string ToString() const
		{
			std::ostringstream ss;
			ss << "vSyncEnabled: " << vSyncEnabled << "\n";
			ss << "framesInFlight: " << framesInFlight << "\n";
			ss << "msaaSampleCount: " << MsaaSampleCountToString(msaaSampleCount) << "\n";
			ss << "renderWidth: " << renderWidth << "\n";
			ss << "renderHeight: " << renderHeight << "\n";
			ss << "enableGui: " << enableGui << "\n";
			ss << "enableDockSpace: " << enableDockSpace << "\n";
			ss << "maxDirectionalLights: " << maxDirectionalLights << "\n";
			ss << "maxPositionalLights: " << maxPositionalLights << "\n";
			ss << "shadowMapResolution: " << shadowMapResolution;
			return ss.str();
		}
	};
}