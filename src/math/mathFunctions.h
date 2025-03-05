#ifndef __INCLUDE_GUARD_mathFunctions_h__
#define __INCLUDE_GUARD_mathFunctions_h__
#include <stdint.h>



namespace emberMath
{
	namespace math
	{
		// Basic math:
		float Abs(float value);
		float Round(float value, int decimals = 0);
		float Floor(float value, int decimals = 0);
		float Ceil(float value, int decimals = 0);
		//float Clamp(float value, float min, float max);
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
		template<typename T>
		T Clamp(T value, T min, T max);
	}
}
#endif // __INCLUDE_GUARD_mathFunctions_h__