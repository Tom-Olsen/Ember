#pragma once
#include "computeShaderAsset.h"
#include "jsonFwd.h"
#include <filesystem>
#include <string>
#include <string_view>



namespace emberAssetLoader
{
	class ComputeShaderAssetLoader
	{
	public: // Methods:
		static ComputeShaderAsset Load(const std::filesystem::path& path);

	private: // Methods:
		static void ValidateRootMembers(const nlohmann::json& json, const std::filesystem::path& path);
		static void SetFeatures(ComputeShaderAsset& computeShaderAsset);
		static std::string FindMacroValue(const std::string& sourceCode, std::string_view macroName, const std::filesystem::path& sourcePath);
		static emberCommon::SceneColorAccess ParseSceneColorAccess(const std::string& macroValue, const std::filesystem::path& sourcePath);
	};
}