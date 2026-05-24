#include "emberMath.h"
#include <gtest/gtest.h>



// Basic math:
TEST(Mathf, Abs)
{
	EXPECT_FLOAT_EQ(math::Abs( 0.0f), 0.0f);
	EXPECT_FLOAT_EQ(math::Abs(-0.0f), 0.0f);
	EXPECT_FLOAT_EQ(math::Abs( 1.0f), 1.0f);
	EXPECT_FLOAT_EQ(math::Abs(-1.0f), 1.0f);
}
TEST(Mathf, Clamp)
{
	float min = 0.5f;
	float max = 1.5f;
	EXPECT_TRUE(math::IsEpsilonEqual(math::Clamp(0.0f, min, max), 0.5f));
	EXPECT_TRUE(math::IsEpsilonEqual(math::Clamp(1.0f, min, max), 1.0f));
	EXPECT_TRUE(math::IsEpsilonEqual(math::Clamp(2.0f, min, max), 1.5f));
}
TEST(Mathf, Max)
{
	EXPECT_TRUE(math::IsEpsilonEqual(math::Max(0.0f, 1.0f), 1.0f));
	EXPECT_TRUE(math::IsEpsilonEqual(math::Max(1.0f, 0.0f), 1.0f));
}
TEST(Mathf, Min)
{
	EXPECT_TRUE(math::IsEpsilonEqual(math::Min(0.0f, 1.0f), 0.0f));
	EXPECT_TRUE(math::IsEpsilonEqual(math::Min(1.0f, 0.0f), 0.0f));
}
TEST(Mathf, Sign)
{
	EXPECT_FLOAT_EQ(math::Sign(0.0f), 0.0f);
	EXPECT_FLOAT_EQ(math::Sign(-0.0f), 0.0f);
	EXPECT_FLOAT_EQ(math::Sign(4.0f), 1.0f);
	EXPECT_FLOAT_EQ(math::Sign(-4.0f), -1.0f);
}
TEST(Mathf, Sqrt)
{
	EXPECT_TRUE(math::IsEpsilonEqual(math::Sqrt(0.0f), 0.0f));
	EXPECT_TRUE(math::IsEpsilonEqual(math::Sqrt(1.0f), 1.0f));
	EXPECT_TRUE(math::IsEpsilonEqual(math::Sqrt(4.0f), 2.0f));
	EXPECT_TRUE(math::IsEpsilonEqual(math::Sqrt(9.0f), 3.0f));
	EXPECT_TRUE(std::isnan(math::Sqrt(-9.0f)));
}
TEST(Mathf, Factorial)
{
	int factorial = 1;
	for (int i = 0; i < 10; i++)
	{
		if (i > 0)
			factorial *= i;
		EXPECT_EQ(math::Factorial(i), factorial);
	}
}

// Trigonometry:
TEST(Mathf, Sin)
{
	EXPECT_TRUE(math::IsEpsilonEqual(math::Sin(0.0f), 0.0f));
	EXPECT_TRUE(math::IsEpsilonEqual(math::Sin(math::pi2), 1.0f));
	EXPECT_TRUE(math::IsEpsilonEqual(math::Sin(math::pi), 0.0f));
	EXPECT_TRUE(math::IsEpsilonEqual(math::Sin(3.0f * math::pi2), -1.0f));
	EXPECT_TRUE(math::IsEpsilonEqual(math::Sin(2.0f * math::pi), 0.0f));
}
TEST(Mathf, Cos)
{
	EXPECT_TRUE(math::IsEpsilonEqual(math::Cos(0.0f), 1.0f));
	EXPECT_TRUE(math::IsEpsilonEqual(math::Cos(math::pi2), 0.0f));
	EXPECT_TRUE(math::IsEpsilonEqual(math::Cos(math::pi), -1.0f));
	EXPECT_TRUE(math::IsEpsilonEqual(math::Cos(3.0f * math::pi2), 0.0f));
	EXPECT_TRUE(math::IsEpsilonEqual(math::Cos(2.0f * math::pi), 1.0f));
}
TEST(Mathf, Tan)
{
	EXPECT_TRUE(math::IsEpsilonEqual(math::Tan(0.0f), 0.0f));
	EXPECT_TRUE(math::IsEpsilonEqual(math::Tan(math::pi4), 1.0f));
	EXPECT_TRUE(math::IsEpsilonEqual(math::Tan(-math::pi4), -1.0f));
	EXPECT_TRUE(math::IsEpsilonEqual(math::Tan(3.0f * math::pi4), -1.0f));
	EXPECT_TRUE(math::IsEpsilonEqual(math::Tan(-3.0f * math::pi4), 1.0f));
}
TEST(Mathf, Asin)
{
	EXPECT_TRUE(math::IsEpsilonEqual(math::Asin(0.0f), 0.0f));
	EXPECT_TRUE(math::IsEpsilonEqual(math::Asin(1.0f), math::pi2));
	EXPECT_TRUE(math::IsEpsilonEqual(math::Asin(-1.0f), -math::pi2));
}
TEST(Mathf, Acos)
{
	EXPECT_TRUE(math::IsEpsilonEqual(math::Acos(1.0f), 0.0f));
	EXPECT_TRUE(math::IsEpsilonEqual(math::Acos(0.0f), math::pi2));
	EXPECT_TRUE(math::IsEpsilonEqual(math::Acos(-1.0f), math::pi));
}
TEST(Mathf, Atan)
{
	EXPECT_TRUE(math::IsEpsilonEqual(math::Atan(0.0f), 0.0f));
	EXPECT_TRUE(math::IsEpsilonEqual(math::Atan(1.0f), math::pi4));
	EXPECT_TRUE(math::IsEpsilonEqual(math::Atan(-1.0f), -math::pi4));
}
TEST(Mathf, Atan2)
{
	EXPECT_TRUE(math::IsEpsilonEqual(math::Atan2(0.0f, 1.0f), 0.0f));
	EXPECT_TRUE(math::IsEpsilonEqual(math::Atan2(1.0f, 1.0f), math::pi4));
	EXPECT_TRUE(math::IsEpsilonEqual(math::Atan2(1.0f, 0.0f), math::pi2));
	EXPECT_TRUE(math::IsEpsilonEqual(math::Atan2(1.0f, -1.0f), 3.0f * math::pi4));
	EXPECT_TRUE(math::IsEpsilonEqual(math::Atan2(0.0f, -1.0f), math::pi));
	EXPECT_TRUE(math::IsEpsilonEqual(math::Atan2(-1.0f, -1.0f), -3.0f * math::pi4));
	EXPECT_TRUE(math::IsEpsilonEqual(math::Atan2(-1.0f, 0.0f), -math::pi2));
	EXPECT_TRUE(math::IsEpsilonEqual(math::Atan2(-1.0f, 1.0f), -math::pi4));
}