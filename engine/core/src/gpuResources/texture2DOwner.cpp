#include "texture2DOwner.h"
#include "textureManager.h"
#include <stdexcept>



namespace emberCore
{
    // Public methods:
    // Constructor/Destructor:
    Texture2DOwner::Texture2DOwner() = default;
    Texture2DOwner::Texture2DOwner(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, emberCommon::TextureImageCountMode imageCountMode)
        : m_texture2d(TextureManager::CreateUniqueTextureName(name), width, height, format, usage, imageCountMode)
    {

    }
    Texture2DOwner::Texture2DOwner(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const float> data, emberCommon::TextureImageCountMode imageCountMode)
        : m_texture2d(TextureManager::CreateUniqueTextureName(name), width, height, format, usage, data, imageCountMode)
    {

    }
    Texture2DOwner::Texture2DOwner(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const Float2> data, emberCommon::TextureImageCountMode imageCountMode)
        : m_texture2d(TextureManager::CreateUniqueTextureName(name), width, height, format, usage, data, imageCountMode)
    {

    }
    Texture2DOwner::Texture2DOwner(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const Float3> data, emberCommon::TextureImageCountMode imageCountMode)
        : m_texture2d(TextureManager::CreateUniqueTextureName(name), width, height, format, usage, data, imageCountMode)
    {

    }
    Texture2DOwner::Texture2DOwner(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const Float4> data, emberCommon::TextureImageCountMode imageCountMode)
        : m_texture2d(TextureManager::CreateUniqueTextureName(name), width, height, format, usage, data, imageCountMode)
    {

    }
    Texture2DOwner::Texture2DOwner(const std::string& name, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, const std::filesystem::path& path, emberCommon::TextureImageCountMode imageCountMode)
        : m_texture2d(TextureManager::CreateUniqueTextureName(name), format, usage, path, imageCountMode)
    {

    }
    Texture2DOwner::~Texture2DOwner()
    {
        Reset();
    }



    // Movable:
    Texture2DOwner::Texture2DOwner(Texture2DOwner&& other) noexcept
        : m_texture2d(other.m_texture2d)
    {
        other.m_texture2d = Texture2d();
    }
    Texture2DOwner& Texture2DOwner::operator=(Texture2DOwner&& other)
    {
        if (this != &other)
        {
            Reset();
            m_texture2d = other.m_texture2d;
            other.m_texture2d = Texture2d();
        }
        return *this;
    }



    // Getters:
    Texture2d Texture2DOwner::GetTexture() const
    {
        if (!m_texture2d.IsValid())
            throw std::runtime_error("Texture2DOwner::GetTexture() failed. Texture is invalid or expired.");
        return m_texture2d;
    }



    // Deleter:
    void Texture2DOwner::Reset()
    {
        m_texture2d.Destroy();
    }
}