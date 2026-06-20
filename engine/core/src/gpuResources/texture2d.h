#pragma once
#include "texture.h"
#include "commonTextureFormat.h"
#include "commonTextureUsage.h"
#include "emberCoreExport.h"
#include <filesystem>
#include <span>
#include <string>



namespace emberEngine
{
	class EMBER_CORE_API Texture2d : public Texture
	{
	public: // methods:
		// Constructor/Destructor:
		Texture2d(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage);
		Texture2d(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const float> data);
		Texture2d(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const Float2> data);
		Texture2d(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const Float3> data);
		Texture2d(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const Float4> data);
		Texture2d(const std::string& name, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, const std::filesystem::path& path);
		Texture2d(emberBackendInterface::ITexture* pITexture, bool ownsTexture);
		~Texture2d();

		// Non-copyable:
		Texture2d(const Texture2d&) = delete;
		Texture2d& operator=(const Texture2d&) = delete;

		// Movable:
		Texture2d(Texture2d&& other) noexcept = default;
		Texture2d& operator=(Texture2d&& other) noexcept = default;
	};
}