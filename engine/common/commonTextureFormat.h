#pragma once
#include <cstdint>
#include <sstream>
#include <string>
#include <string_view>



namespace emberCommon
{
    enum class FormatFlag : uint32_t
    {
        undefined,
        d00_s8,
        d16_s0,
        d16_s8,
        d24_s8,
        d32_s0,
        d32_s8,
        r08_srgb,
        r08_uint,
        r08_sint,
        r08_uscaled,
        r08_sscaled,
        r08_unorm,
        r08_snorm,
        r16_uint,
        r16_sint,
        r16_uscaled,
        r16_sscaled,
        r16_unorm,
        r16_snorm,
        r16_sfloat,
        r32_uint,
        r32_sint,
        r32_sfloat,
        r64_uint,
        r64_sint,
        r64_sfloat,
        rg08_srgb,
        rg08_uint,
        rg08_sint,
        rg08_uscaled,
        rg08_sscaled,
        rg08_unorm,
        rg08_snorm,
        rg16_uint,
        rg16_sint,
        rg16_uscaled,
        rg16_sscaled,
        rg16_unorm,
        rg16_snorm,
        rg16_sfloat,
        rg32_uint,
        rg32_sint,
        rg32_sfloat,
        rg64_uint,
        rg64_sint,
        rg64_sfloat,
        rgb08_srgb,
        rgb08_uint,
        rgb08_sint,
        rgb08_uscaled,
        rgb08_sscaled,
        rgb08_unorm,
        rgb08_snorm,
        rgb16_uint,
        rgb16_sint,
        rgb16_uscaled,
        rgb16_sscaled,
        rgb16_unorm,
        rgb16_snorm,
        rgb16_sfloat,
        rgb32_uint,
        rgb32_sint,
        rgb32_sfloat,
        rgb64_uint,
        rgb64_sint,
        rgb64_sfloat,
        rgba08_srgb,
        rgba08_uint,
        rgba08_sint,
        rgba08_uscaled,
        rgba08_sscaled,
        rgba08_unorm,
        rgba08_snorm,
        rgba16_uint,
        rgba16_sint,
        rgba16_uscaled,
        rgba16_sscaled,
        rgba16_unorm,
        rgba16_snorm,
        rgba16_sfloat,
        rgba32_uint,
        rgba32_sint,
        rgba32_sfloat,
        rgba64_uint,
        rgba64_sint,
        rgba64_sfloat
    };

