#include "textureManager.h"
#include "emberTime.h"
#include "logger.h"
#include "macros.h"
#include "texture.h"
#include "texture2d.h"
#include "textureCube.h"
#include <filesystem>
#include <unordered_set>



namespace emberEngine
{
	using namespace vulkanBackend;



	// Static members:
	bool TextureManager::s_isInitialized = false;
	std::unordered_map<std::string, std::unique_ptr<Texture>> TextureManager::s_textures;



	// Initialization/Cleanup:
	void TextureManager::Init()
	{
		if (s_isInitialized)
			return;
		s_isInitialized = true;

		#ifdef LOG_INITIALIZATION
		Time::Init();
		#endif

		// Iterate through the texture directory:
		std::filesystem::path directoryPath = (std::filesystem::path(ENGINE_CORE_PATH) / "textures").make_preferred();
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
			TextureFormat format = TextureFormats::rgba08_srgb;

			if (name.find("normal") != std::string::npos)
				format = TextureFormats::rgba08_unorm;
			else if (name.find("roughness") != std::string::npos || name.find("metallic") != std::string::npos)
				format = TextureFormats::r08_unorm;

			Texture2d* pTexture2d = new Texture2d(name, format, filePath);
			AddTexture(pTexture2d);
		}
		
		// TextureCubes:
		TextureCube* skyboxWhite = new TextureCube("skyboxWhite", TextureFormats::rgba08_srgb, directoryPath.string() + "/white/");
		AddTexture(skyboxWhite);
		TextureCube* skybox0 = new TextureCube("skybox0", TextureFormats::rgba08_srgb, directoryPath.string() + "/skyboxClouds1/");
		AddTexture(skybox0);
		TextureCube* skybox1 = new TextureCube("skybox1", TextureFormats::rgba08_srgb, directoryPath.string() + "/skyboxNebula0/");
		AddTexture(skybox1);

		#ifdef LOG_INITIALIZATION
		Time::Update();
		LOG_TRACE("TextureManager initialized. ({}s)", Time::GetDeltaTime());
		#endif
	}
	void TextureManager::Clear()
	{
		Context::WaitDeviceIdle();
		s_textures.clear();
	}



	// Add/Get/Delete:
	void TextureManager::AddTexture(Texture* pTexture)
	{
		// If texture already contained in TextureManager, do nothing.
		if (s_textures.emplace(pTexture->GetName(), std::unique_ptr<Texture>(pTexture)).second == false)
		{
			LOG_WARN("Texture with the name: {} already exists in TextureManager!", pTexture->GetName());
			return;
		}
	}
	Texture* TextureManager::GetTexture(const std::string& name)
	{
		auto it = s_textures.find(name);
		if (it != s_textures.end())
			return it->second.get();
		LOG_WARN("Texture '{}' not found!", name);
		return nullptr;
	}
	void TextureManager::DeleteTexture(const std::string& name)
	{
		Context::WaitDeviceIdle();
		s_textures.erase(name);
	}



	// Debugging:
	void TextureManager::PrintAllTextureNames()
	{
		LOG_TRACE("Names of all managed textures:");
		for (const auto& [name, _] : s_textures)
			LOG_TRACE("  " + name);
	}
}