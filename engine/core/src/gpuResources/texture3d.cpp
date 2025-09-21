#include "texture2d.h"
#include "iTexture2d.h"
#include "vulkanRenderTexture2d.h"
#include "vulkanSampleTexture2d.h"
#include "vulkanStorageSampleTexture2d.h"
#include "vulkanStorageTexture2d.h"
#include "vulkanTextureCube.h"



namespace emberEngine
{
	Texture2d::Texture2d(const std::string& name, int width, int height, TextureFormat format, TextureUsage usage)
	{
		switch (usage)
		{
		case TextureUsage::sample:
			if (format == )
			m_pITexture2d = std::make_unique<vulkanRendererBackend::SampleTexture2d>(name, width, height);
			break;
		case TextureUsage::storage:
			m_pITexture2d = std::make_unique<vulkanRendererBackend::StorageTexture2d>(name);
			break;
		case TextureUsage::storageAndSample:
			m_pITexture2d = std::make_unique<vulkanRendererBackend::StorageSampleTexture2d>(name);
			break;
		case TextureUsage::renderTarget:
			m_pITexture2d = std::make_unique<vulkanRendererBackend::RenderTexture2d>(name);
			break;
		case TextureUsage::cubeMap:
			m_pITexture2d = std::make_unique<vulkanRendererBackend::TextureCube>(name);
			break;
		}
	}
	Texture2d::~Texture2d()
	{

	}
}