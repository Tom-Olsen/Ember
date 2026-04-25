#pragma once
#include <cstdint>
#include <string_view>



namespace emberCommon
{
    enum class MsaaSampleCount : uint32_t
    {
        sampleCount01 = 0x00000001,
        sampleCount02 = 0x00000002,
        sampleCount04 = 0x00000004,
        sampleCount08 = 0x00000008,
        sampleCount16 = 0x00000010,
        sampleCount32 = 0x00000020,
        sampleCount64 = 0x00000040
    };
    inline constexpr std::string_view MsaaSampleCountToString(MsaaSampleCount sampleCount)
    {
        switch (sampleCount)
        {
            case MsaaSampleCount::sampleCount01: return "sampleCount01";
            case MsaaSampleCount::sampleCount02: return "sampleCount02";
            case MsaaSampleCount::sampleCount04: return "sampleCount04";
            case MsaaSampleCount::sampleCount08: return "sampleCount08";
            case MsaaSampleCount::sampleCount16: return "sampleCount16";
            case MsaaSampleCount::sampleCount32: return "sampleCount32";
            case MsaaSampleCount::sampleCount64: return "sampleCount64";
            default: return "unknown";
        }
    }
}