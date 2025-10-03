#pragma once
#include "commonTextureFormat.h"
#include <string>



namespace emberBackendInterface
{
    class ITexture
    {
    public: // Methods:
        // Virtual destructor for v-table:
        virtual ~ITexture() = default;

        // Getters:
        virtual const std::string& GetName() const = 0;
        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;
        virtual uint32_t GetDepth() const = 0;
        virtual uint32_t GetChannels() const = 0;
        virtual const emberCommon::TextureFormat GetTextureFormat() const = 0;
        virtual void* GetVkImageView() const = 0;

        // Setters:
        virtual void SetData(void* data) = 0;
    };
}