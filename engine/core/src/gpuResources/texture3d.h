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
	public: // methods:
		// Constructor/Destructor:
		Texture3d();
		Texture3d(const std::string& name, int width, int height, int depth, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage);
		Texture3d(const std::string& name, int width, int height, int depth, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const float> data);
		Texture3d(const std::string& name, int width, int height, int depth, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const Float2> data);
		Texture3d(const std::string& name, int width, int height, int depth, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const Float3> data);
		Texture3d(const std::string& name, int width, int height, int depth, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const Float4> data);
		// Needs a assetLoader 3d file format implementation.
        //Texture3d(const std::string& name, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, const std::filesystem::path& path);
		Texture3d(emberBackendInterface::ITexture* pITexture, bool ownsTexture);
		~Texture3d();

		// Non-copyable:
		Texture3d(const Texture3d&) = delete;
		Texture3d& operator=(const Texture3d&) = delete;

		// Movable:
		Texture3d(Texture3d&& other) noexcept = default;
		Texture3d& operator=(Texture3d&& other) noexcept = default;
	};
}
