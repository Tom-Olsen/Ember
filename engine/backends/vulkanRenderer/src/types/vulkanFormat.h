#pragma once
#include <cstdint>
#include <string>



namespace vulkanRendererBackend
{
    using Format = uint32_t;
    struct Formats
    {
        static Format undefined;
        static Format r8_unorm;
        static Format r8_snorm;
        static Format r8_uscaled;
        static Format r8_sscaled;
        static Format r8_uint;
        static Format r8_sint;
        static Format r8_srgb;
        static Format r8g8_unorm;
        static Format r8g8_snorm;
        static Format r8g8_uscaled;
        static Format r8g8_sscaled;
        static Format r8g8_uint;
        static Format r8g8_sint;
        static Format r8g8_srgb;
        static Format r8g8b8_unorm;
        static Format r8g8b8_snorm;
        static Format r8g8b8_uscaled;
        static Format r8g8b8_sscaled;
        static Format r8g8b8_uint;
        static Format r8g8b8_sint;
        static Format r8g8b8_srgb;
        static Format r8g8b8a8_unorm;
        static Format r8g8b8a8_snorm;
        static Format r8g8b8a8_uscaled;
        static Format r8g8b8a8_sscaled;
        static Format r8g8b8a8_uint;
        static Format r8g8b8a8_sint;
        static Format r8g8b8a8_srgb;
        static Format r16_unorm;
        static Format r16_snorm;
        static Format r16_uscaled;
        static Format r16_sscaled;
        static Format r16_uint;
        static Format r16_sint;
        static Format r16_sfloat;
        static Format r16g16_unorm;
        static Format r16g16_snorm;
        static Format r16g16_uscaled;
        static Format r16g16_sscaled;
        static Format r16g16_uint;
        static Format r16g16_sint;
        static Format r16g16_sfloat;
        static Format r16g16b16_unorm;
        static Format r16g16b16_snorm;
        static Format r16g16b16_uscaled;
        static Format r16g16b16_sscaled;
        static Format r16g16b16_uint;
        static Format r16g16b16_sint;
        static Format r16g16b16_sfloat;
        static Format r16g16b16a16_unorm;
        static Format r16g16b16a16_snorm;
        static Format r16g16b16a16_uscaled;
        static Format r16g16b16a16_sscaled;
        static Format r16g16b16a16_uint;
        static Format r16g16b16a16_sint;
        static Format r16g16b16a16_sfloat;
        static Format r32_uint;
        static Format r32_sint;
        static Format r32_sfloat;
        static Format r32g32_uint;
        static Format r32g32_sint;
        static Format r32g32_sfloat;
        static Format r32g32b32_uint;
        static Format r32g32b32_sint;
        static Format r32g32b32_sfloat;
        static Format r32g32b32a32_uint;
        static Format r32g32b32a32_sint;
        static Format r32g32b32a32_sfloat;
        static Format r64_uint;
        static Format r64_sint;
        static Format r64_sfloat;
        static Format r64g64_uint;
        static Format r64g64_sint;
        static Format r64g64_sfloat;
        static Format r64g64b64_uint;
        static Format r64g64b64_sint;
        static Format r64g64b64_sfloat;
        static Format r64g64b64a64_uint;
        static Format r64g64b64a64_sint;
        static Format r64g64b64a64_sfloat;
        static Format d16_unorm;
        static Format d32_sfloat;
        static Format s8_uint;
        static Format d16_unorm_s8_uint;
        static Format d24_unorm_s8_uint;
        static Format d32_sfloat_s8_uint;
        static Format max_enum;
        static std::string ToString(Format format);
    };
}