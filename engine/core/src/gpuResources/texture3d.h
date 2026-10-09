#pragma once
#include "texture.h"
#include "commonTextureFormat.h"
#include "commonTextureUsage.h"
#include "emberCoreExport.h"
#include <span>
#include <string>



namespace emberCore
{
	class EMBER_CORE_API Texture3d : public Texture
	{
		// Friends:
		friend class TextureManager;

	public: // methods:
		// Constructor/Destructor:
		Texture3d();
		Texture3d(const std::string& name, int width, int height, int depth, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, emberCommon::TextureImageCountMode imageCountMode = emberCommon::TextureImageCountMode::single);
		Texture3d(const std::string& name, int width, int height, int depth, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const float> data, emberCommon::TextureImageCountMode imageCountMode = emberCommon::TextureImageCountMode::single);
		Texture3d(const std::string& name, int width, int height, int depth, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const Float2> data, emberCommon::TextureImageCountMode imageCountMode = emberCommon::TextureImageCountMode::single);
		Texture3d(const std::string& name, int width, int height, int depth, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const Float3> data, emberCommon::TextureImageCountMode imageCountMode = emberCommon::TextureImageCountMode::single);
		Texture3d(const std::string& name, int width, int height, int depth, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const Float4> data, emberCommon::TextureImageCountMode imageCountMode = emberCommon::TextureImageCountMode::single);
		// Needs a assetLoader 3d file format implementation.
        //Texture3d(const std::string& name, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, const std::filesystem::path& path);
		~Texture3d();

		// Copyable:
		Texture3d(const Texture3d&) = default;
		Texture3d& operator=(const Texture3d&) = default;

		// Movable:
		Texture3d(Texture3d&& other) noexcept = default;
		Texture3d& operator=(Texture3d&& other) noexcept = default;

	private: // Methods:
		explicit Texture3d(const Texture& texture);
	};
}