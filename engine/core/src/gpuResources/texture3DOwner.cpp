#include "texture3DOwner.h"
#include "textureManager.h"
#include <stdexcept>



namespace emberCore
{
    // Public methods:
    // Constructor/Destructor:
    Texture3DOwner::Texture3DOwner() = default;
    Texture3DOwner::Texture3DOwner(const std::string& name, int width, int height, int depth, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, emberCommon::TextureImageCountMode imageCountMode)
        : m_texture3d(TextureManager::CreateUniqueTextureName(name), width, height, depth, format, usage, imageCountMode)
    {

    }
    Texture3DOwner::Texture3DOwner(const std::string& name, int width, int height, int depth, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const float> data, emberCommon::TextureImageCountMode imageCountMode)
        : m_texture3d(TextureManager::CreateUniqueTextureName(name), width, height, depth, format, usage, data, imageCountMode)
    {

    }
    Texture3DOwner::Texture3DOwner(const std::string& name, int width, int height, int depth, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const Float2> data, emberCommon::TextureImageCountMode imageCountMode)
        : m_texture3d(TextureManager::CreateUniqueTextureName(name), width, height, depth, format, usage, data, imageCountMode)
    {

    }
    Texture3DOwner::Texture3DOwner(const std::string& name, int width, int height, int depth, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const Float3> data, emberCommon::TextureImageCountMode imageCountMode)
        : m_texture3d(TextureManager::CreateUniqueTextureName(name), width, height, depth, format, usage, data, imageCountMode)
    {

    }
    Texture3DOwner::Texture3DOwner(const std::string& name, int width, int height, int depth, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const Float4> data, emberCommon::TextureImageCountMode imageCountMode)
        : m_texture3d(TextureManager::CreateUniqueTextureName(name), width, height, depth, format, usage, data, imageCountMode)
    {

    }
    Texture3DOwner::~Texture3DOwner()
    {
        Reset();
    }



    // Movable:
    Texture3DOwner::Texture3DOwner(Texture3DOwner&& other) noexcept
        : m_texture3d(other.m_texture3d)
    {
        other.m_texture3d = Texture3d();
    }
    Texture3DOwner& Texture3DOwner::operator=(Texture3DOwner&& other)
    {
        if (this != &other)
        {
            Reset();
            m_texture3d = other.m_texture3d;
            other.m_texture3d = Texture3d();
        }
        return *this;
    }



    // Getters:
    Texture3d Texture3DOwner::GetTexture() const
    {
        if (!m_texture3d.IsValid())
            throw std::runtime_error("Texture3DOwner::GetTexture() failed. Texture is invalid or expired.");
        return m_texture3d;
    }



    // Deleter:
    void Texture3DOwner::Reset()
    {
        m_texture3d.Destroy();
    }
}