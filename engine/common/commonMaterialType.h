#pragma once
#include <cstdint>
#include <string_view>



namespace emberCommon
{
	enum class MaterialType : uint32_t
	{
		forward,
		shadow,
		present
	};
	inline constexpr std::string_view MaterialTypeNames[] =
	{
		"forward",
		"shadow",
		"present"
	};
}