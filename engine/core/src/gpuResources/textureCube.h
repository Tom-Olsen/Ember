#pragma once
#include "emberMath.h"
#include "textureFormat.h"
#include "textureUsage.h"
#include <memory>



// Forward decleration:
namespace emberBackendInterface
{
	class ITexture2d;
}



namespace emberEngine
{
	class Texture2d
	{
	private: // Members:
		std::unique_ptr<emberBackendInterface::ITexture2d> m_pITexture2d;

	public: // Constructors/Destructor:
		Texture2d(const std::string& name, int width, int height, TextureFormat format, TextureUsage usage);
		~Texture2d();

	public: // Methods:
        // Getters:
		const std::string& GetName() const
		{
			return m_pITexture2d->GetName();
		}
		uint64_t GetWidth() const
		{
			return m_pITexture2d->GetWidth();
		}
		uint64_t GetHeight() const
		{
			return m_pITexture2d->GetHeight();
		}
		uint64_t GetChannels() const
		{
			return m_pITexture2d->GetChannels();
		}
		TextureFormat GetFormat() const
		{
			return m_pITexture2d->GetFormat();
		}
	};
}