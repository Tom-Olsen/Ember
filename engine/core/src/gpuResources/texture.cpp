#include "texture2d.h"
#include "iTexture.h"



namespace emberEngine
{
	// Private Methods:
	emberBackendInterface::ITexture* Texture::GetInterfaceHandle()
	{
		return m_pITexture;
	}



	// Public Methods:
	// Constructors/Destructor:
	Texture::Texture()
	{
		m_ownsTexture = false;
		m_pITexture = nullptr;
	}
	Texture::Texture(emberBackendInterface::ITexture* pITexture, bool ownsTexture)
	{
		m_ownsTexture = ownsTexture;
		m_pITexture = pITexture;
	}
	Texture::~Texture()
	{
		if (m_ownsTexture)
			delete m_pITexture;
	}
	


	// Movable:
	Texture::Texture(Texture&& other) noexcept
	{
		m_pITexture = other.m_pITexture;
		m_ownsTexture = other.m_ownsTexture;

		other.m_pITexture = nullptr;
		other.m_ownsTexture = false;
	}
	Texture& Texture::operator=(Texture&& other) noexcept
	{
		if (this != &other)
		{
			if (m_ownsTexture)
				delete m_pITexture;

			m_pITexture = other.m_pITexture;
			m_ownsTexture = other.m_ownsTexture;

			other.m_pITexture = nullptr;
			other.m_ownsTexture = false;
		}
		return *this;
	}



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



	// Setters:
	void Texture::SetData(void* data)
	{
		m_pITexture->SetData(data);
	}
}