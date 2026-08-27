#pragma once
#include <cstdint>
#include <string_view>



namespace emberCommon
{
	enum class MaterialPass : uint32_t
	{
		gizmo,
		outline,
		shadow,
		deferredGeometry,
		deferredLighting,
		forward,
		present,
		count
	};
	inline constexpr std::string_view MaterialPassToString(MaterialPass materialPass)
	{
		switch (materialPass)
		{
			case MaterialPass::gizmo: return "gizmo";
			case MaterialPass::outline: return "outline";
			case MaterialPass::shadow: return "shadow";
			case MaterialPass::deferredGeometry: return "deferredGeometry";
			case MaterialPass::deferredLighting: return "deferredLighting";
			case MaterialPass::forward: return "forward";
			case MaterialPass::present: return "present";
			case MaterialPass::count: return "count";
			default: return "unknown";
		}
	}
}