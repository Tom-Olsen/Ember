#include "texture2d.h"
#include "logger.h"
#include "vulkanConvertTextureFormat.h"
#include "vulkanDepthTexture2dArray.h"
#include "vulkanFormat.h"
#include "vulkanRenderTexture2d.h"
#include "vulkanSampleTexture2d.h"
#include "vulkanStorageSampleTexture2d.h"
#include "vulkanStorageTexture2d.h"
#include "vulkanSampleTextureCube.h"



namespace emberEngine
{
	Texture2d::Texture2d(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage)
	{
		vulkanRendererBackend::Format vulkanFormat = vulkanRendererBackend::TextureFormatCommonToVulkan(format);
		switch (usage)
		{
			case emberCommon::TextureUsage::sample:
				m_pITexture = std::make_unique<vulkanRendererBackend::SampleTexture2d>(name, vulkanFormat, width, height);
				break;
			case emberCommon::TextureUsage::storage:
				m_pITexture = std::make_unique<vulkanRendererBackend::StorageTexture2d>(name, vulkanFormat, width, height);
				break;
			case emberCommon::TextureUsage::storageSample:
				m_pITexture = std::make_unique<vulkanRendererBackend::StorageSampleTexture2d>(name, vulkanFormat, width, height);
				break;
			case emberCommon::TextureUsage::renderTarget:
				m_pITexture = std::make_unique<vulkanRendererBackend::RenderTexture2d>(name, vulkanFormat, width, height);
				break;
			default: LOG_ERROR("Texture2d: Unsuported TextureUsage '{}'", emberCommon::TextureUsageNames[static_cast<int>(usage)]);
		}
	}
	Texture2d::Texture2d(const std::string& name, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, const std::filesystem::path& path)
	{
		vulkanRendererBackend::Format vulkanFormat = vulkanRendererBackend::TextureFormatCommonToVulkan(format);
		switch (usage)
		{
			case emberCommon::TextureUsage::sample:
				m_pITexture = std::make_unique<vulkanRendererBackend::SampleTexture2d>(name, vulkanFormat, path);
				break;
			case emberCommon::TextureUsage::storage:
				m_pITexture = std::make_unique<vulkanRendererBackend::StorageTexture2d>(name, vulkanFormat, path);
				break;
			case emberCommon::TextureUsage::storageSample:
				m_pITexture = std::make_unique<vulkanRendererBackend::StorageSampleTexture2d>(name, vulkanFormat, path);
				break;
			case emberCommon::TextureUsage::renderTarget:
				m_pITexture = nullptr;
				LOG_ERROR("Texture2d: TextureUsage = 'renderTarget' does not support loading from disk. Use constructor without 'path'.");
				break;
			default: LOG_ERROR("Texture2d: Unsuported TextureUsage '{}'", emberCommon::TextureUsageNames[static_cast<int>(usage)]);
		}
	}
	Texture2d::~Texture2d()
	{

	}
}