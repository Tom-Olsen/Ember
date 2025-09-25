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
	Texture2d::Texture2d(const std::string& name, int width, int height, const TextureFormat& format, TextureUsage usage)
	{
		vulkanRendererBackend::Format vulkanFormat = vulkanRendererBackend::TextureFormatToVulkanFormat(format);
		switch (usage)
		{
			case TextureUsage::sample:
				m_pITexture = std::make_unique<vulkanRendererBackend::SampleTexture2d>(name, vulkanFormat, width, height);
				break;
			case TextureUsage::storage:
				m_pITexture = std::make_unique<vulkanRendererBackend::StorageTexture2d>(name, vulkanFormat, width, height);
				break;
			case TextureUsage::storageAndSample:
				m_pITexture = std::make_unique<vulkanRendererBackend::StorageSampleTexture2d>(name, vulkanFormat, width, height);
				break;
			case TextureUsage::renderTarget:
				m_pITexture = std::make_unique<vulkanRendererBackend::RenderTexture2d>(name, vulkanFormat, width, height);
				break;
			default LOG_ERROR("Texture2d: Unsuported TextureUsage '{}'", TextureUsageNames[usage]);
		}
	}
	Texture2d::Texture2d(const std::string& name, const TextureFormat& format, TextureUsage usage, const std::filesystem::path& path)
	{
		vulkanRendererBackend::Format vulkanFormat = vulkanRendererBackend::TextureFormatToVulkanFormat(format);
		switch (usage)
		{
		case TextureUsage::sample:
			m_pITexture = std::make_unique<vulkanRendererBackend::SampleTexture2d>(name, vulkanFormat, path);
			break;
		case TextureUsage::storage:
			m_pITexture = std::make_unique<vulkanRendererBackend::StorageTexture2d>(name, vulkanFormat, path);
			break;
		case TextureUsage::storageAndSample:
			m_pITexture = std::make_unique<vulkanRendererBackend::StorageSampleTexture2d>(name, vulkanFormat, path);
			break;
		case TextureUsage::renderTarget:
			m_pITexture = std::make_unique<vulkanRendererBackend::RenderTexture2d>(name, vulkanFormat);
			LOG_WARN("Texture2d: TextureUsage = 'renderTarget' does not support loading from disk. Use constructor without 'path'.")
			break;
			default LOG_ERROR("Texture2d: Unsuported TextureUsage '{}'", TextureUsageNames[usage]);
		}
	}
	Texture2d::~Texture2d()
	{

	}
}