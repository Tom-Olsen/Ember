#include "texture2d.h"
#include "iTexture.h"



namespace emberEngine
{
	// Getters:
	const std::string& GetName() const
	{
		return m_pITexture->GetName();
	}
	uint32_t GetWidth() const
	{
		return m_pITexture->GetWidth();
	}
	uint32_t GetHeight() const
	{
		return m_pITexture->GetHeight();
	}
	uint32_t GetDepth() const
	{
		return m_pITexture->GetDepth();
	}
	uint32_t GetChannels() const
	{
		return m_pITexture->GetChannels();
	}
	TextureFormat GetFormat() const
	{
		return m_pITexture->GetFormat();
	}
}