#pragma once
#include <string_view>



namespace emberCommon
{
	enum class MaterialType : uint32_t
	{
		forwardOpaque,
		forwardTransparent,
		shadow,
		skybox,
		present
	};
	inline constexpr std::string_view MaterialTypeNames[] =
	{
		"forwardOpaque",
		"forwardTransparent",
		"shadow",
		"skybox",
		"present"
	};
}