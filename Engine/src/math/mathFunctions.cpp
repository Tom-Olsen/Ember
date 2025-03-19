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
	}
}