#include "texture2d.h"
#include "vulkanRenderTexture2d.h"
#include "vulkanSampleTexture2d.h"
#include "vulkanStorageSampleTexture2d.h"
#include "vulkanStorageTexture2d.h"
#include "vulkanTextureCube.h"



namespace emberEngine
{
	Texture2d::Texture2d(const std::string& name, int width, int height, const TextureFormat& format, TextureUsage usage)
	{
		switch (usage)
		{
		case TextureUsage::sample:
			m_pITexture = std::make_unique<vulkanRendererBackend::SampleTexture2d>(name, format, width, height);
			break;
		case TextureUsage::storage:
			m_pITexture = std::make_unique<vulkanRendererBackend::StorageTexture2d>(name);
			break;
		case TextureUsage::storageAndSample:
			m_pITexture = std::make_unique<vulkanRendererBackend::StorageSampleTexture2d>(name);
			break;
		case TextureUsage::renderTarget:
			m_pITexture = std::make_unique<vulkanRendererBackend::RenderTexture2d>(name);
			break;
		case TextureUsage::cubeMap:
			m_pITexture = std::make_unique<vulkanRendererBackend::TextureCube>(name);
			break;
		}
	}
	Texture2d::~Texture2d()
	{

	}
}