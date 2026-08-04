#pragma once
#include <cstdint>
#include <string_view>



namespace emberCommon
{
	enum class MaterialType : uint32_t
	{
		forward,
		gizmo,
		outline,
		shadow,
		present,
		count
	};
	inline constexpr std::string_view MaterialTypeToString(MaterialType materialType)
	{
		switch (materialType)
		{
			case MaterialType::forward: return "forward";
			case MaterialType::gizmo: return "gizmo";
			case MaterialType::outline: return "outline";
			case MaterialType::shadow: return "shadow";
			case MaterialType::present: return "present";
			case MaterialType::count: return "count";
			default: return "unknown";
		}
	}
}