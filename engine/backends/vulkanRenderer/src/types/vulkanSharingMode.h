#pragma once
#include <cstdint>
#include <string>



namespace vulkanRendererBackend
{
    using SharingMode = uint32_t;
    struct SharingModes
    {
        static SharingMode exclusive;
        static SharingMode concurrent;
        static SharingMode max_enum;
        static std::string ToString(SharingMode sharingMode);
    };
}