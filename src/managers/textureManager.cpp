#include "textureManager.h"
#include "texture2d.h"
#include "textureCube.h"
#include "vulkanContext.h"
#include "vulkanMacros.h"
#include <filesystem>
#include <unordered_set>



namespace emberEngine
{
	// Static members:
	bool TextureManager::s_isInitialized = false;
	VulkanContext* TextureManager::s_pContext;
	std::unordered_map<std::string, std::unique_ptr<Texture2d>> TextureManager::s_texture2ds;
	std::unordered_map<std::string, std::unique_ptr<TextureCube>> TextureManager::s_textureCubes;



	// Initialization and cleanup:
	void TextureManager::Init(VulkanContext* pContext)
	{
		if (s_isInitialized)
			return;

		s_isInitialized = true;
		s_pContext = pContext;

		// Iterate through the texture directory:
		std::filesystem::path directoryPath = "../textures";
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
					Texture2d* texture = new Texture2d(s_pContext, entry.path(), name, format);
					AddTexture2d(texture);
				}
			}
		}

		// Manual adding:
		TextureCube* texCube = new TextureCube(s_pContext, "../textures/skyboxClouds1/", "skyboxClouds0", VK_FORMAT_R8G8B8A8_SRGB);
		//TextureCube* texCube = new TextureCube(s_pContext, "../textures/skyboxNebula0/", "skyboxClouds0", VK_FORMAT_R8G8B8A8_SRGB);
		AddTextureCube(texCube);
	}
	void TextureManager::Clear()
	{
		s_pContext->WaitDeviceIdle();
		s_texture2ds.clear();
		s_textureCubes.clear();
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
	void TextureManager::AddTextureCube(TextureCube* pTexture)
	{
		// If texture already contained in TextureManager, do nothing.
		if (s_textureCubes.emplace(pTexture->GetName(), std::unique_ptr<TextureCube>(pTexture)).second == false)
		{
			LOG_WARN("TextureCube with the name: {} already exists in TextureManager!", pTexture->GetName());
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
	TextureCube* TextureManager::GetTextureCube(const std::string& name)
	{
		auto it = s_textureCubes.find(name);
		if (it != s_textureCubes.end())
			return it->second.get();
		LOG_WARN("TextureCube '{}' not found!", name);
		return nullptr;
	}
	void TextureManager::DeleteTexture2d(const std::string& name)
	{
		s_pContext->WaitDeviceIdle();
		s_texture2ds.erase(name);
	}
	void TextureManager::DeleteTextureCube(const std::string& name)
	{
		s_pContext->WaitDeviceIdle();
		s_textureCubes.erase(name);
	}



	// Debugging:
	void TextureManager::PrintAllTextureNames()
	{
		LOG_TRACE("Names of all managed textures:");
		LOG_TRACE("  s_texture2ds:");
		for (const auto& [name, _] : s_texture2ds)
			LOG_TRACE("    " + name);
		LOG_TRACE("  s_textureCubes:");
		for (const auto& [name, _] : s_textureCubes)
			LOG_TRACE("    " + name);
	}
}