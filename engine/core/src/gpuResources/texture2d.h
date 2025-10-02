#pragma once
#include "texture.h"
#include "commonTextureFormat.h"
#include "commonTextureUsage.h"
#include <string>
#include <filesystem>



namespace emberEngine
{
	class Texture2d : public Texture
	{
	public: // methods:
		// Constructor/Destructor:
		Texture2d(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage);
		Texture2d(const std::string& name, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, const std::filesystem::path& path);
		~Texture2d();

		// Non-copyable:
		Texture2d(const Texture2d&) = delete;
		Texture2d& operator=(const Texture2d&) = delete;

		// Movable:
		Texture2d(Texture2d&& other) noexcept = default;
		Texture2d& operator=(Texture2d&& other) noexcept = default;
	};
}