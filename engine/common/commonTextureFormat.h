#pragma once
#include <variant>
#include <cstdint>
#include <string>
#include <string_view>



// Forward declerations:
namespace emberCommon
{
    struct TextureFormat;
    struct DepthStencilFormat;
}
namespace vulkanRendererBackend
{
    using Format = uint32_t;
    const vulkanRendererBackend::Format TextureFormatToVulkanFormat(const emberCommon::TextureFormat&);
    const vulkanRendererBackend::Format DepthStencilFormatToVulkanFormat(const emberCommon::DepthStencilFormat&);
}



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
        // Friends:
        friend struct DepthStencilFormats;
        friend const vulkanRendererBackend::Format vulkanRendererBackend::DepthStencilFormatToVulkanFormat(const emberCommon::DepthStencilFormat&);

    private: // Members:
        TextureFormatType formatType;
        uint32_t textureFormatId;

    private: //Methods:
        // Hidden and compile time only constructor:
        inline constexpr DepthStencilFormat(TextureFormatType formatType, uint32_t textureFormatId)
            : formatType(formatType), textureFormatId(textureFormatId) {}
        inline uint32_t GetId() const { return textureFormatId; }

    public: // Methods:
        inline TextureFormatType GetFormatType() const { return formatType; }
    };



    // Base Texture Format struct:
    struct TextureFormat
    {
        // Friends:
        friend struct TextureFormats;   // only TextureFormats can construct TextureFormat objects.
        friend const vulkanRendererBackend::Format vulkanRendererBackend::TextureFormatToVulkanFormat(const emberCommon::TextureFormat&);

    private: // Members:
        uint32_t channelCount;
        uint32_t bytesPerChannel;
        TextureFormatType formatType;
        uint32_t textureFormatId;

    private: //Methods:
        // Hidden and compile time only constructor:
        inline constexpr TextureFormat(uint32_t channelCount, uint32_t bytesPerChannel, TextureFormatType formatType, uint32_t textureFormatId)
            : channelCount(channelCount), bytesPerChannel(bytesPerChannel), formatType(formatType), textureFormatId(textureFormatId) {}
        inline uint32_t GetId() const { return textureFormatId; }

    public: // Methods:
        inline uint32_t GetChannelCount() const { return channelCount; }
        inline uint32_t GetBytesPerChannel() const { return bytesPerChannel; }
        inline TextureFormatType GetFormatType() const { return formatType; }
        inline std::string ToString() const
        {
            std::string bitsString;
            if (bytesPerChannel == 1) bitsString = "08";
            if (bytesPerChannel == 2) bitsString = "16";
            if (bytesPerChannel == 4) bitsString = "32";
            if (bytesPerChannel == 8) bitsString = "64";
            std::string channelString;
            if (channelCount == 1) channelString = "r";
            if (channelCount == 2) channelString = "rg";
            if (channelCount == 3) channelString = "rgb";
            if (channelCount == 4) channelString = "rgba";

            return std::string("TextureFormat_") + channelString + bitsString + std::string("_") + std::string(TextureFormatTypeNames[static_cast<int>(formatType)]);
        }
    };

    // Static instances of all supported formats:
    struct DepthStencilFormats
    {
        inline static DepthStencilFormat undefined { TextureFormatType::undefined,        0 };
        inline static DepthStencilFormat d00_s8    { TextureFormatType::depth00_stencil8, 1 };
        inline static DepthStencilFormat d16_s0    { TextureFormatType::depth16_stencil0, 2 };
        inline static DepthStencilFormat d16_s8    { TextureFormatType::depth16_stencil8, 3 };
        inline static DepthStencilFormat d24_s8    { TextureFormatType::depth24_stencil8, 4 };
        inline static DepthStencilFormat d32_s0    { TextureFormatType::depth32_stencil0, 5 };
        inline static DepthStencilFormat d32_s8    { TextureFormatType::depth32_stencil8, 6 };
    };
    struct TextureFormats
    {
        // Fallback:
        inline static TextureFormat undefined   {0, 0, TextureFormatType::undefined, 0};
         
        // Single Channel:
        inline static constexpr TextureFormat r08_srgb    { 1, 1, TextureFormatType::srgb,     0 };
        inline static constexpr TextureFormat r08_uint    { 1, 1, TextureFormatType::uint,     1 };
        inline static constexpr TextureFormat r08_sint    { 1, 1, TextureFormatType::sint,     2 };
        inline static constexpr TextureFormat r08_uscaled { 1, 1, TextureFormatType::uscaled,  3 };
        inline static constexpr TextureFormat r08_sscaled { 1, 1, TextureFormatType::sscaled,  4 };
        inline static constexpr TextureFormat r08_unorm   { 1, 1, TextureFormatType::unorm,    5 };
        inline static constexpr TextureFormat r08_snorm   { 1, 1, TextureFormatType::snorm,    6 };

        inline static constexpr TextureFormat r16_uint    { 1, 2, TextureFormatType::uint,     7 };
        inline static constexpr TextureFormat r16_sint    { 1, 2, TextureFormatType::sint,     8 };
        inline static constexpr TextureFormat r16_uscaled { 1, 2, TextureFormatType::uscaled,  9 };
        inline static constexpr TextureFormat r16_sscaled { 1, 2, TextureFormatType::sscaled, 10 };
        inline static constexpr TextureFormat r16_unorm   { 1, 2, TextureFormatType::unorm,   11 };
        inline static constexpr TextureFormat r16_snorm   { 1, 2, TextureFormatType::snorm,   12 };
        inline static constexpr TextureFormat r16_sfloat  { 1, 2, TextureFormatType::sfloat,  13 };

        inline static constexpr TextureFormat r32_uint    { 1, 4, TextureFormatType::uint,    14 };
        inline static constexpr TextureFormat r32_sint    { 1, 4, TextureFormatType::sint,    15 };
        inline static constexpr TextureFormat r32_sfloat  { 1, 4, TextureFormatType::sfloat,  16 };

        inline static constexpr TextureFormat r64_uint    { 1, 8, TextureFormatType::uint,    17 };
        inline static constexpr TextureFormat r64_sint    { 1, 8, TextureFormatType::sint,    18 };
        inline static constexpr TextureFormat r64_sfloat  { 1, 8, TextureFormatType::sfloat,  19 };

        // Dual Channel:
        inline static constexpr TextureFormat rg08_srgb    { 2, 1, TextureFormatType::srgb,    20 };
        inline static constexpr TextureFormat rg08_uint    { 2, 1, TextureFormatType::uint,    21 };
        inline static constexpr TextureFormat rg08_sint    { 2, 1, TextureFormatType::sint,    22 };
        inline static constexpr TextureFormat rg08_uscaled { 2, 1, TextureFormatType::uscaled, 23 };
        inline static constexpr TextureFormat rg08_sscaled { 2, 1, TextureFormatType::sscaled, 24 };
        inline static constexpr TextureFormat rg08_unorm   { 2, 1, TextureFormatType::unorm,   25 };
        inline static constexpr TextureFormat rg08_snorm   { 2, 1, TextureFormatType::snorm,   26 };

        inline static constexpr TextureFormat rg16_uint    { 2, 2, TextureFormatType::uint,    27 };
        inline static constexpr TextureFormat rg16_sint    { 2, 2, TextureFormatType::sint,    28 };
        inline static constexpr TextureFormat rg16_uscaled { 2, 2, TextureFormatType::uscaled, 29 };
        inline static constexpr TextureFormat rg16_sscaled { 2, 2, TextureFormatType::sscaled, 30 };
        inline static constexpr TextureFormat rg16_unorm   { 2, 2, TextureFormatType::unorm,   31 };
        inline static constexpr TextureFormat rg16_snorm   { 2, 2, TextureFormatType::snorm,   32 };
        inline static constexpr TextureFormat rg16_sfloat  { 2, 2, TextureFormatType::sfloat,  33 };

        inline static constexpr TextureFormat rg32_uint    { 2, 4, TextureFormatType::uint,    34 };
        inline static constexpr TextureFormat rg32_sint    { 2, 4, TextureFormatType::sint,    35 };
        inline static constexpr TextureFormat rg32_sfloat  { 2, 4, TextureFormatType::sfloat,  36 };

        inline static constexpr TextureFormat rg64_uint    { 2, 8, TextureFormatType::uint,    37 };
        inline static constexpr TextureFormat rg64_sint    { 2, 8, TextureFormatType::sint,    38 };
        inline static constexpr TextureFormat rg64_sfloat  { 2, 8, TextureFormatType::sfloat,  39 };

        // Triple Channel:
        inline static constexpr TextureFormat rgb08_srgb    { 3, 1, TextureFormatType::srgb,    40 };
        inline static constexpr TextureFormat rgb08_uint    { 3, 1, TextureFormatType::uint,    41 };
        inline static constexpr TextureFormat rgb08_sint    { 3, 1, TextureFormatType::sint,    42 };
        inline static constexpr TextureFormat rgb08_uscaled { 3, 1, TextureFormatType::uscaled, 43 };
        inline static constexpr TextureFormat rgb08_sscaled { 3, 1, TextureFormatType::sscaled, 44 };
        inline static constexpr TextureFormat rgb08_unorm   { 3, 1, TextureFormatType::unorm,   45 };
        inline static constexpr TextureFormat rgb08_snorm   { 3, 1, TextureFormatType::snorm,   46 };

        inline static constexpr TextureFormat rgb16_uint    { 3, 2, TextureFormatType::uint,    47 };
        inline static constexpr TextureFormat rgb16_sint    { 3, 2, TextureFormatType::sint,    48 };
        inline static constexpr TextureFormat rgb16_uscaled { 3, 2, TextureFormatType::uscaled, 49 };
        inline static constexpr TextureFormat rgb16_sscaled { 3, 2, TextureFormatType::sscaled, 50 };
        inline static constexpr TextureFormat rgb16_unorm   { 3, 2, TextureFormatType::unorm,   51 };
        inline static constexpr TextureFormat rgb16_snorm   { 3, 2, TextureFormatType::snorm,   52 };
        inline static constexpr TextureFormat rgb16_sfloat  { 3, 2, TextureFormatType::sfloat,  53 };

        inline static constexpr TextureFormat rgb32_uint    { 3, 4, TextureFormatType::uint,    54 };
        inline static constexpr TextureFormat rgb32_sint    { 3, 4, TextureFormatType::sint,    55 };
        inline static constexpr TextureFormat rgb32_sfloat  { 3, 4, TextureFormatType::sfloat,  56 };

        inline static constexpr TextureFormat rgb64_uint    { 3, 8, TextureFormatType::uint,    57 };
        inline static constexpr TextureFormat rgb64_sint    { 3, 8, TextureFormatType::sint,    58 };
        inline static constexpr TextureFormat rgb64_sfloat  { 3, 8, TextureFormatType::sfloat,  59 };

        // Quadrupal Channel:
        inline static constexpr TextureFormat rgba08_srgb    { 4, 1, TextureFormatType::srgb,    60 };
        inline static constexpr TextureFormat rgba08_uint    { 4, 1, TextureFormatType::uint,    61 };
        inline static constexpr TextureFormat rgba08_sint    { 4, 1, TextureFormatType::sint,    62 };
        inline static constexpr TextureFormat rgba08_uscaled { 4, 1, TextureFormatType::uscaled, 63 };
        inline static constexpr TextureFormat rgba08_sscaled { 4, 1, TextureFormatType::sscaled, 64 };
        inline static constexpr TextureFormat rgba08_unorm   { 4, 1, TextureFormatType::unorm,   65 };
        inline static constexpr TextureFormat rgba08_snorm   { 4, 1, TextureFormatType::snorm,   66 };

        inline static constexpr TextureFormat rgba16_uint    { 4, 2, TextureFormatType::uint,    67 };
        inline static constexpr TextureFormat rgba16_sint    { 4, 2, TextureFormatType::sint,    68 };
        inline static constexpr TextureFormat rgba16_uscaled { 4, 2, TextureFormatType::uscaled, 69 };
        inline static constexpr TextureFormat rgba16_sscaled { 4, 2, TextureFormatType::sscaled, 70 };
        inline static constexpr TextureFormat rgba16_unorm   { 4, 2, TextureFormatType::unorm,   71 };
        inline static constexpr TextureFormat rgba16_snorm   { 4, 2, TextureFormatType::snorm,   72 };
        inline static constexpr TextureFormat rgba16_sfloat  { 4, 2, TextureFormatType::sfloat,  73 };

        inline static constexpr TextureFormat rgba32_uint    { 4, 4, TextureFormatType::uint,    74 };
        inline static constexpr TextureFormat rgba32_sint    { 4, 4, TextureFormatType::sint,    75 };
        inline static constexpr TextureFormat rgba32_sfloat  { 4, 4, TextureFormatType::sfloat,  76 };

        inline static constexpr TextureFormat rgba64_uint    { 4, 8, TextureFormatType::uint,    77 };
        inline static constexpr TextureFormat rgba64_sint    { 4, 8, TextureFormatType::sint,    78 };
        inline static constexpr TextureFormat rgba64_sfloat  { 4, 8, TextureFormatType::sfloat,  79 };
    };
}