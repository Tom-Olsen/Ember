#pragma once
#include "texture3d.h"



namespace emberCore
{
    class EMBER_CORE_API Texture3DOwner
    {
    private: // Members:
        Texture3d m_texture3d;

    public: // Methods:
        // Constructor/Destructor:
        Texture3DOwner();
        Texture3DOwner(const std::string& name, int width, int height, int depth, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, emberCommon::TextureImageCountMode imageCountMode = emberCommon::TextureImageCountMode::single);
        Texture3DOwner(const std::string& name, int width, int height, int depth, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const float> data, emberCommon::TextureImageCountMode imageCountMode = emberCommon::TextureImageCountMode::single);
        Texture3DOwner(const std::string& name, int width, int height, int depth, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const Float2> data, emberCommon::TextureImageCountMode imageCountMode = emberCommon::TextureImageCountMode::single);
        Texture3DOwner(const std::string& name, int width, int height, int depth, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const Float3> data, emberCommon::TextureImageCountMode imageCountMode = emberCommon::TextureImageCountMode::single);
        Texture3DOwner(const std::string& name, int width, int height, int depth, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const Float4> data, emberCommon::TextureImageCountMode imageCountMode = emberCommon::TextureImageCountMode::single);
        ~Texture3DOwner();

        // Non-copyable:
        Texture3DOwner(const Texture3DOwner&) = delete;
        Texture3DOwner& operator=(const Texture3DOwner&) = delete;

        // Movable:
        Texture3DOwner(Texture3DOwner&& other) noexcept;
        Texture3DOwner& operator=(Texture3DOwner&& other);

        // Getters:
        Texture3d GetTexture() const;

        // Deleter:
        void Reset();
    };
}