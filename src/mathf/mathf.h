#ifndef __INCLUDE_GUARD_mathf_h__
#define __INCLUDE_GUARD_mathf_h__
#include <stdint.h>



namespace emberEngine
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



	// Collection of common math functions:
	namespace mathf
	{
		// Constants:
		constexpr float epsilon = 1e-4f;
		constexpr float pi = 3.14159265358979323846f;
		constexpr float pi2 = 0.50f * pi;
		constexpr float pi4 = 0.25f * pi;
		constexpr float piInv = 1.0f / pi;
		constexpr float rad2deg = 180.0f / pi;
		constexpr float deg2rad = pi / 180.0f;
		constexpr float e = 2.71828182845904523536f;
		constexpr float sqrt2 = 1.41421356237309504880f;
		constexpr float sqrt2Inv = 0.70710678118654752440f;
		constexpr float sqrt3 = 1.73205080756887729353f;
		constexpr float sqrt3Inv = 0.57735026918962576451f;
		constexpr float maxValue = 3.402823466e+38f;
		constexpr float minValue = -3.402823466e+38f;

		// Basic math:
		float Abs(float value);
		float Round(float value, int decimals = 0);
		float Floor(float value, int decimals = 0);
		float Ceil(float value, int decimals = 0);
		float Clamp(float value, float min, float max);
		float Sign(float value);
		float Sqrt(float value);
		float Pow(float value, float exponent);
		float Factorial(int n);

		// Trigonometry:
		float Sin(float angle);
		float Cos(float angle);
		float Tan(float angle);
		float Asin(float value);
		float Acos(float value);
		float Atan(float z);
		float Atan2(float y, float x);

		// Templates:
		template<typename T>
		T Min(T a, T b);
		template<typename T>
		T Max(T a, T b);
	}
}



// Include all mathf headers:
// Basic types:
#include "int2.h"
#include "int3.h"
#include "uint3.h"
#include "float2.h"
#include "float3.h"
#include "float4.h"
#include "float2x2.h"
#include "float2x3.h"
#include "float3x2.h"
#include "float3x3.h"
#include "float4x4.h"

// Geometry:
#include "bounds.h"
#include "bounds2d.h"
#include "geometry3d.h"
#include "ray.h"

// Random:
#include "emberRandom.h"
#endif // __INCLUDE_GUARD_mathf_h__