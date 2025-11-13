#pragma once
#include <cstddef>
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
		float Sign(float value);
		float Sqrt(float value);
		float Pow(float value, float exponent);
		float Factorial(int n);
		uint32_t NextPowerOfTwo(uint32_t n);
		uint64_t NextPowerOfTwo(uint64_t n);
		float Ln(float value);
		float Log(float value, float base);
		float Log2(float value);
		float Log10(float value);

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

		// Primes:
		bool IsPrime(size_t n);
		size_t NextPrimeAbove(size_t n);
	}
}