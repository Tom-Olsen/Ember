#pragma once
#include <cstdint>



namespace emberCommon
{
	struct ComputeShaderId
	{
		uint32_t index;
		uint32_t generation;
	};
	inline constexpr ComputeShaderId invalidComputeShaderId{ static_cast<uint32_t>(-1), static_cast<uint32_t>(-1) };
}