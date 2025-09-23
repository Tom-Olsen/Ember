#include "texture2d.h"
#include "iTexture.h"



namespace emberEngine
{
	// Getters:
	const std::string& Texture::GetName() const
	{
		return m_pITexture->GetName();
	}
	uint32_t Texture::GetWidth() const
	{
		return m_pITexture->GetWidth();
	}
	uint32_t Texture::GetHeight() const
	{
		return m_pITexture->GetHeight();
	}
	uint32_t Texture::GetDepth() const
	{
		return m_pITexture->GetDepth();
	}
	uint32_t Texture::GetChannels() const
	{
		return m_pITexture->GetChannels();
	}
	const TextureFormat& Texture::GetFormat() const
	{
		return m_pITexture->GetTextureFormat();
	}
}