#include "textureCube.h"
#include "commonTextureType.h"
#include "gpuResourceFactory.h"
#include "imageLoader.h"
#include "textureManager.h"
#include <span>



namespace emberCore
{
	// Public methods:
	// Constructor/Destructor:
	TextureCube::TextureCube() = default;
	TextureCube::TextureCube(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, emberCommon::TextureImageCountMode imageCountMode)
		: Texture(TextureManager::AddTexture(name, GpuResourceFactory::CreateTextureCube(width, height, format, usage, imageCountMode, nullptr), emberCommon::TextureType::textureCube))
	{

	}
	TextureCube::TextureCube(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const float> data, emberCommon::TextureImageCountMode imageCountMode)
		: TextureCube(name, width, height, format, usage, imageCountMode)
	{
		try
		{
			SetData(data);
		}
		catch (...)
		{
			Destroy();
			throw;
		}
	}
	TextureCube::TextureCube(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const Float2> data, emberCommon::TextureImageCountMode imageCountMode)
		: TextureCube(name, width, height, format, usage, imageCountMode)
	{
		try
		{
			SetData(data);
		}
		catch (...)
		{
			Destroy();
			throw;
		}
	}
	TextureCube::TextureCube(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const Float3> data, emberCommon::TextureImageCountMode imageCountMode)
		: TextureCube(name, width, height, format, usage, imageCountMode)
	{
		try
		{
			SetData(data);
		}
		catch (...)
		{
			Destroy();
			throw;
		}
	}
	TextureCube::TextureCube(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const Float4> data, emberCommon::TextureImageCountMode imageCountMode)
		: TextureCube(name, width, height, format, usage, imageCountMode)
	{
		try
		{
			SetData(data);
		}
		catch (...)
		{
			Destroy();
			throw;
		}
	}
	TextureCube::TextureCube(const std::string& name, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, const std::filesystem::path& path, emberCommon::TextureImageCountMode imageCountMode)
	{
		emberAssetLoader::ImageData imageData = emberAssetLoader::ImageLoader::LoadCubeFiles(path, format.channels, false);
		m_textureId = TextureManager::AddTexture(name, GpuResourceFactory::CreateTextureCube(imageData.width, imageData.height, format, usage, imageCountMode, nullptr), emberCommon::TextureType::textureCube);
		try
		{
			SetRawData(std::as_bytes(std::span(imageData.pixels)));
		}
		catch (...)
		{
			Destroy();
			throw;
		}
	}
	TextureCube::~TextureCube() = default;



	// Private methods:
	TextureCube::TextureCube(const Texture& texture)
		: Texture(texture)
	{

	}
}