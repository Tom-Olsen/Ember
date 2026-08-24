#pragma once
#include <cstdint>
#include <string_view>



namespace emberCommon
{
	enum class MaterialPass : uint32_t
	{
		forward,
		gizmo,
		outline,
		shadow,
		present,
		count
	};
	inline constexpr std::string_view MaterialPassToString(MaterialPass materialPass)
	{
		switch (materialPass)
		{
			case MaterialPass::forward: return "forward";
			case MaterialPass::gizmo: return "gizmo";
			case MaterialPass::outline: return "outline";
			case MaterialPass::shadow: return "shadow";
			case MaterialPass::present: return "present";
			case MaterialPass::count: return "count";
			default: return "unknown";
		}
	}
}