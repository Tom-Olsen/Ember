#pragma once
#include "texture2d.h"



namespace emberCore
{
    class EMBER_CORE_API Texture2DOwner
    {
    private: // Members:
        Texture2d m_texture2d;

    public: // Methods:
        // Constructor/Destructor:
        Texture2DOwner();
        Texture2DOwner(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, emberCommon::TextureImageCountMode imageCountMode = emberCommon::TextureImageCountMode::single);
        Texture2DOwner(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const float> data, emberCommon::TextureImageCountMode imageCountMode = emberCommon::TextureImageCountMode::single);
        Texture2DOwner(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const Float2> data, emberCommon::TextureImageCountMode imageCountMode = emberCommon::TextureImageCountMode::single);
        Texture2DOwner(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const Float3> data, emberCommon::TextureImageCountMode imageCountMode = emberCommon::TextureImageCountMode::single);
        Texture2DOwner(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const Float4> data, emberCommon::TextureImageCountMode imageCountMode = emberCommon::TextureImageCountMode::single);
        Texture2DOwner(const std::string& name, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, const std::filesystem::path& path, emberCommon::TextureImageCountMode imageCountMode = emberCommon::TextureImageCountMode::single);
        ~Texture2DOwner();

        // Non-copyable:
        Texture2DOwner(const Texture2DOwner&) = delete;
        Texture2DOwner& operator=(const Texture2DOwner&) = delete;

        // Movable:
        Texture2DOwner(Texture2DOwner&& other) noexcept;
        Texture2DOwner& operator=(Texture2DOwner&& other);

        // Getters:
        Texture2d GetTexture() const;

        // Deleter:
        void Reset();
    };
}