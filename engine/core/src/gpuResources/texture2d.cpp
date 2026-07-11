#include "texture2d.h"
#include "assetLoader.h"
#include "iTexture.h"
#include "logger.h"
#include "renderer.h"
#include <cstddef>
#include <span>



namespace emberCore
{
	// Public methods:
	// Constructor/Destructor:
	Texture2d::Texture2d() : Texture()
	{

	}
	Texture2d::Texture2d(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage)
	{
		m_ownsITexture = true;
        m_name = name;
		m_pITexture = Renderer::CreateTexture2d(width, height, format, usage, nullptr);
        m_pITexture->SetDebugName(m_name);
	}
	Texture2d::Texture2d(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const float> data) : Texture2d(name, width, height, format, usage)
	{
		if (usage == emberCommon::TextureUsage::renderTarget)
			LOG_WARN("Texture2d: TextureUsage = 'renderTarget' does not support loading from float data. Ignoring data.");
		else
			SetData(data);
	}
	Texture2d::Texture2d(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const Float2> data) : Texture2d(name, width, height, format, usage)
	{
		if (usage == emberCommon::TextureUsage::renderTarget)
			LOG_WARN("Texture2d: TextureUsage = 'renderTarget' does not support loading from Float2 data. Ignoring data.");
		else
			SetData(data);
	}
	Texture2d::Texture2d(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const Float3> data) : Texture2d(name, width, height, format, usage)
	{
		if (usage == emberCommon::TextureUsage::renderTarget)
			LOG_WARN("Texture2d: TextureUsage = 'renderTarget' does not support loading from Float3 data. Ignoring data.");
		else
			SetData(data);
	}
	Texture2d::Texture2d(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const Float4> data) : Texture2d(name, width, height, format, usage)
	{
		if (usage == emberCommon::TextureUsage::renderTarget)
			LOG_WARN("Texture2d: TextureUsage = 'renderTarget' does not support loading from Float4 data. Ignoring data.");
		else
			SetData(data);
	}
	Texture2d::Texture2d(const std::string& name, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, const std::filesystem::path& path)
	{
		if (usage == emberCommon::TextureUsage::renderTarget)
			throw std::runtime_error("Texture2d: TextureUsage = 'renderTarget' does not support loading from path.");

		emberAssetLoader::Image imageAsset = emberAssetLoader::LoadImageFile(path, format.channels);
		m_ownsITexture = true;
        m_name = name;
		m_pITexture = Renderer::CreateTexture2d(imageAsset.width, imageAsset.height, format, usage, nullptr);
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