#pragma once
#include <string_view>



namespace emberEngine
{
	enum class TextureFormat : uint32_t
	{
        // Color:
        rgba08_UNorm,
        rgba08_SRGB,
        rgba16_Float,
        rgba32_Float,

        // Depth/Stencil:
        d24_UNorm_S8_UInt,
        d32_Float,
        d32_Float_S8_UInt,

        // Single channel:
        r08_UNorm,
        r08_UInt,
        r16_Float,
        r32_Float,

        // Fallback:
        unknown = 0xFFFFFFFF
	};
	constexpr std::string_view TextureFormatNames[] =
	{
        // Color:
        "rgba08_UNorm",
        "rgba08_SRGB",
        "rgba16_Float",
        "rgba32_Float",

        // Depth/Stencil:
        "d24_UNorm_S8_UInt",
        "d32_Float",
        "d32_Float_S8_UInt",

        // Single channel:
        "r08_UNorm",
        "r08_UInt",
        "r16_Float",
        "r32_Float",

        // Fallback:
        "unknown"
	};
}