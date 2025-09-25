#pragma once
#include <variant>
#include <cstdint>
#include <string_view>



namespace emberCommon
{
    enum class TextureFormatType : uint32_t
    {
        // Fallback:
        undefined,          // undefined / unsupported format
        // Basic types:
        srgb,               // maps [0, 255] to [0.0, 1.0] in perceptually linear way, according to gamma encoding standart. 8bit only!
        uint,               // unsigned integer      € [     0,   255] (8bit example)
        sint,               //   signed integer      € [  -128,   127] (8bit example)
        uscaled,            // unsigned scaled float € [   0.0, 255.0] (8bit example)   
        sscaled,            //   signed scaled float € [-128.0, 127.0] (8bit example)   
        unorm,              // unsigned normalized   € [   0.0,   1.0]
        snorm,              //   signed normalized   € [  -1.0,   1.0]
        sfloat,             //   signed float
        // Depth/stencil types:
        depth00_stencil8,    //  0bit depth, 8bit stencil
        depth16_stencil0,    // 16bit depth, 0bit stencil
        depth16_stencil8,    // 16bit depth, 8bit stencil
        depth24_stencil8,    // 24bit depth, 8bit stencil
        depth32_stencil0,    // 32bit depth, 0bit stencil
        depth32_stencil8     // 32bit depth, 8bit stencil
    };
    inline constexpr std::string_view TextureFormatTypeNames[] =
    {
        "undefined",
        "srgb",
        "uint",
        "sint",
        "uscaled",
        "sscaled",
        "unorm",
        "snorm",
        "sfloat",
        "depth00_stencil8",
        "depth16_stencil0",
        "depth16_stencil8",
        "depth24_stencil8",
        "depth32_stencil0",
        "depth32_stencil8"
    };



    // Depth/stencil texture formats:
    struct DepthStencilFormat
    {
        TextureFormatType formatType;
        DepthStencilFormat(TextureFormatType type) { formatType = type; }
    };



    // Base Texture Format struct:
    struct TextureFormat
    {
    public: // Members:
        TextureFormatType formatType;

    public: // Methods:
        virtual ~TextureFormat() = default;
        virtual uint32_t ChannelCount() const = 0;
        virtual uint32_t BytesPerChannel() const = 0;
    };



    // Single channel texture formats:
    struct TextureFormat1Channel : public TextureFormat
    {
        inline uint32_t ChannelCount() const override { return 1; }
    };
    struct TextureFormat1Channel08Bit : public TextureFormat1Channel
    {
        TextureFormat1Channel08Bit(TextureFormatType type) { formatType = type; }
        inline uint32_t BytesPerChannel() const override { return 1; }
    };
    struct TextureFormat1Channel16Bit : public TextureFormat1Channel
    {
        TextureFormat1Channel16Bit(TextureFormatType type) { formatType = type; }
        inline uint32_t BytesPerChannel() const override { return 2; }
    };
    struct TextureFormat1Channel32Bit : public TextureFormat1Channel
    {
        TextureFormat1Channel32Bit(TextureFormatType type) { formatType = type; }
        inline uint32_t BytesPerChannel() const override { return 4; }
    };
    struct TextureFormat1Channel64Bit : public TextureFormat1Channel
    {
        TextureFormat1Channel64Bit(TextureFormatType type) { formatType = type; }
        inline uint32_t BytesPerChannel() const override { return 8; }
    };



    // Dual channel texture formats:
    struct TextureFormat2Channel : public TextureFormat
    {
        inline uint32_t ChannelCount() const override { return 2; }
    };
    struct TextureFormat2Channel08Bit : public TextureFormat2Channel
    {
        TextureFormat2Channel08Bit(TextureFormatType type) { formatType = type; }
        inline uint32_t BytesPerChannel() const override { return 1; }
    };
    struct TextureFormat2Channel16Bit : public TextureFormat2Channel
    {
        TextureFormat2Channel16Bit(TextureFormatType type) { formatType = type; }
        inline uint32_t BytesPerChannel() const override { return 2; }
    };
    struct TextureFormat2Channel32Bit : public TextureFormat2Channel
    {
        TextureFormat2Channel32Bit(TextureFormatType type) { formatType = type; }
        inline uint32_t BytesPerChannel() const override { return 4; }
    };
    struct TextureFormat2Channel64Bit : public TextureFormat2Channel
    {
        TextureFormat2Channel64Bit(TextureFormatType type) { formatType = type; }
        inline uint32_t BytesPerChannel() const override { return 8; }
    };



