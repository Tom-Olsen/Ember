#pragma once
#include "commonComputeShaderFeatures.h"
#include <filesystem>
#include <string>



namespace emberCommon
{
	struct ComputeShaderCreateInfo
	{
		std::filesystem::path binaryPath;
		ComputeShaderFeatures features;
		std::string name;
	};
}