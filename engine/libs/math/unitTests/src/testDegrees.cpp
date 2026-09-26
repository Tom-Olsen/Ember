#include "emberMath.h"
#include <gtest/gtest.h>
#include <sstream>
#include <type_traits>



static_assert(std::is_constructible_v<Degrees, float>);
static_assert(!std::is_convertible_v<float, Degrees>);
static_assert(!std::is_convertible_v<Degrees, float>);
static_assert(std::is_trivially_copyable_v<Degrees>);
static_assert(sizeof(Degrees) == sizeof(float));



// Constructors:
TEST(Degrees, DefaultConstructor)
{
	Degrees degrees;
	EXPECT_FLOAT_EQ(degrees.value, 0.0f);
}
TEST(Degrees, ValueConstructor)
{
	Degrees degrees(90.0f);
	EXPECT_FLOAT_EQ(degrees.value, 90.0f);
}



// Conversions:
TEST(Degrees, ToRadians)
{
	const Degrees degrees(180.0f);
	Radians radians = degrees.ToRadians();
	EXPECT_TRUE(math::IsEpsilonEqual(radians.value, math::pi));
}
TEST(Degrees, ToRadiansPreservesRange)
{
	const Degrees degrees(-450.0f);
	Radians radians = degrees.ToRadians();
	EXPECT_TRUE(math::IsEpsilonEqual(radians.value, -2.5f * math::pi));
	EXPECT_TRUE(radians.ToDegrees().IsEpsilonEqual(degrees));
}



// Addition:
TEST(Degrees, OperatorAddition)
{
	Degrees result = Degrees(30.0f) + Degrees(45.0f);
	EXPECT_FLOAT_EQ(result.value, 75.0f);
}
TEST(Degrees, OperatorAdditionAssignment)
{
	Degrees result(30.0f);
	result += Degrees(45.0f);
	EXPECT_FLOAT_EQ(result.value, 75.0f);
}



// Subtraction:
TEST(Degrees, OperatorSubtraction)
{
	Degrees result = Degrees(75.0f) - Degrees(45.0f);
	EXPECT_FLOAT_EQ(result.value, 30.0f);
}
TEST(Degrees, OperatorSubtractionAssignment)
{
	Degrees result(75.0f);
	result -= Degrees(45.0f);
	EXPECT_FLOAT_EQ(result.value, 30.0f);
}
TEST(Degrees, OperatorNegation)
{
	Degrees result = -Degrees(45.0f);
	EXPECT_FLOAT_EQ(result.value, -45.0f);
}



// Multiplication:
TEST(Degrees, OperatorMultiplication)
{
	Degrees result = Degrees(45.0f) * 2.0f;
	EXPECT_FLOAT_EQ(result.value, 90.0f);
}
TEST(Degrees, OperatorLeftMultiplication)
{
	Degrees result = 2.0f * Degrees(45.0f);
	EXPECT_FLOAT_EQ(result.value, 90.0f);
}
TEST(Degrees, OperatorMultiplicationAssignment)
{
	Degrees result(45.0f);
	result *= 2.0f;
	EXPECT_FLOAT_EQ(result.value, 90.0f);
}



// Division:
TEST(Degrees, OperatorScalarDivision)
{
	Degrees result = Degrees(90.0f) / 2.0f;
	EXPECT_FLOAT_EQ(result.value, 45.0f);
}
TEST(Degrees, OperatorAngleDivision)
{
	float result = Degrees(90.0f) / Degrees(45.0f);
	EXPECT_FLOAT_EQ(result, 2.0f);
}
TEST(Degrees, OperatorDivisionAssignment)
{
	Degrees result(90.0f);
	result /= 2.0f;
	EXPECT_FLOAT_EQ(result.value, 45.0f);
}



// Comparison:
TEST(Degrees, IsEpsilonEqual)
{
	Degrees a(90.0f);
	Degrees b(90.0f + 0.5f * math::absEpsilon);
	EXPECT_TRUE(a.IsEpsilonEqual(b));
	EXPECT_FALSE(a == b);
}
TEST(Degrees, OperatorEquality)
{
	EXPECT_TRUE(Degrees(90.0f) == Degrees(90.0f));
	EXPECT_FALSE(Degrees(90.0f) == Degrees(45.0f));
}
TEST(Degrees, OperatorInequality)
{
	EXPECT_TRUE(Degrees(90.0f) != Degrees(45.0f));
	EXPECT_FALSE(Degrees(90.0f) != Degrees(90.0f));
}
TEST(Degrees, OperatorOrdering)
{
	Degrees smaller(45.0f);
	Degrees larger(90.0f);
	EXPECT_TRUE(smaller < larger);
	EXPECT_TRUE(smaller <= larger);
	EXPECT_TRUE(smaller <= smaller);
	EXPECT_TRUE(larger > smaller);
	EXPECT_TRUE(larger >= smaller);
	EXPECT_TRUE(larger >= larger);
}



// Logging:
TEST(Degrees, ToString)
{
	EXPECT_EQ(Degrees(90.0f).ToString(), "90 deg");
}
TEST(Degrees, OperatorStream)
{
	std::ostringstream stream;
	stream << Degrees(90.0f);
	EXPECT_EQ(stream.str(), "90 deg");
}