#include "textureManager.h"
#include "logger.h"
#include "texture.h"
#include "texture2d.h"
#include "textureCube.h"
#include <filesystem>
#include <unordered_set>



namespace emberEngine
{
	// Static members:
	bool TextureManager::s_isInitialized = false;
	std::unordered_map<std::string, std::unique_ptr<Texture>> TextureManager::s_textures;



	// Initialization/Cleanup:
	void TextureManager::Init()
	{
		if (s_isInitialized)
			return;
		s_isInitialized = true;

		// Iterate through the texture directory: (for now everything interpreted as 2d sample textures)
		emberCommon::TextureUsage usage = emberCommon::TextureUsage::sample;
		std::filesystem::path directoryPath = (std::filesystem::path(ENGINE_RESOURCES_DIR) / "textures").make_preferred();
		std::unordered_set<std::string> validExtensions = { ".png", ".jpg", ".jpeg", ".bmp" };
		for (const auto& entry : std::filesystem::directory_iterator(directoryPath))
		{
			if (!entry.is_regular_file())
				continue;

			std::filesystem::path filePath = entry.path();
			std::string extension = filePath.extension().string();
			std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower); // normalize

			// Invalid extension:
			if (validExtensions.find(extension) == validExtensions.end())
				continue;

			std::string name = filePath.stem().string();
			emberCommon::TextureFormat format = emberCommon::TextureFormats::rgba08_srgb;

			if (name.find("normal") != std::string::npos)
				format = emberCommon::TextureFormats::rgba08_unorm;
			else if (name.find("roughness") != std::string::npos || name.find("metallic") != std::string::npos)
				format = emberCommon::TextureFormats::r08_unorm;

			Texture2d texture2d(name, format, usage, filePath);
			AddTexture(std::move(texture2d));
		}
		
		// TextureCubes:
		TextureCube skyboxWhite("skyboxWhite", emberCommon::TextureFormats::rgba08_srgb, emberCommon::TextureUsage::sample, directoryPath.string() + "/white/");
		AddTexture(std::move(skyboxWhite));
		TextureCube skybox0("skybox0", emberCommon::TextureFormats::rgba08_srgb, emberCommon::TextureUsage::sample, directoryPath.string() + "/skyboxClouds1/");
		AddTexture(std::move(skybox0));
		TextureCube skybox1("skybox1", emberCommon::TextureFormats::rgba08_srgb, emberCommon::TextureUsage::sample, directoryPath.string() + "/skyboxNebula0/");
		AddTexture(std::move(skybox1));
	}
	void TextureManager::Clear()
	{
		s_textures.clear();
		s_isInitialized = false;
	}



	// Add/Get/Delete:
	void TextureManager::AddTexture(Texture&& texture)
	{
		auto newTexture = std::make_unique<Texture>(std::move(texture));
		std::string name = newTexture->GetName();
		if (!s_textures.emplace(name, std::move(newTexture)).second)
			LOG_WARN("Texture with the name: {} already exists in TextureManager!", name);
	}
	Texture& TextureManager::GetTexture(const std::string& name)
	{
		auto it = s_textures.find(name);
		if (it == s_textures.end())
			throw std::runtime_error("Texture not found: " + name);
		return *(it->second);
	}
	Texture* TextureManager::TryGetTexture(const std::string& name)
	{
		auto it = s_textures.find(name);
		if (it != s_textures.end())
			return it->second.get();
		LOG_WARN("Texture '{}' not found!", name);
		return nullptr;
	}
	void TextureManager::DeleteTexture(const std::string& name)
	{
		s_textures.erase(name);
	}



	// Debugging:
	void TextureManager::Print()
	{
		LOG_TRACE("Names of all managed textures:");
		for (const auto& [name, _] : s_textures)
			LOG_TRACE("  " + name);
	}
}