    inline constexpr std::string_view FormatFlagToString(FormatFlag format)
    {
        switch (format)
        {
            case FormatFlag::undefined: return "undefined";
            case FormatFlag::d00_s8: return "d00_s8";
            case FormatFlag::d16_s0: return "d16_s0";
            case FormatFlag::d16_s8: return "d16_s8";
            case FormatFlag::d24_s8: return "d24_s8";
            case FormatFlag::d32_s0: return "d32_s0";
            case FormatFlag::d32_s8: return "d32_s8";
            case FormatFlag::r08_srgb: return "r08_srgb";
            case FormatFlag::r08_uint: return "r08_uint";
            case FormatFlag::r08_sint: return "r08_sint";
            case FormatFlag::r08_uscaled: return "r08_uscaled";
            case FormatFlag::r08_sscaled: return "r08_sscaled";
            case FormatFlag::r08_unorm: return "r08_unorm";
            case FormatFlag::r08_snorm: return "r08_snorm";
            case FormatFlag::r16_uint: return "r16_uint";
            case FormatFlag::r16_sint: return "r16_sint";
            case FormatFlag::r16_uscaled: return "r16_uscaled";
            case FormatFlag::r16_sscaled: return "r16_sscaled";
            case FormatFlag::r16_unorm: return "r16_unorm";
            case FormatFlag::r16_snorm: return "r16_snorm";
            case FormatFlag::r16_sfloat: return "r16_sfloat";
            case FormatFlag::r32_uint: return "r32_uint";
            case FormatFlag::r32_sint: return "r32_sint";
            case FormatFlag::r32_sfloat: return "r32_sfloat";
            case FormatFlag::r64_uint: return "r64_uint";
            case FormatFlag::r64_sint: return "r64_sint";
            case FormatFlag::r64_sfloat: return "r64_sfloat";
            case FormatFlag::rg08_srgb: return "rg08_srgb";
            case FormatFlag::rg08_uint: return "rg08_uint";
            case FormatFlag::rg08_sint: return "rg08_sint";
            case FormatFlag::rg08_uscaled: return "rg08_uscaled";
            case FormatFlag::rg08_sscaled: return "rg08_sscaled";
            case FormatFlag::rg08_unorm: return "rg08_unorm";
            case FormatFlag::rg08_snorm: return "rg08_snorm";
            case FormatFlag::rg16_uint: return "rg16_uint";
            case FormatFlag::rg16_sint: return "rg16_sint";
            case FormatFlag::rg16_uscaled: return "rg16_uscaled";
            case FormatFlag::rg16_sscaled: return "rg16_sscaled";
            case FormatFlag::rg16_unorm: return "rg16_unorm";
            case FormatFlag::rg16_snorm: return "rg16_snorm";
            case FormatFlag::rg16_sfloat: return "rg16_sfloat";
            case FormatFlag::rg32_uint: return "rg32_uint";
            case FormatFlag::rg32_sint: return "rg32_sint";
            case FormatFlag::rg32_sfloat: return "rg32_sfloat";
            case FormatFlag::rg64_uint: return "rg64_uint";
            case FormatFlag::rg64_sint: return "rg64_sint";
            case FormatFlag::rg64_sfloat: return "rg64_sfloat";
            case FormatFlag::rgb08_srgb: return "rgb08_srgb";
            case FormatFlag::rgb08_uint: return "rgb08_uint";
            case FormatFlag::rgb08_sint: return "rgb08_sint";
            case FormatFlag::rgb08_uscaled: return "rgb08_uscaled";
            case FormatFlag::rgb08_sscaled: return "rgb08_sscaled";
            case FormatFlag::rgb08_unorm: return "rgb08_unorm";
            case FormatFlag::rgb08_snorm: return "rgb08_snorm";
            case FormatFlag::rgb16_uint: return "rgb16_uint";
            case FormatFlag::rgb16_sint: return "rgb16_sint";
            case FormatFlag::rgb16_uscaled: return "rgb16_uscaled";
            case FormatFlag::rgb16_sscaled: return "rgb16_sscaled";
            case FormatFlag::rgb16_unorm: return "rgb16_unorm";
            case FormatFlag::rgb16_snorm: return "rgb16_snorm";
            case FormatFlag::rgb16_sfloat: return "rgb16_sfloat";
            case FormatFlag::rgb32_uint: return "rgb32_uint";
            case FormatFlag::rgb32_sint: return "rgb32_sint";
            case FormatFlag::rgb32_sfloat: return "rgb32_sfloat";
            case FormatFlag::rgb64_uint: return "rgb64_uint";
            case FormatFlag::rgb64_sint: return "rgb64_sint";
            case FormatFlag::rgb64_sfloat: return "rgb64_sfloat";
            case FormatFlag::rgba08_srgb: return "rgba08_srgb";
            case FormatFlag::rgba08_uint: return "rgba08_uint";
            case FormatFlag::rgba08_sint: return "rgba08_sint";
            case FormatFlag::rgba08_uscaled: return "rgba08_uscaled";
            case FormatFlag::rgba08_sscaled: return "rgba08_sscaled";
            case FormatFlag::rgba08_unorm: return "rgba08_unorm";
            case FormatFlag::rgba08_snorm: return "rgba08_snorm";
            case FormatFlag::rgba16_uint: return "rgba16_uint";
            case FormatFlag::rgba16_sint: return "rgba16_sint";
            case FormatFlag::rgba16_uscaled: return "rgba16_uscaled";
            case FormatFlag::rgba16_sscaled: return "rgba16_sscaled";
            case FormatFlag::rgba16_unorm: return "rgba16_unorm";
            case FormatFlag::rgba16_snorm: return "rgba16_snorm";
            case FormatFlag::rgba16_sfloat: return "rgba16_sfloat";
            case FormatFlag::rgba32_uint: return "rgba32_uint";
            case FormatFlag::rgba32_sint: return "rgba32_sint";
            case FormatFlag::rgba32_sfloat: return "rgba32_sfloat";
            case FormatFlag::rgba64_uint: return "rgba64_uint";
            case FormatFlag::rgba64_sint: return "rgba64_sint";
            case FormatFlag::rgba64_sfloat: return "rgba64_sfloat";
            default: return "unknown";
        }
    }


