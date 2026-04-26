#include "texture2d.h"
#include "iTexture.h"



namespace emberEngine
{
	// Static members:
	uint32_t Texture::s_unnamedTextureCounter = 0;



	// Private Methods:
	emberBackendInterface::ITexture* Texture::GetInterfaceHandle()
	{
		return m_pITexture;
	}
	std::string Texture::CreateUnnamedTextureName()
	{
		return "#unnamed" + std::to_string(s_unnamedTextureCounter++);
	}



	// Public Methods:
	// Constructors/Destructor:
	Texture::Texture()
	{
		m_ownsITexture = false;
        m_name = CreateUnnamedTextureName();
		m_pITexture = nullptr;
	}
	Texture::Texture(emberBackendInterface::ITexture* pITexture, bool ownsTexture)
	{
		m_ownsITexture = ownsTexture;
        m_name = CreateUnnamedTextureName();
		m_pITexture = pITexture;
	}
	Texture::~Texture()
	{
		if (m_ownsITexture && m_pITexture)
			delete m_pITexture;
	}
	


	// Movable:
	Texture::Texture(Texture&& other) noexcept
	{
		m_ownsITexture = other.m_ownsITexture;
		m_name = std::move(other.m_name);
		m_pITexture = other.m_pITexture;

		other.m_ownsITexture = false;
		other.m_name = CreateUnnamedTextureName();
		other.m_pITexture = nullptr;
	}
	Texture& Texture::operator=(Texture&& other) noexcept
	{
		if (this != &other)
		{
			if (m_ownsITexture)
				delete m_pITexture;

			m_ownsITexture = other.m_ownsITexture;
			m_name = std::move(other.m_name);
			m_pITexture = other.m_pITexture;

			other.m_ownsITexture = false;
			other.m_name = CreateUnnamedTextureName();
			other.m_pITexture = nullptr;
		}
		return *this;
	}



	// Getters:
	const std::string& Texture::GetName() const
	{
		return m_name;
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