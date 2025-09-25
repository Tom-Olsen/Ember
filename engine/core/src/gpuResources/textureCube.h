#include "texture.h"
#include "textureFormat.h"
#include "textureUsage.h"
#include <string>
#include <filesystem>



namespace emberEngine
{
	class TextureCube : public Texture
	{
	public: // methods:
		// Constructor/Destructor:
		TextureCube(const std::string& name, int width, int height, const TextureFormat& format, TextureUsage usage);
		TextureCube(const std::string& name, const TextureFormat& format, TextureUsage usage, const std::filesystem::path& path);
		~TextureCube();

		// Non-copyable:
		TextureCube(const TextureCube&) = delete;
		TextureCube& operator=(const TextureCube&) = delete;

		// Movable:
		TextureCube(TextureCube&& other) noexcept = default;
		TextureCube& operator=(TextureCube&& other) noexcept = default;
	};
}