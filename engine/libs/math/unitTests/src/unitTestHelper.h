#include "float2.h"
#include "float3.h"
#include "float4.h"
#include <gtest/gtest.h>



// Floating point precision:
constexpr float epsilon = 1e-6f;


template<typename T>
inline void ExpectNearVec(const T& a, const T& b, float e)
{
	for (int i = 0; i < T::size; i++)
		EXPECT_NEAR(static_cast<float>(a[i]), static_cast<float>(b[i]), e);
}
template<typename T>
inline void ExpectEqualVec(const T& a, const T& b)
{
	for (int i = 0; i < T::size; i++)
		EXPECT_EQ(a[i], b[i]);
}