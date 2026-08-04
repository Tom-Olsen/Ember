#pragma once
#include <cstdint>
#include <string_view>



namespace emberCommon
{
	enum class GizmoRenderMode : uint32_t
	{
		opaque,
		transparent,
		wireframe,
		count
	};
	inline constexpr std::string_view GizmoRenderModeToString(GizmoRenderMode renderMode)
	{
		switch (renderMode)
		{
			case GizmoRenderMode::opaque: return "opaque";
			case GizmoRenderMode::transparent: return "transparent";
			case GizmoRenderMode::wireframe: return "wireframe";
			case GizmoRenderMode::count: return "count";
			default: return "unknown";
		}
	}
}