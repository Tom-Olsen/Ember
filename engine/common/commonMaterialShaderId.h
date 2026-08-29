#pragma once
#include <cstdint>



namespace emberCommon
{
	struct MaterialShaderId
	{
		uint32_t index;
		uint32_t generation;
	};
	inline constexpr MaterialShaderId invalidMaterialShaderId{ static_cast<uint32_t>(-1), static_cast<uint32_t>(-1) };
}