    struct TextureFormat
    {
        FormatFlag flag;
        uint32_t channels;
        uint32_t bytesPerChannel;
        inline std::string ToString() const
        {
            std::ostringstream ss;
            ss << "flag: " << FormatFlagToString(flag);
            ss << ", channels: " << channels;
            ss << ", bytesPerChannel: " << bytesPerChannel;
            return ss.str();
        }
    };
    struct TextureFormats
    {
        inline static TextureFormat r08_srgb{ FormatFlag::r08_srgb, 1, 1 };
        inline static TextureFormat r08_uint{ FormatFlag::r08_uint, 1, 1 };
        inline static TextureFormat r08_sint{ FormatFlag::r08_sint, 1, 1 };
        inline static TextureFormat r08_uscaled{ FormatFlag::r08_uscaled, 1, 1 };
        inline static TextureFormat r08_sscaled{ FormatFlag::r08_sscaled, 1, 1 };
        inline static TextureFormat r08_unorm{ FormatFlag::r08_unorm, 1, 1 };
        inline static TextureFormat r08_snorm{ FormatFlag::r08_snorm, 1, 1 };
        inline static TextureFormat r16_uint{ FormatFlag::r16_uint, 1, 2 };
        inline static TextureFormat r16_sint{ FormatFlag::r16_sint, 1, 2 };
        inline static TextureFormat r16_uscaled{ FormatFlag::r16_uscaled, 1, 2 };
        inline static TextureFormat r16_sscaled{ FormatFlag::r16_sscaled, 1, 2 };
        inline static TextureFormat r16_unorm{ FormatFlag::r16_unorm, 1, 2 };
        inline static TextureFormat r16_snorm{ FormatFlag::r16_snorm, 1, 2 };
        inline static TextureFormat r16_sfloat{ FormatFlag::r16_sfloat, 1, 2 };
        inline static TextureFormat r32_uint{ FormatFlag::r32_uint, 1, 4 };
        inline static TextureFormat r32_sint{ FormatFlag::r32_sint, 1, 4 };
        inline static TextureFormat r32_sfloat{ FormatFlag::r32_sfloat, 1, 4 };
        inline static TextureFormat r64_uint{ FormatFlag::r64_uint, 1, 8 };
        inline static TextureFormat r64_sint{ FormatFlag::r64_sint, 1, 8 };
        inline static TextureFormat r64_sfloat{ FormatFlag::r64_sfloat, 1, 8 };
        inline static TextureFormat rg08_srgb{ FormatFlag::rg08_srgb, 2, 1 };
        inline static TextureFormat rg08_uint{ FormatFlag::rg08_uint, 2, 1 };
        inline static TextureFormat rg08_sint{ FormatFlag::rg08_sint, 2, 1 };
        inline static TextureFormat rg08_uscaled{ FormatFlag::rg08_uscaled, 2, 1 };
        inline static TextureFormat rg08_sscaled{ FormatFlag::rg08_sscaled, 2, 1 };
        inline static TextureFormat rg08_unorm{ FormatFlag::rg08_unorm, 2, 1 };
        inline static TextureFormat rg08_snorm{ FormatFlag::rg08_snorm, 2, 1 };
        inline static TextureFormat rg16_uint{ FormatFlag::rg16_uint, 2, 2 };
        inline static TextureFormat rg16_sint{ FormatFlag::rg16_sint, 2, 2 };
        inline static TextureFormat rg16_uscaled{ FormatFlag::rg16_uscaled, 2, 2 };
        inline static TextureFormat rg16_sscaled{ FormatFlag::rg16_sscaled, 2, 2 };
        inline static TextureFormat rg16_unorm{ FormatFlag::rg16_unorm, 2, 2 };
        inline static TextureFormat rg16_snorm{ FormatFlag::rg16_snorm, 2, 2 };
        inline static TextureFormat rg16_sfloat{ FormatFlag::rg16_sfloat, 2, 2 };
        inline static TextureFormat rg32_uint{ FormatFlag::rg32_uint, 2, 4 };
        inline static TextureFormat rg32_sint{ FormatFlag::rg32_sint, 2, 4 };
        inline static TextureFormat rg32_sfloat{ FormatFlag::rg32_sfloat, 2, 4 };
        inline static TextureFormat rg64_uint{ FormatFlag::rg64_uint, 2, 8 };
        inline static TextureFormat rg64_sint{ FormatFlag::rg64_sint, 2, 8 };
        inline static TextureFormat rg64_sfloat{ FormatFlag::rg64_sfloat, 2, 8 };
        inline static TextureFormat rgb08_srgb{ FormatFlag::rgb08_srgb, 3, 1 };
        inline static TextureFormat rgb08_uint{ FormatFlag::rgb08_uint, 3, 1 };
        inline static TextureFormat rgb08_sint{ FormatFlag::rgb08_sint, 3, 1 };
        inline static TextureFormat rgb08_uscaled{ FormatFlag::rgb08_uscaled, 3, 1 };
        inline static TextureFormat rgb08_sscaled{ FormatFlag::rgb08_sscaled, 3, 1 };
        inline static TextureFormat rgb08_unorm{ FormatFlag::rgb08_unorm, 3, 1 };
        inline static TextureFormat rgb08_snorm{ FormatFlag::rgb08_snorm, 3, 1 };
        inline static TextureFormat rgb16_uint{ FormatFlag::rgb16_uint, 3, 2 };
        inline static TextureFormat rgb16_sint{ FormatFlag::rgb16_sint, 3, 2 };
        inline static TextureFormat rgb16_uscaled{ FormatFlag::rgb16_uscaled, 3, 2 };
        inline static TextureFormat rgb16_sscaled{ FormatFlag::rgb16_sscaled, 3, 2 };
        inline static TextureFormat rgb16_unorm{ FormatFlag::rgb16_unorm, 3, 2 };
        inline static TextureFormat rgb16_snorm{ FormatFlag::rgb16_snorm, 3, 2 };
        inline static TextureFormat rgb16_sfloat{ FormatFlag::rgb16_sfloat, 3, 2 };
        inline static TextureFormat rgb32_uint{ FormatFlag::rgb32_uint, 3, 4 };
        inline static TextureFormat rgb32_sint{ FormatFlag::rgb32_sint, 3, 4 };
        inline static TextureFormat rgb32_sfloat{ FormatFlag::rgb32_sfloat, 3, 4 };
        inline static TextureFormat rgb64_uint{ FormatFlag::rgb64_uint, 3, 8 };
        inline static TextureFormat rgb64_sint{ FormatFlag::rgb64_sint, 3, 8 };
        inline static TextureFormat rgb64_sfloat{ FormatFlag::rgb64_sfloat, 3, 8 };
        inline static TextureFormat rgba08_srgb{ FormatFlag::rgba08_srgb, 4, 1 };
        inline static TextureFormat rgba08_uint{ FormatFlag::rgba08_uint, 4, 1 };
        inline static TextureFormat rgba08_sint{ FormatFlag::rgba08_sint, 4, 1 };
        inline static TextureFormat rgba08_uscaled{ FormatFlag::rgba08_uscaled, 4, 1 };
        inline static TextureFormat rgba08_sscaled{ FormatFlag::rgba08_sscaled, 4, 1 };
        inline static TextureFormat rgba08_unorm{ FormatFlag::rgba08_unorm, 4, 1 };
        inline static TextureFormat rgba08_snorm{ FormatFlag::rgba08_snorm, 4, 1 };
        inline static TextureFormat rgba16_uint{ FormatFlag::rgba16_uint, 4, 2 };
        inline static TextureFormat rgba16_sint{ FormatFlag::rgba16_sint, 4, 2 };
        inline static TextureFormat rgba16_uscaled{ FormatFlag::rgba16_uscaled, 4, 2 };
        inline static TextureFormat rgba16_sscaled{ FormatFlag::rgba16_sscaled, 4, 2 };
        inline static TextureFormat rgba16_unorm{ FormatFlag::rgba16_unorm, 4, 2 };
        inline static TextureFormat rgba16_snorm{ FormatFlag::rgba16_snorm, 4, 2 };
        inline static TextureFormat rgba16_sfloat{ FormatFlag::rgba16_sfloat, 4, 2 };
        inline static TextureFormat rgba32_uint{ FormatFlag::rgba32_uint, 4, 4 };
        inline static TextureFormat rgba32_sint{ FormatFlag::rgba32_sint, 4, 4 };
        inline static TextureFormat rgba32_sfloat{ FormatFlag::rgba32_sfloat, 4, 4 };
        inline static TextureFormat rgba64_uint{ FormatFlag::rgba64_uint, 4, 8 };
        inline static TextureFormat rgba64_sint{ FormatFlag::rgba64_sint, 4, 8 };
        inline static TextureFormat rgba64_sfloat{ FormatFlag::rgba64_sfloat, 4, 8 };
    };
}