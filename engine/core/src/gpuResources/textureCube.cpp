#include "textureCube.h"
#include "assetLoader.h"
#include "iTexture.h"
#include "renderer.h"



namespace emberEngine
{
	TextureCube::TextureCube(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, void* data)
	{
		m_pITexture = std::unique_ptr<emberBackendInterface::ITexture>(Renderer::CreateTextureCube(name, width, height, format, usage, data));
	}
	TextureCube::TextureCube(const std::string& name, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, const std::filesystem::path& path)
	{
		emberAssetLoader::Image imageAsset = emberAssetLoader::LoadImageCubeFiles(path, format.channels, false);
		void* data = static_cast<void*>(imageAsset.pixels.data());
		m_pITexture = std::unique_ptr<emberBackendInterface::ITexture>(Renderer::CreateTextureCube(name, imageAsset.width, imageAsset.height, format, usage, data));
	}
	TextureCube::~TextureCube()
	{

	}
}