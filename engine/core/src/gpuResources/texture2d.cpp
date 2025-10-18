#include "texture2d.h"
#include "assetLoader.h"
#include "logger.h"
#include "iTexture.h"
#include "renderer.h"



namespace emberEngine
{
	// Public methods:
	// Constructor/Destructor:
	Texture2d::Texture2d(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, void* data)
	{
		if (usage == emberCommon::TextureUsage::renderTarget && data != nullptr)
			LOG_WARN("Texture2d: TextureUsage = 'renderTarget' does not support loading from void* data. Ignoring data.");
		m_ownsTexture = true;
		m_pITexture = Renderer::CreateTexture2d(name, width, height, format, usage, data);
	}
	Texture2d::Texture2d(const std::string& name, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, const std::filesystem::path& path)
	{
		if (usage == emberCommon::TextureUsage::renderTarget)
			throw std::runtime_error("Texture2d: TextureUsage = 'renderTarget' does not support loading from path.");

		emberAssetLoader::Image imageAsset = emberAssetLoader::LoadImageFile(path, format.channels);
		void* data = static_cast<void*>(imageAsset.pixels.data());
		m_ownsTexture = true;
		m_pITexture = Renderer::CreateTexture2d(name, imageAsset.width, imageAsset.height, format, usage, data);
	}
	Texture2d::Texture2d(emberBackendInterface::ITexture* pITexture, bool ownsTexture)
	{
		m_ownsTexture = ownsTexture;
		m_pITexture = pITexture;
	}
	Texture2d::~Texture2d()
	{

	}
}