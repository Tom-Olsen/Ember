#pragma once
#include <cstdint>
#include <string_view>



namespace emberCommon
{
	enum class MaterialType : uint8_t
	{
		forwardOpaque,
		forwardTransparent,
		shadow,
		skybox,
		present
	};
	inline constexpr std::string_view MaterialTypeToString(MaterialType type)
	{
		switch (type)
		{
			case MaterialType::forwardOpaque: return "forwardOpaque";
			case MaterialType::forwardTransparent: return "forwardTransparent";
			case MaterialType::shadow: return "shadow";
			case MaterialType::skybox: return "skybox";
			case MaterialType::present: return "present";
			default: return "unknown";
		}
	}
}