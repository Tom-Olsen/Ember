#pragma once
#include <string_view>



namespace emberCommon
{
    enum class TextureUsage : uint32_t
    {
        sample,
        storage,
		storageSample,
        renderTarget
    };
	inline constexpr std::string_view TextureUsageNames[] =
	{
		"sample",
		"storage",
		"storageSample",
		"renderTarget"
	};
}