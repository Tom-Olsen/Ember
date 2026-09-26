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
        count,
        materialDefault     // resolve to the material's cull mode before passing it to a renderer backend.
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
			case CullMode::materialDefault: return "materialDefault";
			default: return "unknown";
		}
	}
}