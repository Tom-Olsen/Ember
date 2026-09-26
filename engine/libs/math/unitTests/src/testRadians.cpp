#include "emberMath.h"
#include <gtest/gtest.h>
#include <sstream>
#include <type_traits>



static_assert(std::is_constructible_v<Radians, float>);
static_assert(!std::is_convertible_v<float, Radians>);
static_assert(!std::is_convertible_v<Radians, float>);
static_assert(std::is_trivially_copyable_v<Radians>);
static_assert(sizeof(Radians) == sizeof(float));



// Constructors:
TEST(Radians, DefaultConstructor)
{
	Radians radians;
	EXPECT_FLOAT_EQ(radians.value, 0.0f);
}
TEST(Radians, ValueConstructor)
{
	Radians radians(math::pi2);
	EXPECT_FLOAT_EQ(radians.value, math::pi2);
}



// Conversions:
TEST(Radians, ToDegrees)
{
	const Radians radians(math::pi);
	Degrees degrees = radians.ToDegrees();
	EXPECT_TRUE(math::IsEpsilonEqual(degrees.value, 180.0f));
}
TEST(Radians, ToDegreesPreservesRange)
{
	const Radians radians(-2.5f * math::pi);
	Degrees degrees = radians.ToDegrees();
	EXPECT_TRUE(math::IsEpsilonEqual(degrees.value, -450.0f));
	EXPECT_TRUE(degrees.ToRadians().IsEpsilonEqual(radians));
}



// Addition:
TEST(Radians, OperatorAddition)
{
	Radians result = Radians(1.0f) + Radians(2.0f);
	EXPECT_FLOAT_EQ(result.value, 3.0f);
}
TEST(Radians, OperatorAdditionAssignment)
{
	Radians result(1.0f);
	result += Radians(2.0f);
	EXPECT_FLOAT_EQ(result.value, 3.0f);
}



// Subtraction:
TEST(Radians, OperatorSubtraction)
{
	Radians result = Radians(3.0f) - Radians(2.0f);
	EXPECT_FLOAT_EQ(result.value, 1.0f);
}
TEST(Radians, OperatorSubtractionAssignment)
{
	Radians result(3.0f);
	result -= Radians(2.0f);
	EXPECT_FLOAT_EQ(result.value, 1.0f);
}
TEST(Radians, OperatorNegation)
{
	Radians result = -Radians(1.0f);
	EXPECT_FLOAT_EQ(result.value, -1.0f);
}



// Multiplication:
TEST(Radians, OperatorMultiplication)
{
	Radians result = Radians(1.5f) * 2.0f;
	EXPECT_FLOAT_EQ(result.value, 3.0f);
}
TEST(Radians, OperatorLeftMultiplication)
{
	Radians result = 2.0f * Radians(1.5f);
	EXPECT_FLOAT_EQ(result.value, 3.0f);
}
TEST(Radians, OperatorMultiplicationAssignment)
{
	Radians result(1.5f);
	result *= 2.0f;
	EXPECT_FLOAT_EQ(result.value, 3.0f);
}



// Division:
TEST(Radians, OperatorScalarDivision)
{
	Radians result = Radians(3.0f) / 2.0f;
	EXPECT_FLOAT_EQ(result.value, 1.5f);
}
TEST(Radians, OperatorAngleDivision)
{
	float result = Radians(3.0f) / Radians(1.5f);
	EXPECT_FLOAT_EQ(result, 2.0f);
}
TEST(Radians, OperatorDivisionAssignment)
{
	Radians result(3.0f);
	result /= 2.0f;
	EXPECT_FLOAT_EQ(result.value, 1.5f);
}



// Comparison:
TEST(Radians, IsEpsilonEqual)
{
	Radians a(1.0f);
	Radians b(1.0f + 0.5f * math::absEpsilon);
	EXPECT_TRUE(a.IsEpsilonEqual(b));
	EXPECT_FALSE(a == b);
}
TEST(Radians, OperatorEquality)
{
	EXPECT_TRUE(Radians(1.0f) == Radians(1.0f));
	EXPECT_FALSE(Radians(1.0f) == Radians(2.0f));
}
TEST(Radians, OperatorInequality)
{
	EXPECT_TRUE(Radians(1.0f) != Radians(2.0f));
	EXPECT_FALSE(Radians(1.0f) != Radians(1.0f));
}
TEST(Radians, OperatorOrdering)
{
	Radians smaller(1.0f);
	Radians larger(2.0f);
	EXPECT_TRUE(smaller < larger);
	EXPECT_TRUE(smaller <= larger);
	EXPECT_TRUE(smaller <= smaller);
	EXPECT_TRUE(larger > smaller);
	EXPECT_TRUE(larger >= smaller);
	EXPECT_TRUE(larger >= larger);
}



// Logging:
TEST(Radians, ToString)
{
	EXPECT_EQ(Radians(1.0f).ToString(), "1 rad");
}
TEST(Radians, OperatorStream)
{
	std::ostringstream stream;
	stream << Radians(1.0f);
	EXPECT_EQ(stream.str(), "1 rad");
}