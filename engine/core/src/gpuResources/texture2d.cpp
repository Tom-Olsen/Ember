#include "texture2d.h"
#include "commonTextureType.h"
#include "gpuResourceFactory.h"
#include "imageLoader.h"
#include "logger.h"
#include "textureManager.h"
#include <cstddef>
#include <span>
#include <stdexcept>



namespace emberCore
{
	// Public methods:
	// Constructor/Destructor:
	Texture2d::Texture2d() : Texture()
	{

	}
	Texture2d::Texture2d(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, emberCommon::TextureImageCountMode imageCountMode)
		: Texture(TextureManager::AddTexture(name, GpuResourceFactory::CreateTexture2d(width, height, format, usage, imageCountMode, nullptr), emberCommon::TextureType::texture2d))
	{

	}
	Texture2d::Texture2d(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const float> data, emberCommon::TextureImageCountMode imageCountMode)
		: Texture2d(name, width, height, format, usage, imageCountMode)
	{
		if (usage == emberCommon::TextureUsage::renderTarget)
			LOG_WARN("Texture2d: TextureUsage = 'renderTarget' does not support loading from float data. Ignoring data.");
		else
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
	}
	Texture2d::Texture2d(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const Float2> data, emberCommon::TextureImageCountMode imageCountMode)
		: Texture2d(name, width, height, format, usage, imageCountMode)
	{
		if (usage == emberCommon::TextureUsage::renderTarget)
			LOG_WARN("Texture2d: TextureUsage = 'renderTarget' does not support loading from Float2 data. Ignoring data.");
		else
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
	}
	Texture2d::Texture2d(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const Float3> data, emberCommon::TextureImageCountMode imageCountMode)
		: Texture2d(name, width, height, format, usage, imageCountMode)
	{
		if (usage == emberCommon::TextureUsage::renderTarget)
			LOG_WARN("Texture2d: TextureUsage = 'renderTarget' does not support loading from Float3 data. Ignoring data.");
		else
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
	}
	Texture2d::Texture2d(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const Float4> data, emberCommon::TextureImageCountMode imageCountMode)
		: Texture2d(name, width, height, format, usage, imageCountMode)
	{
		if (usage == emberCommon::TextureUsage::renderTarget)
			LOG_WARN("Texture2d: TextureUsage = 'renderTarget' does not support loading from Float4 data. Ignoring data.");
		else
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
	}
	Texture2d::Texture2d(const std::string& name, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, const std::filesystem::path& path, emberCommon::TextureImageCountMode imageCountMode)
	{
		if (usage == emberCommon::TextureUsage::renderTarget)
			throw std::runtime_error("Texture2d: TextureUsage = 'renderTarget' does not support loading from path.");

		emberAssetLoader::ImageData imageData = emberAssetLoader::ImageLoader::LoadFile(path, format.channels);
		m_textureId = TextureManager::AddTexture(name, GpuResourceFactory::CreateTexture2d(imageData.width, imageData.height, format, usage, imageCountMode, nullptr), emberCommon::TextureType::texture2d);
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
	Texture2d::~Texture2d() = default;



	// Private methods:
	Texture2d::Texture2d(const Texture& texture)
		: Texture(texture)
	{

	}
}