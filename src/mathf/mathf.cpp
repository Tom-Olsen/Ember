#include "mathf.h"
#include <math.h>



namespace mathf
{
	float Clamp(float value, float min, float max)
	{
		if (value < min)
			return min;
		if (value > max)
			return max;
		return value;
	}

	float ToDegrees(float radians)
	{
		return radians * 180.0f / PI;
	}

	float ToRadians(float degrees)
	{
		return degrees * PI / 180.0f;
	}

	float Sign(float value)
	{
		if (value > 0.0f)
			return 1.0f;
		if (value < 0.0f)
			return -1.0f;
		return 0.0f;
	}

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

	template<int N>
	float IntegerPow(float a)
	{
		if constexpr (N > 1)
			return a * IntegerPow<N - 1>(a);
		else if (N == 1)
			return a;
		else if (N == 0)
			return 1.0f;
		else if (N < 0)
			return 1.0f / IntegerPow<-N>(a);
	}
	template float IntegerPow<-9>(float a);
	template float IntegerPow<-8>(float a);
	template float IntegerPow<-7>(float a);
	template float IntegerPow<-6>(float a);
	template float IntegerPow<-5>(float a);
	template float IntegerPow<-4>(float a);
	template float IntegerPow<-3>(float a);
	template float IntegerPow<-2>(float a);
	template float IntegerPow<-1>(float a);
	template float IntegerPow< 0>(float a);
	template float IntegerPow< 1>(float a);
	template float IntegerPow< 2>(float a);
	template float IntegerPow< 3>(float a);
	template float IntegerPow< 4>(float a);
	template float IntegerPow< 5>(float a);
	template float IntegerPow< 6>(float a);
	template float IntegerPow< 7>(float a);
	template float IntegerPow< 8>(float a);
	template float IntegerPow< 9>(float a);

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
	template float Factorial<0>();
	template float Factorial<1>();
	template float Factorial<2>();
	template float Factorial<3>();
	template float Factorial<4>();
	template float Factorial<5>();
	template float Factorial<6>();
	template float Factorial<7>();
	template float Factorial<8>();
	template float Factorial<9>();
}