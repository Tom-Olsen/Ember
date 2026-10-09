#pragma once
#include "jsonFwd.h"
#include "textureAsset.h"
#include <filesystem>
#include <string>



namespace emberAssetLoader
{
	class TextureAssetLoader
	{
	public: // Methods:
		static TextureAsset Load(const std::filesystem::path& path);

	private: // Methods:
		static void ValidateRootMembers(const nlohmann::json& json, const std::filesystem::path& path);
		static emberCommon::TextureType ParseTextureType(const std::filesystem::path& path, const std::string& value);
		static emberCommon::TextureFormat ParseTextureFormat(const std::filesystem::path& path, const std::string& value);
		static emberCommon::TextureUsage ParseTextureUsage(const std::filesystem::path& path, const std::string& value);
		static std::filesystem::path ResolveSourcePath(const std::filesystem::path& path, const std::string& sourcePath, emberCommon::TextureType textureType);
	};
}