#pragma once
#include "texture.h"
#include "commonTextureFormat.h"
#include "commonTextureUsage.h"
#include "emberCoreExport.h"
#include <filesystem>
#include <span>
#include <string>



namespace emberCore
{
	class EMBER_CORE_API Texture2d : public Texture
	{
		// Friends:
		friend class TextureManager;
		
	public: // methods:
		// Constructor/Destructor:
		Texture2d();    // needed for array construction.
		Texture2d(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, emberCommon::TextureImageCountMode imageCountMode = emberCommon::TextureImageCountMode::single);
		Texture2d(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const float> data, emberCommon::TextureImageCountMode imageCountMode = emberCommon::TextureImageCountMode::single);
		Texture2d(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const Float2> data, emberCommon::TextureImageCountMode imageCountMode = emberCommon::TextureImageCountMode::single);
		Texture2d(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const Float3> data, emberCommon::TextureImageCountMode imageCountMode = emberCommon::TextureImageCountMode::single);
		Texture2d(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const Float4> data, emberCommon::TextureImageCountMode imageCountMode = emberCommon::TextureImageCountMode::single);
		Texture2d(const std::string& name, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, const std::filesystem::path& path, emberCommon::TextureImageCountMode imageCountMode = emberCommon::TextureImageCountMode::single);
		~Texture2d();

		// Copyable:
		Texture2d(const Texture2d&) = default;
		Texture2d& operator=(const Texture2d&) = default;

		// Movable:
		Texture2d(Texture2d&& other) noexcept = default;
		Texture2d& operator=(Texture2d&& other) noexcept = default;

	private: // Methods:
		explicit Texture2d(const Texture& texture);
	};
}