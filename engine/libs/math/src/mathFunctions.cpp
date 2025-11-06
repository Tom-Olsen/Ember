#include "mathFunctions.h"
#include <math.h>
#include <stdexcept>



namespace emberMath
{
	namespace math
	{
		// Basic math:
		float Abs(float value)
		{
			return fabsf(value);
		}
		float Round(float value, int decimals)
		{
			float factor = powf(10.0f, decimals);
			return roundf(value * factor) / factor;
		}
		float Floor(float value, int decimals)
		{
			float factor = powf(10.0f, decimals);
			return floorf(value * factor) / factor;
		}
		float Ceil(float value, int decimals)
		{
			float factor = powf(10.0f, decimals);
			return ceilf(value * factor) / factor;
		}
		float Sign(float value)
		{// value > 0.0f -> 1.0f, value < 0.0f -> -1.0f, value == 0.0f -> 0.0f
			return (0.0f < value) - (value < 0.0f);
		}
		float Sqrt(float vlaue)
		{
			return sqrtf(vlaue);
		}
		float Pow(float value, float exponent)
		{
			return powf(value, exponent);
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
		uint32_t NextPowerOfTwo(uint32_t n)
		{
			if (n == 0) return 1;
			n--;
			n |= n >> 1;
			n |= n >> 2;
			n |= n >> 4;
			n |= n >> 8;
			n |= n >> 16;
			n++;
			return n;
		}
		uint64_t NextPowerOfTwo(uint64_t n)
		{
			if (n == 0) return 1;
			n--;
			n |= n >> 1;
			n |= n >> 2;
			n |= n >> 4;
			n |= n >> 8;
			n |= n >> 16;
			n |= n >> 32;
			n++;
			return n;
		}
		float Ln(float value)
		{
			return log(value);
		}
		float Log(float value, float base)
		{
			return log(value) / log(base);
		}
		float Log2(float value)
		{
			return log2(value);
		}
		float Log10(float value)
		{
			return log10(value);
		}



		// Trigonometry:
		float Sin(float angle)
		{
			return sinf(angle);
		}
		float Cos(float angle)
		{
			return cosf(angle);
		}
		float Tan(float angle)
		{
			return tanf(angle);
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



		// Templates:
		template<typename T>
		T Min(T a, T b)
		{
			return a < b ? a : b;
		}
		template float math::Min(float, float);
		template int math::Min(int, int);
		template uint16_t math::Min(uint16_t, uint16_t);
		template uint32_t math::Min(uint32_t, uint32_t);
		template uint64_t math::Min(uint64_t, uint64_t);
		template<typename T>

		T Max(T a, T b)
		{
			return a > b ? a : b;
		}
		template float math::Max(float, float);
		template int math::Max(int, int);
		template uint16_t math::Max(uint16_t, uint16_t);
		template uint32_t math::Max(uint32_t, uint32_t);
		template uint64_t math::Max(uint64_t, uint64_t);

		template<typename T>
		T Clamp(T value, T min, T max)
		{
			if (value < min)
				return min;
			if (value > max)
				return max;
			return value;
		}
		template float math::Clamp(float, float, float);
		template int math::Clamp(int, int, int);
		template uint16_t math::Clamp(uint16_t, uint16_t, uint16_t);
		template uint32_t math::Clamp(uint32_t, uint32_t, uint32_t);
		template uint64_t math::Clamp(uint64_t, uint64_t, uint64_t);



		// Primes:
		bool IsPrime(size_t n)
		{
			if (n < 2) return false;
			if (n % 2 == 0) return n == 2;
			for (size_t i = 3; i * i <= n; i += 2)
				if (n % i == 0) return false;
			return true;
		}
		size_t NextPrimeAbove(size_t n)
		{
			if (n <= 2) return 2;
			size_t candidate = (n % 2 == 0) ? n + 1 : n + 2;
			while (!IsPrime(candidate))
				candidate += 2;
			return candidate;
		}
	}
}