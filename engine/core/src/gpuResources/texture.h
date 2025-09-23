#pragma once
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
	protected: // Members:
		std::unique_ptr<emberBackendInterface::ITexture> m_pITexture;

	public: // Methods:
		// Constructors/Destructor:
		Texture() = default;
		virtual ~Texture();

		// Non-copyable:
		Texture(const Texture&) = delete;
		Texture& operator=(const Texture&) = delete;

		// Movable:
		Texture(Texture&& other) noexcept = default;
		Texture& operator=(Texture&& other) noexcept = default;

		// Getters:
		const std::string& GetName() const;
		uint32_t GetWidth() const;
		uint32_t GetHeight() const;
		uint32_t GetDepth() const;
		uint32_t GetChannels() const;
		const TextureFormat& GetFormat() const;
	};
}