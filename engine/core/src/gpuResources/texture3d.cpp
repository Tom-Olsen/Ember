#include "texture3d.h"
#include "commonTextureType.h"
#include "gpuResourceFactory.h"
#include "logger.h"
#include "textureManager.h"



namespace emberCore
{
	// Public methods:
	// Constructor/Destructor:
	Texture3d::Texture3d() : Texture()
	{

	}
	Texture3d::Texture3d(const std::string& name, int width, int height, int depth, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, emberCommon::TextureImageCountMode imageCountMode)
		: Texture(TextureManager::AddTexture(name, GpuResourceFactory::CreateTexture3d(width, height, depth, format, usage, imageCountMode, nullptr), emberCommon::TextureType::texture3d))
	{

	}
	Texture3d::Texture3d(const std::string& name, int width, int height, int depth, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const float> data, emberCommon::TextureImageCountMode imageCountMode)
		: Texture3d(name, width, height, depth, format, usage, imageCountMode)
	{
		if (usage == emberCommon::TextureUsage::renderTarget)
			LOG_WARN("Texture3d: TextureUsage = 'renderTarget' does not support loading from float data. Ignoring data.");
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
	Texture3d::Texture3d(const std::string& name, int width, int height, int depth, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const Float2> data, emberCommon::TextureImageCountMode imageCountMode)
		: Texture3d(name, width, height, depth, format, usage, imageCountMode)
	{
		if (usage == emberCommon::TextureUsage::renderTarget)
			LOG_WARN("Texture3d: TextureUsage = 'renderTarget' does not support loading from Float2 data. Ignoring data.");
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
	Texture3d::Texture3d(const std::string& name, int width, int height, int depth, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const Float3> data, emberCommon::TextureImageCountMode imageCountMode)
		: Texture3d(name, width, height, depth, format, usage, imageCountMode)
	{
		if (usage == emberCommon::TextureUsage::renderTarget)
			LOG_WARN("Texture3d: TextureUsage = 'renderTarget' does not support loading from Float3 data. Ignoring data.");
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
	Texture3d::Texture3d(const std::string& name, int width, int height, int depth, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const Float4> data, emberCommon::TextureImageCountMode imageCountMode)
		: Texture3d(name, width, height, depth, format, usage, imageCountMode)
	{
		if (usage == emberCommon::TextureUsage::renderTarget)
			LOG_WARN("Texture3d: TextureUsage = 'renderTarget' does not support loading from Float4 data. Ignoring data.");
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
	Texture3d::~Texture3d() = default;



	// Private methods:
	Texture3d::Texture3d(const Texture& texture)
		: Texture(texture)
	{

	}
}