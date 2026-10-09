#pragma once
#include <cstdint>



namespace emberCommon
{
	struct TextureId
	{
		uint32_t index;
		uint32_t generation;

		// Equality operator overloading:
		constexpr bool operator==(const TextureId& other) const
		{
			return index == other.index && generation == other.generation;
		}
		constexpr bool operator!=(const TextureId& other) const
		{
			return !(*this == other);
		}
	};
	inline constexpr TextureId invalidTextureId{ static_cast<uint32_t>(-1), static_cast<uint32_t>(-1) };
}