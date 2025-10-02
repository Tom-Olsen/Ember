#pragma once
#include "commonTextureFormat.h"
#include <memory>
#include <string>



// Forward decleration:
namespace emberBackendInterface
{
	class ITexture;
}



namespace emberEngine
{
	class Texture
	{
		// Friends:
		friend class DearImGui;
		friend class ShaderProperties;

	protected: // Members:
		std::unique_ptr<emberBackendInterface::ITexture> m_pITexture;
		emberBackendInterface::ITexture* GetInterfaceHandle();

	public: // Methods:
		// Constructors/Destructor:
		Texture() = default;
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
		const emberCommon::TextureFormat& GetFormat() const;
	};
}