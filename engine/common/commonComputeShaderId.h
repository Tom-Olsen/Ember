#pragma once
#include <cstdint>



namespace emberCommon
{
	struct ComputeShaderId
	{
		uint32_t index;
		uint32_t generation;
		
		// Equality operator overloading:
        constexpr bool operator==(const ComputeShaderId& other) const
        {
            return index == other.index && generation == other.generation;
        }
        constexpr bool operator!=(const ComputeShaderId& other) const
        {
            return !(*this == other);
        }
	};
	inline constexpr ComputeShaderId invalidComputeShaderId{ static_cast<uint32_t>(-1), static_cast<uint32_t>(-1) };
}