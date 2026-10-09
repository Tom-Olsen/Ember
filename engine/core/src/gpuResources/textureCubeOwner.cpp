#include "textureCubeOwner.h"
#include "textureManager.h"
#include <stdexcept>



namespace emberCore
{
    // Public methods:
    // Constructor/Destructor:
    TextureCubeOwner::TextureCubeOwner() = default;
    TextureCubeOwner::TextureCubeOwner(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, emberCommon::TextureImageCountMode imageCountMode)
        : m_textureCube(TextureManager::CreateUniqueTextureName(name), width, height, format, usage, imageCountMode)
    {

    }
    TextureCubeOwner::TextureCubeOwner(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const float> data, emberCommon::TextureImageCountMode imageCountMode)
        : m_textureCube(TextureManager::CreateUniqueTextureName(name), width, height, format, usage, data, imageCountMode)
    {

    }
    TextureCubeOwner::TextureCubeOwner(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const Float2> data, emberCommon::TextureImageCountMode imageCountMode)
        : m_textureCube(TextureManager::CreateUniqueTextureName(name), width, height, format, usage, data, imageCountMode)
    {

    }
    TextureCubeOwner::TextureCubeOwner(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const Float3> data, emberCommon::TextureImageCountMode imageCountMode)
        : m_textureCube(TextureManager::CreateUniqueTextureName(name), width, height, format, usage, data, imageCountMode)
    {

    }
    TextureCubeOwner::TextureCubeOwner(const std::string& name, int width, int height, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, std::span<const Float4> data, emberCommon::TextureImageCountMode imageCountMode)
        : m_textureCube(TextureManager::CreateUniqueTextureName(name), width, height, format, usage, data, imageCountMode)
    {

    }
    TextureCubeOwner::TextureCubeOwner(const std::string& name, const emberCommon::TextureFormat& format, emberCommon::TextureUsage usage, const std::filesystem::path& path, emberCommon::TextureImageCountMode imageCountMode)
        : m_textureCube(TextureManager::CreateUniqueTextureName(name), format, usage, path, imageCountMode)
    {

    }
    TextureCubeOwner::~TextureCubeOwner()
    {
        Reset();
    }



    // Movable:
    TextureCubeOwner::TextureCubeOwner(TextureCubeOwner&& other) noexcept
        : m_textureCube(other.m_textureCube)
    {
        other.m_textureCube = TextureCube();
    }
    TextureCubeOwner& TextureCubeOwner::operator=(TextureCubeOwner&& other)
    {
        if (this != &other)
        {
            Reset();
            m_textureCube = other.m_textureCube;
            other.m_textureCube = TextureCube();
        }
        return *this;
    }



    // Getters:
    TextureCube TextureCubeOwner::GetTexture() const
    {
        if (!m_textureCube.IsValid())
            throw std::runtime_error("TextureCubeOwner::GetTexture() failed. Texture is invalid or expired.");
        return m_textureCube;
    }



    // Deleter:
    void TextureCubeOwner::Reset()
    {
        m_textureCube.Destroy();
    }
}