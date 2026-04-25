#pragma once
#include <cstdint>
#include <string_view>



namespace emberCommon
{
	enum RenderQueue : int32_t // negative values allowed.
	{
		opaque = 0,
		skybox = 1000,
		transparent = 2000
	};
	inline constexpr std::string_view RenderQueueToString(RenderQueue renderQueue)
	{
		switch (renderQueue)
		{
			case RenderQueue::opaque: return "opaque";
			case RenderQueue::skybox: return "skybox";
			case RenderQueue::transparent: return "transparent";
			default: return "unknown";
		}
	}
}