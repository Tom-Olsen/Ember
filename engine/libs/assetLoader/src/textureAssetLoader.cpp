#include "textureAssetLoader.h"
#include "json.h"
#include "jsonUtility.h"
#include <array>
#include <stdexcept>



namespace emberAssetLoader
{
	// Public methods:
	TextureAsset TextureAssetLoader::Load(const std::filesystem::path& path)
	{
		nlohmann::json json = JsonUtility::LoadObject(path);
		ValidateRootMembers(json, path);
		TextureAsset textureAsset;
		textureAsset.textureName = JsonUtility::GetRequiredString(json, path, "textureName");
		textureAsset.textureType = ParseTextureType(path, JsonUtility::GetRequiredString(json, path, "textureType"));
		textureAsset.sourcePath = ResolveSourcePath(path, JsonUtility::GetRequiredString(json, path, "sourcePath"), textureAsset.textureType);
		textureAsset.format = ParseTextureFormat(path, JsonUtility::GetRequiredString(json, path, "format"));
		if (json.contains("usage"))
			textureAsset.usage = ParseTextureUsage(path, JsonUtility::GetRequiredString(json, path, "usage"));
		if (json.contains("flipImage"))
			textureAsset.flipImage = JsonUtility::GetRequiredBool(json, path, "flipImage");

		if (textureAsset.textureType == emberCommon::TextureType::textureCube && textureAsset.usage != emberCommon::TextureUsage::sample)
			throw std::runtime_error("TextureAssetLoader::Load(...) failed for '" + path.string() + "'. Cubemap assets only support sampled usage.");
		if (textureAsset.usage != emberCommon::TextureUsage::sample && emberCommon::IsSrgbFormat(textureAsset.format.flag))
			throw std::runtime_error("TextureAssetLoader::Load(...) failed for '" + path.string() + "'. Storage texture assets cannot use an sRGB format.");
		return textureAsset;
	}



	// Private methods:
	void TextureAssetLoader::ValidateRootMembers(const nlohmann::json& json, const std::filesystem::path& path)
	{
		for (auto iterator = json.begin(); iterator != json.end(); iterator++)
		{
			const std::string& memberName = iterator.key();
			if (memberName != "textureName"
				&& memberName != "textureType"
				&& memberName != "sourcePath"
				&& memberName != "format"
				&& memberName != "usage"
				&& memberName != "flipImage")
			{
				throw std::runtime_error("TextureAssetLoader::Load(...) failed for '" + path.string() + "'. Unknown member '" + memberName + "'.");
			}
		}
	}
	emberCommon::TextureType TextureAssetLoader::ParseTextureType(const std::filesystem::path& path, const std::string& value)
	{
		if (value == "texture2d")
			return emberCommon::TextureType::texture2d;
		if (value == "textureCube")
			return emberCommon::TextureType::textureCube;

		throw std::runtime_error("TextureAssetLoader::Load(...) failed for '" + path.string() + "'. Unsupported file-based texture type '" + value + "'.");
	}
	emberCommon::TextureFormat TextureAssetLoader::ParseTextureFormat(const std::filesystem::path& path, const std::string& value)
	{
		static const std::array<emberCommon::TextureFormat, 8> formats =
		{
			emberCommon::TextureFormats::r08_unorm,
			emberCommon::TextureFormats::r08_srgb,
			emberCommon::TextureFormats::rg08_unorm,
			emberCommon::TextureFormats::rg08_srgb,
			emberCommon::TextureFormats::rgb08_unorm,
			emberCommon::TextureFormats::rgb08_srgb,
			emberCommon::TextureFormats::rgba08_unorm,
			emberCommon::TextureFormats::rgba08_srgb
		};
		for (const emberCommon::TextureFormat& format : formats)
			if (value == emberCommon::FormatFlagToString(format.flag))
				return format;

		throw std::runtime_error("TextureAssetLoader::Load(...) failed for '" + path.string() + "'. Unsupported image format '" + value + "'. Image assets require an 8-bit unorm or sRGB format.");
	}
	emberCommon::TextureUsage TextureAssetLoader::ParseTextureUsage(const std::filesystem::path& path, const std::string& value)
	{
		if (value == "sample")
			return emberCommon::TextureUsage::sample;
		if (value == "storage")
			return emberCommon::TextureUsage::storage;
		if (value == "storageSample")
			return emberCommon::TextureUsage::storageSample;

		throw std::runtime_error("TextureAssetLoader::Load(...) failed for '" + path.string() + "'. Unsupported file-based texture usage '" + value + "'.");
	}
	std::filesystem::path TextureAssetLoader::ResolveSourcePath(const std::filesystem::path& path, const std::string& sourcePath, emberCommon::TextureType textureType)
	{
		if (textureType == emberCommon::TextureType::texture2d)
			return JsonUtility::ResolveFilePath(path, sourcePath, "sourcePath");

		std::filesystem::path resolvedPath = sourcePath;
		if (resolvedPath.is_relative())
			resolvedPath = path.parent_path() / resolvedPath;
		resolvedPath = std::filesystem::absolute(resolvedPath).lexically_normal().make_preferred();
		if (!std::filesystem::is_directory(resolvedPath))
			throw std::runtime_error("TextureAssetLoader::Load(...) failed for '" + path.string() + "'. Cubemap source directory does not exist: " + resolvedPath.string());
		return resolvedPath;
	}
}