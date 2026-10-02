#pragma once
#include <cstdint>



namespace emberCommon
{
	struct MaterialShaderId
	{
		uint32_t index;
		uint32_t generation;

		// Equality operator overloading:
		constexpr bool operator==(const MaterialShaderId& other) const
		{
			return index == other.index && generation == other.generation;
		}
		constexpr bool operator!=(const MaterialShaderId& other) const
		{
			return !(*this == other);
		}
	};
	inline constexpr MaterialShaderId invalidMaterialShaderId{ static_cast<uint32_t>(-1), static_cast<uint32_t>(-1) };
}