    // Triple channel texture formats:
    struct TextureFormat3Channel : public TextureFormat
    {
        inline uint32_t ChannelCount() const override { return 3; }
    };
    struct TextureFormat3Channel08Bit : public TextureFormat3Channel
    {
        TextureFormat3Channel08Bit(TextureFormatType type) { formatType = type; }
        inline uint32_t BytesPerChannel() const override { return 1; }
    };
    struct TextureFormat3Channel16Bit : public TextureFormat3Channel
    {
        TextureFormat3Channel16Bit(TextureFormatType type) { formatType = type; }
        inline uint32_t BytesPerChannel() const override { return 2; }
    };
    struct TextureFormat3Channel32Bit : public TextureFormat3Channel
    {
        TextureFormat3Channel32Bit(TextureFormatType type) { formatType = type; }
        inline uint32_t BytesPerChannel() const override { return 4; }
    };
    struct TextureFormat3Channel64Bit : public TextureFormat3Channel
    {
        TextureFormat3Channel64Bit(TextureFormatType type) { formatType = type; }
        inline uint32_t BytesPerChannel() const override { return 8; }
    };



    // Quadrupel channel texture formats:
    struct TextureFormat4Channel : public TextureFormat
    {
        inline uint32_t ChannelCount() const override { return 4; }
    };
    struct TextureFormat4Channel08Bit : public TextureFormat4Channel
    {
        TextureFormat4Channel08Bit(TextureFormatType type) { formatType = type; }
        inline uint32_t BytesPerChannel() const override { return 1; }
    };
    struct TextureFormat4Channel16Bit : public TextureFormat4Channel
    {
        TextureFormat4Channel16Bit(TextureFormatType type) { formatType = type; }
        inline uint32_t BytesPerChannel() const override { return 2; }
    };
    struct TextureFormat4Channel32Bit : public TextureFormat4Channel
    {
        TextureFormat4Channel32Bit(TextureFormatType type) { formatType = type; }
        inline uint32_t BytesPerChannel() const override { return 4; }
    };
    struct TextureFormat4Channel64Bit : public TextureFormat4Channel
    {
        TextureFormat4Channel64Bit(TextureFormatType type) { formatType = type; }
        inline uint32_t BytesPerChannel() const override { return 8; }
    };



    // Static instances of all supported formats:
    struct DepthStencilFormats
    {
        inline static DepthStencilFormat undefined { TextureFormatType::undefined };
        inline static DepthStencilFormat d00_s8    { TextureFormatType::depth00_stencil8 };
        inline static DepthStencilFormat d16_s0    { TextureFormatType::depth16_stencil0 };
        inline static DepthStencilFormat d16_s8    { TextureFormatType::depth16_stencil8 };
        inline static DepthStencilFormat d24_s8    { TextureFormatType::depth24_stencil8 };
        inline static DepthStencilFormat d32_s0    { TextureFormatType::depth32_stencil0 };
        inline static DepthStencilFormat d32_s8    { TextureFormatType::depth32_stencil8 };
    };
    struct TextureFormats
    {
        // Fallback:
        inline static TextureFormat1Channel08Bit undefined  { TextureFormatType::undefined };
         
