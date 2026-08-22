#include "vulkanTextureHandle.h"
#include "logger.h"
#include "vulkanGpuResourceRegistry.h"
#include "vulkanTexture.h"
#include <stdexcept>



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructor/Destructor:
	TextureHandle::TextureHandle()
	{

	}
	TextureHandle::TextureHandle(const Texture& texture)
		: m_resourceHandle(texture.m_registrationHandle)
	{
		if (!m_resourceHandle.IsValid())
			throw std::runtime_error("TextureHandle::TextureHandle(...) failed. Texture has been moved from.");
	}
	TextureHandle::~TextureHandle()
	{

	}



	// Getters:
	Texture* TextureHandle::Get() const
	{
		Texture* pTexture = Texture::s_resourceRegistry.TryGet(m_resourceHandle);
		if (pTexture == nullptr)
			throw std::runtime_error("TextureHandle::Get() failed. Texture is invalid or expired.");
		return pTexture;
	}
	Texture* TextureHandle::TryGet() const
	{
		Texture* pTexture = Texture::s_resourceRegistry.TryGet(m_resourceHandle);
		if (pTexture == nullptr)
			LOG_WARN("TextureHandle::TryGet() failed. Texture is invalid or expired.");
		return pTexture;
	}
	bool TextureHandle::IsValid() const
	{
		return Texture::s_resourceRegistry.TryGet(m_resourceHandle) != nullptr;
	}



	// Comparison operators:
	bool TextureHandle::operator==(const TextureHandle& other) const
	{
		return m_resourceHandle == other.m_resourceHandle;
	}
	bool TextureHandle::operator!=(const TextureHandle& other) const
	{
		return !(*this == other);
	}
}