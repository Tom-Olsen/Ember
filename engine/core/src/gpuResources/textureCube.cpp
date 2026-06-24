#include "textureCube.h"
#include "assetLoader.h"
#include "iTexture.h"
#include "renderer.h"



namespace emberCore
{
	TextureCube::TextureCube(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage)
	{
		m_ownsITexture = true;
        m_name = name;
		m_pITexture = Renderer::CreateTextureCube(width, height, format, usage, nullptr);
        m_pITexture->SetDebugName(m_name);
	}
	TextureCube::TextureCube(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const float> data) : TextureCube(name, width, height, format, usage)
	{
		SetData(data);
	}
	TextureCube::TextureCube(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const Float2> data) : TextureCube(name, width, height, format, usage)
	{
		SetData(data);
	}
	TextureCube::TextureCube(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const Float3> data) : TextureCube(name, width, height, format, usage)
	{
		SetData(data);
	}
	TextureCube::TextureCube(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const Float4> data) : TextureCube(name, width, height, format, usage)
	{
		SetData(data);
	}
	TextureCube::TextureCube(const std::string& name, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, const std::filesystem::path& path)
	{
		emberAssetLoader::Image imageAsset = emberAssetLoader::LoadImageCubeFiles(path, format.channels, false);
		m_ownsITexture = true;
        m_name = name;
		m_pITexture = Renderer::CreateTextureCube(imageAsset.width, imageAsset.height, format, usage, nullptr);
        m_pITexture->SetDebugName(m_name);
		SetRawData(std::as_bytes(std::span(imageAsset.pixels)));
	}
	TextureCube::~TextureCube()
	{

	}



	// Protected methods:
	uint64_t TextureCube::GetExpectedTexelCount() const
	{
		return 6ull * GetWidth() * GetHeight();
	}
}