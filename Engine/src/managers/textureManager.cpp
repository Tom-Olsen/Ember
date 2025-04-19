#include "textureManager.h"
#include "depthTexture2dArray.h"
#include "sampleTexture2d.h"
#include "storageTexture2d.h"
#include "textureCube.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"
#include <filesystem>
#include <unordered_set>



namespace emberEngine
{
	using namespace vulkanBackend;



	// Static members:
	bool TextureManager::s_isInitialized = false;
	std::unordered_map<std::string, std::unique_ptr<Texture2d>> TextureManager::s_texture2ds;



	// Initialization and cleanup:
	void TextureManager::Init()
	{
		if (s_isInitialized)
			return;
		s_isInitialized = true;

		// Iterate through the texture directory:
		std::string engineRootPath = ENGINE_ROOT_PATH;
		std::filesystem::path directoryPath = engineRootPath + "/textures/";
		std::unordered_set<std::string> validExtensions = { ".png", ".jpg", ".jpeg", ".bmp" };
		for (const auto& entry : std::filesystem::directory_iterator(directoryPath))
		{
			if (entry.is_regular_file())
			{
				std::string extension = entry.path().extension().string();
				std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);	// convert to lower case (safer)
				if (validExtensions.find(extension) != validExtensions.end())
				{
					std::string name = entry.path().stem().string();
					VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;
					if (name.find("normal"))
						format = VK_FORMAT_R8G8B8A8_UNORM;
					else if (name.find("roughness") || name.find("metallic"))
						format = VK_FORMAT_R8_UNORM;
					SampleTexture2d* pTexture = new SampleTexture2d(name, format, entry.path());
					AddTexture2d(pTexture);
				}
			}
		}

		// TextureCubes:
		TextureCube* skyBoxWhite = new TextureCube(engineRootPath + "/textures/white/", "skyBoxWhite", VK_FORMAT_R8G8B8A8_SRGB);
		AddTexture2d(skyBoxWhite);
		TextureCube* skyBox0 = new TextureCube(engineRootPath + "/textures/skyboxClouds1/", "skyBox0", VK_FORMAT_R8G8B8A8_SRGB);
		AddTexture2d(skyBox0);
		//TextureCube* skyBox01 = new TextureCube(engineRootPath + "/textures/skyboxNebula0/", "skyBox1", VK_FORMAT_R8G8B8A8_SRGB);
		//AddTexture2d(skyBox1);


		// Default storage Texture2dArray:
		DepthTexture2dArray* blackArray = new DepthTexture2dArray("defaultArrayTexture2d", VK_FORMAT_D32_SFLOAT, 2, 2, 2);
		AddTexture2d(blackArray);

		// Default storage Texture2d:
		StorageTexture2d* storageTexture8x8 = new StorageTexture2d("defaultStorageTexture2d", VK_FORMAT_R32G32B32A32_SFLOAT, 8, 8);
		AddTexture2d(storageTexture8x8);
	}
	void TextureManager::Clear()
	{
		Context::WaitDeviceIdle();
		s_texture2ds.clear();
	}



	// Add/Get/Delete:
	void TextureManager::AddTexture2d(Texture2d* pTexture)
	{
		// If texture already contained in TextureManager, do nothing.
		if (s_texture2ds.emplace(pTexture->GetName(), std::unique_ptr<Texture2d>(pTexture)).second == false)
		{
			LOG_WARN("Texture2d with the name: {} already exists in TextureManager!", pTexture->GetName());
			return;
		}
	}
	Texture2d* TextureManager::GetTexture2d(const std::string& name)
	{
		auto it = s_texture2ds.find(name);
		if (it != s_texture2ds.end())
			return it->second.get();
		LOG_WARN("Texture2d '{}' not found!", name);
		return nullptr;
	}
	void TextureManager::DeleteTexture2d(const std::string& name)
	{
		Context::WaitDeviceIdle();
		s_texture2ds.erase(name);
	}



	// Debugging:
	void TextureManager::PrintAllTextureNames()
	{
		LOG_TRACE("Names of all managed texture2ds:");
		for (const auto& [name, _] : s_texture2ds)
			LOG_TRACE("  " + name);
	}
}