#pragma once
#include "textureFormat.h"
#include <string>



namespace emberBackendInterface
{
    class ITexture2d
    {
    public: // Virtual destructor for v-table:
        virtual ~ITexture2d() = default;

    public: // Methods:
        // Getters:
        virtual const std::string& GetName() const = 0;
        virtual uint64_t GetWidth() const = 0;
        virtual uint64_t GetHeight() const = 0;
        virtual uint64_t GetChannels() const = 0;
        virtual emberEngine::TextureFormat GetFormat() const = 0;
    };
}