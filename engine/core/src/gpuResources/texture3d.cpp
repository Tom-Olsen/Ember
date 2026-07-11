#include "texture3d.h"
#include "iTexture.h"
#include "logger.h"
#include "renderer.h"



namespace emberCore
{
	// Public methods:
	// Constructor/Destructor:
	Texture3d::Texture3d() : Texture()
	{

	}
	Texture3d::Texture3d(const std::string& name, int width, int height, int depth, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage)
	{
		m_ownsITexture = true;
        m_name = name;
		m_pITexture = Renderer::CreateTexture3d(width, height, depth, format, usage, nullptr);
        m_pITexture->SetDebugName(m_name);
	}
	Texture3d::Texture3d(const std::string& name, int width, int height, int depth, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const float> data) : Texture3d(name, width, height, depth, format, usage)
	{
		if (usage == emberCommon::TextureUsage::renderTarget)
			LOG_WARN("Texture3d: TextureUsage = 'renderTarget' does not support loading from float data. Ignoring data.");
		else
			SetData(data);
	}
	Texture3d::Texture3d(const std::string& name, int width, int height, int depth, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const Float2> data) : Texture3d(name, width, height, depth, format, usage)
	{
		if (usage == emberCommon::TextureUsage::renderTarget)
			LOG_WARN("Texture3d: TextureUsage = 'renderTarget' does not support loading from Float2 data. Ignoring data.");
		else
			SetData(data);
	}
	Texture3d::Texture3d(const std::string& name, int width, int height, int depth, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const Float3> data) : Texture3d(name, width, height, depth, format, usage)
	{
		if (usage == emberCommon::TextureUsage::renderTarget)
			LOG_WARN("Texture3d: TextureUsage = 'renderTarget' does not support loading from Float3 data. Ignoring data.");
		else
			SetData(data);
	}
	Texture3d::Texture3d(const std::string& name, int width, int height, int depth, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const Float4> data) : Texture3d(name, width, height, depth, format, usage)
	{
		if (usage == emberCommon::TextureUsage::renderTarget)
			LOG_WARN("Texture3d: TextureUsage = 'renderTarget' does not support loading from Float4 data. Ignoring data.");
		else
			SetData(data);
	}
	Texture3d::Texture3d(emberBackendInterface::ITexture* pITexture, bool ownsTexture)
	{
		m_ownsITexture = ownsTexture;
		m_pITexture = pITexture;
	}
	Texture3d::~Texture3d()
	{

	}
}