        // Single Channel:
        inline static TextureFormat1Channel08Bit r08_srgb    { TextureFormatType::srgb    };
        inline static TextureFormat1Channel08Bit r08_uint    { TextureFormatType::uint    };
        inline static TextureFormat1Channel08Bit r08_sint    { TextureFormatType::sint    };
        inline static TextureFormat1Channel08Bit r08_uscaled { TextureFormatType::uscaled };
        inline static TextureFormat1Channel08Bit r08_sscaled { TextureFormatType::sscaled };
        inline static TextureFormat1Channel08Bit r08_unorm   { TextureFormatType::unorm   };
        inline static TextureFormat1Channel08Bit r08_snorm   { TextureFormatType::snorm   };
                                                             
        inline static TextureFormat1Channel16Bit r16_uint    { TextureFormatType::uint    };
        inline static TextureFormat1Channel16Bit r16_sint    { TextureFormatType::sint    };
        inline static TextureFormat1Channel16Bit r16_uscaled { TextureFormatType::uscaled };
        inline static TextureFormat1Channel16Bit r16_sscaled { TextureFormatType::sscaled };
        inline static TextureFormat1Channel16Bit r16_unorm   { TextureFormatType::unorm   };
        inline static TextureFormat1Channel16Bit r16_snorm   { TextureFormatType::snorm   };
        inline static TextureFormat1Channel16Bit r16_sfloat  { TextureFormatType::sfloat  };
                                                             
        inline static TextureFormat1Channel32Bit r32_uint    { TextureFormatType::uint    };
        inline static TextureFormat1Channel32Bit r32_sint    { TextureFormatType::sint    };
        inline static TextureFormat1Channel32Bit r32_sfloat  { TextureFormatType::sfloat  };
                                                             
        inline static TextureFormat1Channel64Bit r64_uint    { TextureFormatType::uint    };
        inline static TextureFormat1Channel64Bit r64_sint    { TextureFormatType::sint    };
        inline static TextureFormat1Channel64Bit r64_sfloat  { TextureFormatType::sfloat  };

        // Dual Channel:
        inline static TextureFormat2Channel08Bit rg08_srgb    { TextureFormatType::srgb    };
        inline static TextureFormat2Channel08Bit rg08_uint    { TextureFormatType::uint    };
        inline static TextureFormat2Channel08Bit rg08_sint    { TextureFormatType::sint    };
        inline static TextureFormat2Channel08Bit rg08_uscaled { TextureFormatType::uscaled };
        inline static TextureFormat2Channel08Bit rg08_sscaled { TextureFormatType::sscaled };
        inline static TextureFormat2Channel08Bit rg08_unorm   { TextureFormatType::unorm   };
        inline static TextureFormat2Channel08Bit rg08_snorm   { TextureFormatType::snorm   };

        inline static TextureFormat2Channel16Bit rg16_uint    { TextureFormatType::uint    };
        inline static TextureFormat2Channel16Bit rg16_sint    { TextureFormatType::sint    };
        inline static TextureFormat2Channel16Bit rg16_uscaled { TextureFormatType::uscaled };
        inline static TextureFormat2Channel16Bit rg16_sscaled { TextureFormatType::sscaled };
        inline static TextureFormat2Channel16Bit rg16_unorm   { TextureFormatType::unorm   };
        inline static TextureFormat2Channel16Bit rg16_snorm   { TextureFormatType::snorm   };
        inline static TextureFormat2Channel16Bit rg16_sfloat  { TextureFormatType::sfloat  };

        inline static TextureFormat2Channel32Bit rg32_uint    { TextureFormatType::uint    };
        inline static TextureFormat2Channel32Bit rg32_sint    { TextureFormatType::sint    };
        inline static TextureFormat2Channel32Bit rg32_sfloat  { TextureFormatType::sfloat  };

        inline static TextureFormat2Channel64Bit rg64_uint    { TextureFormatType::uint    };
        inline static TextureFormat2Channel64Bit rg64_sint    { TextureFormatType::sint    };
        inline static TextureFormat2Channel64Bit rg64_sfloat  { TextureFormatType::sfloat  };

