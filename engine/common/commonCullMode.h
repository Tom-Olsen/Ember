#pragma once
#include <string_view>



namespace emberCommon
{
    enum class CullMode
    {
        none,               // no culling, render both sides.
        front,              // cull triangles front.
        back,               // cull triangles back.
        both,               // cull front and back triangles.
        count
    };
	inline constexpr std::string_view CullModeToString(CullMode cullMode)
	{
		switch (cullMode)
		{
			case CullMode::none: return "none";
			case CullMode::front: return "front";
			case CullMode::back: return "back";
			case CullMode::both: return "both";
			case CullMode::count: return "count";
			default: return "unknown";
		}
	}
}