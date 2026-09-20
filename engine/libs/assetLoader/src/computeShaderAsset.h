#pragma once
#include "commonComputeShaderFeatures.h"
#include "commonResourceAccessRights.h"
#include <filesystem>
#include <string>



namespace emberAssetLoader
{
	struct ComputeShaderAsset
	{
		// Members:
		std::string computeShaderName;
		std::filesystem::path sourcePath;
		std::filesystem::path binaryPath;
		emberCommon::ComputeShaderFeatures features;
		emberCommon::ResourceAccessRights accessRights;
	};
}