        // Triple Channel:
        inline static TextureFormat3Channel08Bit rgb08_srgb    { TextureFormatType::srgb    };
        inline static TextureFormat3Channel08Bit rgb08_uint    { TextureFormatType::uint    };
        inline static TextureFormat3Channel08Bit rgb08_sint    { TextureFormatType::sint    };
        inline static TextureFormat3Channel08Bit rgb08_uscaled { TextureFormatType::uscaled };
        inline static TextureFormat3Channel08Bit rgb08_sscaled { TextureFormatType::sscaled };
        inline static TextureFormat3Channel08Bit rgb08_unorm   { TextureFormatType::unorm   };
        inline static TextureFormat3Channel08Bit rgb08_snorm   { TextureFormatType::snorm   };

        inline static TextureFormat3Channel16Bit rgb16_uint    { TextureFormatType::uint    };
        inline static TextureFormat3Channel16Bit rgb16_sint    { TextureFormatType::sint    };
        inline static TextureFormat3Channel16Bit rgb16_uscaled { TextureFormatType::uscaled };
        inline static TextureFormat3Channel16Bit rgb16_sscaled { TextureFormatType::sscaled };
        inline static TextureFormat3Channel16Bit rgb16_unorm   { TextureFormatType::unorm   };
        inline static TextureFormat3Channel16Bit rgb16_snorm   { TextureFormatType::snorm   };
        inline static TextureFormat3Channel16Bit rgb16_sfloat  { TextureFormatType::sfloat  };

        inline static TextureFormat3Channel32Bit rgb32_uint    { TextureFormatType::uint    };
        inline static TextureFormat3Channel32Bit rgb32_sint    { TextureFormatType::sint    };
        inline static TextureFormat3Channel32Bit rgb32_sfloat  { TextureFormatType::sfloat  };

        inline static TextureFormat3Channel64Bit rgb64_uint    { TextureFormatType::uint    };
        inline static TextureFormat3Channel64Bit rgb64_sint    { TextureFormatType::sint    };
        inline static TextureFormat3Channel64Bit rgb64_sfloat  { TextureFormatType::sfloat  };

        // Quadrupal Channel:
        inline static TextureFormat4Channel08Bit rgba08_srgb    { TextureFormatType::srgb    };
        inline static TextureFormat4Channel08Bit rgba08_uint    { TextureFormatType::uint    };
        inline static TextureFormat4Channel08Bit rgba08_sint    { TextureFormatType::sint    };
        inline static TextureFormat4Channel08Bit rgba08_uscaled { TextureFormatType::uscaled };
        inline static TextureFormat4Channel08Bit rgba08_sscaled { TextureFormatType::sscaled };
        inline static TextureFormat4Channel08Bit rgba08_unorm   { TextureFormatType::unorm   };
        inline static TextureFormat4Channel08Bit rgba08_snorm   { TextureFormatType::snorm   };

        inline static TextureFormat4Channel16Bit rgba16_uint    { TextureFormatType::uint    };
        inline static TextureFormat4Channel16Bit rgba16_sint    { TextureFormatType::sint    };
        inline static TextureFormat4Channel16Bit rgba16_uscaled { TextureFormatType::uscaled };
        inline static TextureFormat4Channel16Bit rgba16_sscaled { TextureFormatType::sscaled };
        inline static TextureFormat4Channel16Bit rgba16_unorm   { TextureFormatType::unorm   };
        inline static TextureFormat4Channel16Bit rgba16_snorm   { TextureFormatType::snorm   };
        inline static TextureFormat4Channel16Bit rgba16_sfloat  { TextureFormatType::sfloat  };

        inline static TextureFormat4Channel32Bit rgba32_uint    { TextureFormatType::uint    };
        inline static TextureFormat4Channel32Bit rgba32_sint    { TextureFormatType::sint    };
        inline static TextureFormat4Channel32Bit rgba32_sfloat  { TextureFormatType::sfloat  };

        inline static TextureFormat4Channel64Bit rgba64_uint    { TextureFormatType::uint    };
        inline static TextureFormat4Channel64Bit rgba64_sint    { TextureFormatType::sint    };
        inline static TextureFormat4Channel64Bit rgba64_sfloat  { TextureFormatType::sfloat  };
    };
}