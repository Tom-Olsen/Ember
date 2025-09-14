#pragma once



namespace emberEngine
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
}