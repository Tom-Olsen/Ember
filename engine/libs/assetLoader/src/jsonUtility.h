#pragma once
#include "commonResourceAccessRights.h"
#include "jsonFwd.h"
#include <filesystem>
#include <string>
#include <string_view>



namespace emberAssetLoader
{
	namespace JsonUtility
	{
		nlohmann::json LoadObject(const std::filesystem::path& path);
		bool GetRequiredBool(const nlohmann::json& json, const std::filesystem::path& jsonPath, std::string_view memberName);
		std::string GetRequiredString(const nlohmann::json& json, const std::filesystem::path& jsonPath, std::string_view memberName);
		emberCommon::ResourceAccessRights GetResourceAccessRights(const nlohmann::json& json, const std::filesystem::path& jsonPath);
		std::filesystem::path ResolveFilePath(const std::filesystem::path& jsonPath, const std::string& filePath, std::string_view memberName);
	};
}