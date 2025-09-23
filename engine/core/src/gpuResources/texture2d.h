#include "texture.h"
#include "textureFormat.h"
#include "textureUsage.h"
#include <string>



namespace emberEngine
{
	class Texture2d : public Texture
	{
	public: // methods:
		// Constructor/Destructor:
		Texture2d(const std::string& name, int width, int height, const TextureFormat& format, TextureUsage usage);
		~Texture2d();

		// Non-copyable:
		Texture2d(const Texture2d&) = delete;
		Texture2d& operator=(const Texture2d&) = delete;

		// Movable:
		Texture2d(Texture2d&& other) noexcept = default;
		Texture2d& operator=(Texture2d&& other) noexcept = default;
	};
}