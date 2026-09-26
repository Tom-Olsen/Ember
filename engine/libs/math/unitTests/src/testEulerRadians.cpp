#include "emberMath.h"
#include <gtest/gtest.h>
#include <sstream>
#include <type_traits>



static_assert(std::is_constructible_v<EulerRadians, const Float3&>);
static_assert(!std::is_convertible_v<Float3, EulerRadians>);
static_assert(!std::is_convertible_v<EulerRadians, Float3>);
static_assert(sizeof(EulerRadians) == sizeof(Float3));



// Constructors:
TEST(EulerRadians, DefaultConstructor)
{
	EulerRadians eulerAngles;
	EXPECT_TRUE(eulerAngles.value == Float3::zero);
}
TEST(EulerRadians, Float3Constructor)
{
	EulerRadians eulerAngles(Float3(0.5f, 1.0f, 1.5f));
	EXPECT_TRUE(eulerAngles.value == Float3(0.5f, 1.0f, 1.5f));
}
TEST(EulerRadians, FloatConstructor)
{
	EulerRadians eulerAngles(0.5f, 1.0f, 1.5f);
	EXPECT_TRUE(eulerAngles.value == Float3(0.5f, 1.0f, 1.5f));
}
TEST(EulerRadians, RadiansConstructor)
{
	EulerRadians eulerAngles(Radians(0.5f), Radians(1.0f), Radians(1.5f));
	EXPECT_TRUE(eulerAngles.value == Float3(0.5f, 1.0f, 1.5f));
}
TEST(EulerRadians, DegreesConstructor)
{
	EulerRadians eulerAngles(Degrees(45.0f), Degrees(90.0f), Degrees(180.0f));
	EXPECT_TRUE(eulerAngles.value.IsEpsilonEqual(Float3(math::pi4, math::pi2, math::pi)));
}



// Conversions:
TEST(EulerRadians, ToDegrees)
{
	const EulerRadians eulerAngles(Float3(math::pi2, math::pi, -2.5f * math::pi));
	EulerDegrees degrees = eulerAngles.ToDegrees();
	EXPECT_TRUE(degrees.value.IsEpsilonEqual(Float3(90.0f, 180.0f, -450.0f)));
	EXPECT_TRUE(degrees.ToRadians().IsEpsilonEqual(eulerAngles));
}



// Getters:
TEST(EulerRadians, GetValue)
{
	const EulerRadians eulerAngles(Float3(0.5f, 1.0f, 1.5f));
	EXPECT_TRUE(eulerAngles.GetValue() == Float3(0.5f, 1.0f, 1.5f));
}
TEST(EulerRadians, GetComponents)
{
	const EulerRadians eulerAngles(Float3(0.5f, 1.0f, 1.5f));
	EXPECT_FLOAT_EQ(eulerAngles.GetX().value, 0.5f);
	EXPECT_FLOAT_EQ(eulerAngles.GetY().value, 1.0f);
	EXPECT_FLOAT_EQ(eulerAngles.GetZ().value, 1.5f);
}



// Arithmetic:
TEST(EulerRadians, OperatorsScalar)
{
	EulerRadians angles(0.5f, 1.0f, 1.5f);
	EXPECT_EQ(angles * 2.0f, EulerRadians(1.0f, 2.0f, 3.0f));
	EXPECT_EQ(2.0f * angles, EulerRadians(1.0f, 2.0f, 3.0f));
	EXPECT_EQ(angles / 0.5f, EulerRadians(1.0f, 2.0f, 3.0f));
	angles *= 2.0f;
	EXPECT_EQ(angles, EulerRadians(1.0f, 2.0f, 3.0f));
	angles /= 4.0f;
	EXPECT_EQ(angles, EulerRadians(0.25f, 0.5f, 0.75f));
}



// Comparison:
TEST(EulerRadians, IsEpsilonEqual)
{
	EulerRadians a(Float3(0.5f, 1.0f, 1.5f));
	EulerRadians b(Float3(0.5f, 1.0f, 1.5f + 0.5f * math::absEpsilon));
	EXPECT_TRUE(a.IsEpsilonEqual(b));
	EXPECT_FALSE(a == b);
}
TEST(EulerRadians, OperatorsEquality)
{
	EulerRadians a(Float3(0.5f, 1.0f, 1.5f));
	EulerRadians b(Float3(0.5f, 1.0f, 1.5f));
	EulerRadians c(Float3(0.5f, 1.0f, 2.0f));
	EXPECT_TRUE(a == b);
	EXPECT_FALSE(a != b);
	EXPECT_TRUE(a != c);
	EXPECT_FALSE(a == c);
}



// Logging:
TEST(EulerRadians, ToString)
{
	EXPECT_EQ(EulerRadians(Float3(0.5f, 1.0f, 1.5f)).ToString(), "(0.5, 1, 1.5) rad");
}
TEST(EulerRadians, OperatorStream)
{
	std::ostringstream stream;
	stream << EulerRadians(Float3(0.5f, 1.0f, 1.5f));
	EXPECT_EQ(stream.str(), "(0.5, 1, 1.5) rad");
}