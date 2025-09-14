#pragma once
#include <string_view>



namespace emberEngine
{
	enum class MaterialType : uint32_t
	{
		forwardOpaque,
		forwardTransparent,
		shadow,
		skybox,
		present
	};
	constexpr std::string_view MaterialTypeNames[] =
	{
		"forwardOpaque",
		"forwardTransparent",
		"shadow",
		"skybox",
		"present"
	};
}