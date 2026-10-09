#pragma once
#include "commonTextureFormat.h"
#include "commonTextureType.h"
#include "commonTextureUsage.h"
#include <filesystem>
#include <string>



namespace emberAssetLoader
{
	struct TextureAsset
	{
	public: // Members:
		std::string textureName;
		emberCommon::TextureType textureType = emberCommon::TextureType::count;
		std::filesystem::path sourcePath;
		emberCommon::TextureFormat format = {};
		emberCommon::TextureUsage usage = emberCommon::TextureUsage::sample;
		bool flipImage = true;
	};
}