#include "texture2d.h"
#include "iTexture.h"



namespace emberEngine
{
	// Private Methods:
	emberBackendInterface::ITexture* Texture::GetInterfaceHandle()
	{
		return m_pITexture.get();
	}



	// Public Methods:
	// Virtual destructor:
	Texture::~Texture()
	{

	}
	


	// Movable:
	Texture::Texture(Texture&& other) noexcept = default;
	Texture& Texture::operator=(Texture&& other) noexcept = default;



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
	const emberCommon::TextureFormat Texture::GetFormat() const
	{
		return m_pITexture->GetTextureFormat();
	}
}