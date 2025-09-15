#pragma once
#include <variant>
#include <cstdint>



namespace emberEngine
{
    // Format explaination:
    // srgb    = maps [0, 255] to [0.0, 1.0] in perceptually linear way, according to gamma encoding standart. 8bit only!
    // uint    = unsigned integer      € [     0,   255] (8bit example)
    // sint    =   signed integer      € [  -128,   127] (8bit example)
    // uscaled = unsigned scaled float € [   0.0, 255.0] (8bit example)
    // sscaled =   signed scaled float € [-128.0, 127.0] (8bit example)
    // unorm   = unsigned normalized   € [   0.0,   1.0]
    // snorm   =   signed normalized   € [  -1.0,   1.0]
    // sfloat  =   signed float

    enum class TextureFormat1Channel
    {
        // One 8bit channel:
        r08_srgb,
        r08_uint,
        r08_sint,
        r08_uscaled,
        r08_sscaled,
        r08_unorm,
        r08_snorm,
        // One 16bit channel:
        r16_uint,
        r16_sint,
        r16_uscaled,
        r16_sscaled,
        r16_unorm,
        r16_snorm,
        r16_sfloat,
        // One 32bit channel:
        r32_uint,
        r32_sint,
        r32_sfloat,
        // One 64bit channel:
        r64_uint,
        r64_sint,
        r64_sfloat
    };
    enum class TextureFormat2Channel
    {
        // Two 8bit channels:
        rg08_srgb,
        rg08_uint,
        rg08_sint,
        rg08_uscaled,
        rg08_sscaled,
        rg08_unorm,
        rg08_snorm,
        // Two 16bit channels:
        rg16_uint,
        rg16_sint,
        rg16_uscaled,
        rg16_sscaled,
        rg16_unorm,
        rg16_snorm,
        rg16_sfloat,
        // Two 32bit channels:
        rg32_uint,
        rg32_sint,
        rg32_sfloat,
        // Two 64bit channels:
        rg64_uint,
        rg64_sint,
        rg64_sfloat,
    };
    enum class TextureFormat3Channel
    {
        // Three 8bit channels:
        rgb08_srgb,
        rgb08_uint,
        rgb08_sint,
        rgb08_uscaled,
        rgb08_sscaled,
        rgb08_unorm,
        rgb08_snorm,
        // Three 16bit channels:
        rgb16_uint,
        rgb16_sint,
        rgb16_uscaled,
        rgb16_sscaled,
        rgb16_unorm,
        rgb16_snorm,
        rgb16_sfloat,
        // Three 32bit channels:
        rgb32_uint,
        rgb32_sint,
        rgb32_sfloat,
        // Three 64bit channels:
        rgb64_uint,
        rgb64_sint,
        rgb64_sfloat,
    };
    enum class TextureFormat4Channel
    {
        // Four 8bit channels:
        rgba08_srgb,
        rgba08_uint,
        rgba08_sint,
        rgba08_uscaled,
        rgba08_sscaled,
        rgba08_unorm,
        rgba08_snorm,
        // Four 16bit channels:
        rgba16_uint,
        rgba16_sint,
        rgba16_uscaled,
        rgba16_sscaled,
        rgba16_unorm,
        rgba16_snorm,
        rgba16_sfloat,
        // Four 32bit channels:
        rgba32_uint,
        rgba32_sint,
        rgba32_sfloat,
        // Four 64bit channels:
        rgba64_uint,
        rgba64_sint,
        rgba64_sfloat,
    };
    enum class DepthStencilFormat
    {
        s08_uint,
        d16_unorm,
        d16_unorm_s8_uint,
        d24_unorm_s8_uint,
        d32_sfloat,
        d32_sfloat_s8_uint
    };
    
    using GenericFormat = std::variant<TextureFormat1Channel, TextureFormat2Channel, TextureFormat3Channel, TextureFormat4Channel, DepthStencilFormat>;
}