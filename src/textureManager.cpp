#include "textureManager.h"
#include "vulkanMacros.h"
#include <unordered_set>



// Static members:
bool TextureManager::isInitialized = false;
VulkanContext* TextureManager::context;
std::unordered_map<std::string, std::unique_ptr<Texture2d>> TextureManager::texture2ds;



// Initialization and cleanup:
void TextureManager::Init(VulkanContext* vulkanContext)
{
	if (isInitialized)
		return;

	isInitialized = true;
	context = vulkanContext;

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
				Texture2d* texture = new Texture2d(context, entry.path(), entry.path().stem().string());
				AddTexture2d(texture->name, texture);
			}
		}
	}
}
void TextureManager::Clear()
{
	VKA(vkDeviceWaitIdle(context->LogicalDevice()));
	texture2ds.clear();
}



// Add/get/delete:
void TextureManager::AddTexture2d(const std::string name, Texture2d* texture)
{
	// If texture already contained in TextureManager, do nothing.
	if (texture2ds.emplace(name, std::unique_ptr<Texture2d>(texture)).second == false)
	{
		LOG_WARN("Texture2d with the name: {} already exists in TextureManager!", name);
		return;
	}
}
Texture2d* TextureManager::GetTexture2d(const std::string& name)
{
	auto it = texture2ds.find(name);
	if (it != texture2ds.end())
		return it->second.get();
	LOG_WARN("Texture2d '{}' not found!", name);
	return nullptr;
}
void TextureManager::DeleteTexture2d(const std::string& name)
{
	VKA(vkDeviceWaitIdle(context->LogicalDevice()));
	texture2ds.erase(name);
}



// Debugging:
void TextureManager::PrintAllTextureNames()
{
	LOG_TRACE("Names of all managed textures:");
	for (const auto& pair : texture2ds)
		LOG_TRACE(pair.first);
}