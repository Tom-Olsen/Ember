#include "mathf.h"
#include <math.h>



namespace mathf
{
	// Basic math:
	float Abs(float value)
	{
		return fabsf(value);
	}
	float Clamp(float value, float min, float max)
	{
		if (value < min)
			return min;
		if (value > max)
			return max;
		return value;
	}
	float Max(float a, float b)
	{
		return a > b ? a : b;
	}
	float Min(float a, float b)
	{
		return a < b ? a : b;
	}
	float Sign(float value)
	{// value > 0.0f -> 1.0f, value < 0.0f -> -1.0f, value == 0.0f -> 0.0f
		return (0.0f < value) - (value < 0.0f);
	}
	float Sqrt(float vlaue)
	{
		return sqrtf(vlaue);
	}
	template<int N>
	float Factorial()
	{
		if constexpr (N > 1)
			return N * Factorial<N - 1>();
		else if (N == 1)
			return 1;
		else if (N == 0)
			return 1;
	}
	float Factorial(int n)
	{
		if (n < 2) return 1;
		else if (n == 2) return Factorial<2>();
		else if (n == 3) return Factorial<3>();
		else if (n == 4) return Factorial<4>();
		else if (n == 5) return Factorial<5>();
		else if (n == 6) return Factorial<6>();
		else if (n == 7) return Factorial<7>();
		else if (n == 8) return Factorial<8>();
		else if (n == 9) return Factorial<9>();
		else
			throw std::runtime_error("Factorials higher 9 not supported yet!");
		return -1;
	}



	// Trigonometry:
	float Sin(float radiant)
	{
		return sinf(radiant);
	}
	float Cos(float radiant)
	{
		return cosf(radiant);
	}
	float Tan(float radiant)
	{
		return tanf(radiant);
	}
	float Asin(float value)
	{
		return asinf(value);
	}
	float Acos(float value)
	{
		return acosf(value);
	}
	float Atan(float z)
	{
		return atanf(z);
	}
	float Atan2(float y, float x)
	{
		return atan2f(y, x);
	}
}