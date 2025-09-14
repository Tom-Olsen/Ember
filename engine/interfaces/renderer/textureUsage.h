#pragma once
#include <string_view>



namespace emberEngine
{
    enum class TextureUsage : uint32_t
    {
        sample,
        storage,
		storageAndSample,
        renderTarget,
        cubeMap
    };
	constexpr std::string_view TextureUsageNames[] =
	{
		"sample",
		"storage",
		"storageAndSample",
		"renderTarget",
		"cubeMap"
	};
}