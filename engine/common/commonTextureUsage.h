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
	inline constexpr std::string_view TextureUsageToString(TextureUsage usage)
	{
		switch (usage)
		{
			case TextureUsage::sample: return "sample";
			case TextureUsage::storage: return "storage";
			case TextureUsage::storageSample: return "storageSample";
			case TextureUsage::renderTarget: return "renderTarget";
			default: return "unknown";
		}
	}
}