#include "texture.h"
#include "commonTextureFormat.h"
#include "commonTextureUsage.h"
#include "emberCoreExport.h"
#include <filesystem>
#include <span>
#include <string>



namespace emberCore
{
	class EMBER_CORE_API TextureCube : public Texture
	{
	public: // methods:
		// Constructor/Destructor:
		TextureCube(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage);
		TextureCube(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const float> data);
		TextureCube(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const Float2> data);
		TextureCube(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const Float3> data);
		TextureCube(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const Float4> data);
		TextureCube(const std::string& name, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, const std::filesystem::path& path);
		~TextureCube();

		// Non-copyable:
		TextureCube(const TextureCube&) = delete;
		TextureCube& operator=(const TextureCube&) = delete;

		// Movable:
		TextureCube(TextureCube&& other) noexcept = default;
		TextureCube& operator=(TextureCube&& other) noexcept = default;

	protected: // Methods:
		uint64_t GetExpectedTexelCount() const override;
	};
}