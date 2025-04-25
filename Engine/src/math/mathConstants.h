#ifndef __INCLUDE_GUARD_mathConstants_h__
#define __INCLUDE_GUARD_mathConstants_h__
#include <stdint.h>



namespace emberMath
{
	// Enums.
	enum CoordinateSystem
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
		constexpr float epsilon = 1e-4f;
		constexpr float pi =  3.14159265358979323846f;
		constexpr float pi2 = 0.50f * pi;
		constexpr float pi4 = 0.25f * pi;
		constexpr float piInv = 1.0f / pi;
		constexpr float phi = 1.61803398874989484820f;
		constexpr float rad2deg = 180.0f / pi;
		constexpr float deg2rad = pi / 180.0f;
		constexpr float e = 2.71828182845904523536f;
		constexpr float sqrt2 = 1.41421356237309504880f;
		constexpr float sqrt2Inv = 0.70710678118654752440f;
		constexpr float sqrt3 = 1.73205080756887729353f;
		constexpr float sqrt3Inv = 0.57735026918962576451f;
		constexpr float maxValue = 3.402823466e+38f;
		constexpr float minValue = -3.402823466e+38f;
	}
}
#endif // __INCLUDE_GUARD_mathConstants_h__