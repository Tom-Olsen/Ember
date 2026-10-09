#pragma once
#include "textureCube.h"



namespace emberCore
{
    class EMBER_CORE_API TextureCubeOwner
    {
    private: // Members:
        TextureCube m_textureCube;

    public: // Methods:
        // Constructor/Destructor:
        TextureCubeOwner();
        TextureCubeOwner(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, emberCommon::TextureImageCountMode imageCountMode = emberCommon::TextureImageCountMode::single);
        TextureCubeOwner(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const float> data, emberCommon::TextureImageCountMode imageCountMode = emberCommon::TextureImageCountMode::single);
        TextureCubeOwner(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const Float2> data, emberCommon::TextureImageCountMode imageCountMode = emberCommon::TextureImageCountMode::single);
        TextureCubeOwner(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const Float3> data, emberCommon::TextureImageCountMode imageCountMode = emberCommon::TextureImageCountMode::single);
        TextureCubeOwner(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const Float4> data, emberCommon::TextureImageCountMode imageCountMode = emberCommon::TextureImageCountMode::single);
        TextureCubeOwner(const std::string& name, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, const std::filesystem::path& path, emberCommon::TextureImageCountMode imageCountMode = emberCommon::TextureImageCountMode::single);
        ~TextureCubeOwner();

        // Non-copyable:
        TextureCubeOwner(const TextureCubeOwner&) = delete;
        TextureCubeOwner& operator=(const TextureCubeOwner&) = delete;

        // Movable:
        TextureCubeOwner(TextureCubeOwner&& other) noexcept;
        TextureCubeOwner& operator=(TextureCubeOwner&& other);

        // Getters:
        TextureCube GetTexture() const;

        // Deleter:
        void Reset();
    };
}