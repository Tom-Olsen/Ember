#pragma once
#include <cstdint>



namespace emberCommon
{
	struct MaterialId
	{
		uint32_t index;
		uint32_t generation;
		
		// Equality operator overloading:
        constexpr bool operator==(const MaterialId& other) const
        {
            return index == other.index && generation == other.generation;
        }
        constexpr bool operator!=(const MaterialId& other) const
        {
            return !(*this == other);
        }
	};
	inline constexpr MaterialId invalidMaterialId{ static_cast<uint32_t>(-1), static_cast<uint32_t>(-1) };
}