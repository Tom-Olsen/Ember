#pragma once
#include <limits>
#include <stdint.h>



namespace emberMath
{
	// Enums.
	enum class CoordinateSpace
	{
		local,
		world
	};

	// Multi indexing of 2d data e.g. Float2x2 etc.
	struct Index2
	{
		uint32_t i, j;
	};

	namespace math
	{
		constexpr float relEpsilon = 1e-3f; // for comparing big   floats: |a-b|/max(a,b) <= relEpsilon -> roughly equal.
		constexpr float absEpsilon = 1e-5f; // for comparing small floats:          |a-b| <= absEpsilon -> roughly equal.
		constexpr float pi =  3.1415927f;
		constexpr float pi2 = 0.500f * pi;
		constexpr float pi4 = 0.250f * pi;
		constexpr float pi8 = 0.125f * pi;
		constexpr float piInv = 1.0f / pi;
		constexpr float phi = 1.618034f;
		constexpr float rad2deg = 180.0f / pi;
		constexpr float deg2rad = pi / 180.0f;
		constexpr float e = 2.7182818f;
		constexpr float goldenAngle = 2.3999632f;
		constexpr float sqrt2 = 1.41421356f;
		constexpr float sqrt2Inv = 0.70710678f;
		constexpr float sqrt3 = 1.7320508f;
		constexpr float sqrt3Inv = 0.57735027f;
		constexpr float maxValue = std::numeric_limits<float>::max();
		constexpr float minValue = std::numeric_limits<float>::lowest();
	}
}