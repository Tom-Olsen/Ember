#pragma once
#include <cstdint>
#include <string>



namespace vulkanRendererBackend
{
    using SampleCountFlag = uint32_t;
    struct SampleCountFlags
    {
        static SampleCountFlag sample_count_1_bit;
        static SampleCountFlag sample_count_2_bit;
        static SampleCountFlag sample_count_4_bit;
        static SampleCountFlag sample_count_8_bit;
        static SampleCountFlag sample_count_16_bit;
        static SampleCountFlag sample_count_32_bit;
        static SampleCountFlag sample_count_64_bit;
        static SampleCountFlag sample_count_flag_bits_max_enum;
        static std::string ToString(SampleCountFlag sampleCountFlag);
    };
}