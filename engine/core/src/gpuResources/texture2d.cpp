#include "texture2d.h"
#include "iTexture.h"
#include "gpuResourceFactory.h"
#include "imageAssetLoader.h"
#include "logger.h"
#include <cstddef>
#include <span>



namespace emberCore
{
	// Public methods:
	// Constructor/Destructor:
	Texture2d::Texture2d() : Texture()
	{

	}
	Texture2d::Texture2d(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, emberCommon::TextureImageCountMode imageCountMode)
	{
		m_ownsITexture = true;
        m_name = name;
		m_pITexture = GpuResourceFactory::CreateTexture2d(width, height, format, usage, imageCountMode, nullptr);
        m_pITexture->SetDebugName(m_name);
	}
	Texture2d::Texture2d(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const float> data, emberCommon::TextureImageCountMode imageCountMode)
		: Texture2d(name, width, height, format, usage, imageCountMode)
	{
		if (usage == emberCommon::TextureUsage::renderTarget)
			LOG_WARN("Texture2d: TextureUsage = 'renderTarget' does not support loading from float data. Ignoring data.");
		else
			SetData(data);
	}
	Texture2d::Texture2d(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const Float2> data, emberCommon::TextureImageCountMode imageCountMode)
		: Texture2d(name, width, height, format, usage, imageCountMode)
	{
		if (usage == emberCommon::TextureUsage::renderTarget)
			LOG_WARN("Texture2d: TextureUsage = 'renderTarget' does not support loading from Float2 data. Ignoring data.");
		else
			SetData(data);
	}
	Texture2d::Texture2d(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const Float3> data, emberCommon::TextureImageCountMode imageCountMode)
		: Texture2d(name, width, height, format, usage, imageCountMode)
	{
		if (usage == emberCommon::TextureUsage::renderTarget)
			LOG_WARN("Texture2d: TextureUsage = 'renderTarget' does not support loading from Float3 data. Ignoring data.");
		else
			SetData(data);
	}
	Texture2d::Texture2d(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const Float4> data, emberCommon::TextureImageCountMode imageCountMode)
		: Texture2d(name, width, height, format, usage, imageCountMode)
	{
		if (usage == emberCommon::TextureUsage::renderTarget)
			LOG_WARN("Texture2d: TextureUsage = 'renderTarget' does not support loading from Float4 data. Ignoring data.");
		else
			SetData(data);
	}
	Texture2d::Texture2d(const std::string& name, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, const std::filesystem::path& path, emberCommon::TextureImageCountMode imageCountMode)
	{
		if (usage == emberCommon::TextureUsage::renderTarget)
			throw std::runtime_error("Texture2d: TextureUsage = 'renderTarget' does not support loading from path.");

		emberAssetLoader::ImageAsset imageAsset = emberAssetLoader::ImageAssetLoader::LoadFile(path, format.channels);
		m_ownsITexture = true;
        m_name = name;
		m_pITexture = GpuResourceFactory::CreateTexture2d(imageAsset.width, imageAsset.height, format, usage, imageCountMode, nullptr);
        m_pITexture->SetDebugName(m_name);
		SetRawData(std::as_bytes(std::span(imageAsset.pixels)));
	}
	Texture2d::Texture2d(emberBackendInterface::ITexture* pITexture, bool ownsTexture)
	{
		m_ownsITexture = ownsTexture;
		m_pITexture = pITexture;
	}
	Texture2d::~Texture2d()
	{

	}
}