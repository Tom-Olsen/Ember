#pragma once
#include <cstdint>



namespace emberCore
{
	struct MaterialId
	{
		uint32_t index;
		uint32_t generation;
	};
	inline constexpr MaterialId invalidMaterialId{ static_cast<uint32_t>(-1), static_cast<uint32_t>(-1) };
}