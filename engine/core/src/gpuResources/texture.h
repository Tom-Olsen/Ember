#pragma once
#include "commonTextureFormat.h"
#include "emberCoreExport.h"
#include <memory>
#include <string>



// Forward decleration:
namespace emberBackendInterface
{
	class ITexture;
}



namespace emberEngine
{
	class EMBER_CORE_API Texture
	{
		// Friends:
		friend class Gui;
		friend class ShaderProperties;

	protected: // Members:
		bool m_ownsITexture;
		emberBackendInterface::ITexture* m_pITexture; // conditional ownership, depending on usecase.
		emberBackendInterface::ITexture* GetInterfaceHandle();

	public: // Methods:
		// Constructors/Destructor:
		Texture();
		Texture(emberBackendInterface::ITexture* pITexture, bool ownsTexture);
		virtual ~Texture();

		// Non-copyable:
		Texture(const Texture&) = delete;
		Texture& operator=(const Texture&) = delete;

		// Movable:
		Texture(Texture&& other) noexcept;
		Texture& operator=(Texture&& other) noexcept;

		// Getters:
		const std::string& GetName() const;
		uint32_t GetWidth() const;
		uint32_t GetHeight() const;
		uint32_t GetDepth() const;
		uint32_t GetChannels() const;
		const emberCommon::TextureFormat GetFormat() const;

		// Setters:
		void SetData(void* data);
	};
}