#include "textureCube.h"
#include "logger.h"
#include "vulkanConvertTextureFormat.h"
#include "vulkanFormat.h"
#include "vulkanSampleTextureCube.h"



namespace emberEngine
{
	TextureCube::TextureCube(const std::string& name, int width, int height, const TextureFormat& format, TextureUsage usage)
	{
		vulkanRendererBackend::Format vulkanFormat = vulkanRendererBackend::TextureFormatToVulkanFormat(format);
		switch (usage)
		{
			case TextureUsage::sample:
				m_pITexture = std::make_unique<vulkanRendererBackend::SampleTextureCube>(name, vulkanFormat, width, height);
				break;
			default LOG_ERROR("TextureCube: Unsuported TextureUsage '{}'", TextureUsageNames[usage]);
		}
	}
	TextureCube::TextureCube(const std::string& name, const TextureFormat& format, TextureUsage usage, const std::filesystem::path& path)
	{
		vulkanRendererBackend::Format vulkanFormat = vulkanRendererBackend::TextureFormatToVulkanFormat(format);
		switch (usage)
		{
		case TextureUsage::sample:
			m_pITexture = std::make_unique<vulkanRendererBackend::SampleTextureCube>(name, vulkanFormat, path);
			break;
			default LOG_ERROR("TextureCube: Unsuported TextureUsage '{}'", TextureUsageNames[usage]);
		}
	}
	TextureCube::~TextureCube()
	{

	}
}