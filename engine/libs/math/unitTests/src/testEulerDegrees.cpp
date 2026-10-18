#include "emberMath.h"
#include <gtest/gtest.h>
#include <sstream>
#include <type_traits>



static_assert(std::is_constructible_v<EulerDegrees, const Float3&>);
static_assert(!std::is_convertible_v<Float3, EulerDegrees>);
static_assert(!std::is_convertible_v<EulerDegrees, Float3>);
static_assert(sizeof(EulerDegrees) == sizeof(Float3));
static_assert(std::is_constructible_v<EulerDegrees, const Float3x3&>);
static_assert(std::is_constructible_v<EulerDegrees, const Float4x4&>);
static_assert(!std::is_convertible_v<Float3x3, EulerDegrees>);
static_assert(!std::is_convertible_v<Float4x4, EulerDegrees>);



// Constructors:
TEST(EulerDegrees, DefaultConstructor)
{
	EulerDegrees eulerAngles;
	EXPECT_TRUE(eulerAngles.value == Float3::zero);
}
TEST(EulerDegrees, Float3Constructor)
{
	EulerDegrees eulerAngles(Float3(45.0f, 90.0f, 180.0f));
	EXPECT_TRUE(eulerAngles.value == Float3(45.0f, 90.0f, 180.0f));
}
TEST(EulerDegrees, FloatConstructor)
{
	EulerDegrees eulerAngles(45.0f, 90.0f, 180.0f);
	EXPECT_TRUE(eulerAngles.value == Float3(45.0f, 90.0f, 180.0f));
}
TEST(EulerDegrees, DegreesConstructor)
{
	EulerDegrees eulerAngles(Degrees(45.0f), Degrees(90.0f), Degrees(180.0f));
	EXPECT_TRUE(eulerAngles.value == Float3(45.0f, 90.0f, 180.0f));
}
TEST(EulerDegrees, RadiansConstructor)
{
	EulerDegrees eulerAngles(Radians(math::pi4), Radians(math::pi2), Radians(math::pi));
	EXPECT_TRUE(eulerAngles.value.IsEpsilonEqual(Float3(45.0f, 90.0f, 180.0f)));
}
TEST(EulerDegrees, RotationMatrixConstructors)
{
	EXPECT_EQ(EulerDegrees(Float3x3::identity), EulerDegrees());
	EXPECT_EQ(EulerDegrees(Float4x4::identity), EulerDegrees());
	EulerDegrees expected(25.0f, -35.0f, 70.0f);
	Float3x3 rotationMatrix = Float3x3::Rotate(expected.ToRadians());
	EXPECT_TRUE(EulerDegrees(rotationMatrix).IsEpsilonEqual(expected));
	EXPECT_TRUE(EulerDegrees{Float4x4(rotationMatrix)}.IsEpsilonEqual(expected));
}
TEST(EulerDegrees, RotationMatrixConstructorsGimbalLock)
{
	for (float x : {-90.0f, 90.0f})
	{
		Float3x3 rotationMatrix = Float3x3::Rotate(EulerDegrees(x, 50.0f, -30.0f).ToRadians());
		EXPECT_TRUE(Float3x3::Rotate(EulerDegrees(rotationMatrix).ToRadians()).IsEpsilonEqual(rotationMatrix));
		EXPECT_TRUE(Float3x3::Rotate(EulerDegrees{Float4x4(rotationMatrix)}.ToRadians()).IsEpsilonEqual(rotationMatrix));
	}
}
#ifndef NDEBUG
TEST(EulerDegrees, RotationMatrixConstructorsRejectInvalidRotations)
{
	EXPECT_DEATH((void)EulerDegrees(Float3x3::Scale(2.0f)), "Assertion");
	EXPECT_DEATH((void)EulerDegrees(Float4x4::Scale(2.0f)), "Assertion");
	EXPECT_DEATH((void)EulerDegrees(Float3x3::Scale(Float3(-1.0f, 1.0f, 1.0f))), "Assertion");
	EXPECT_DEATH((void)EulerDegrees(Float4x4::Translate(Float3::one)), "Assertion");
}
#endif



// Conversions:
TEST(EulerDegrees, ToRadians)
{
	const EulerDegrees eulerAngles(Float3(90.0f, 180.0f, -450.0f));
	EulerRadians radians = eulerAngles.ToRadians();
	EXPECT_TRUE(radians.value.IsEpsilonEqual(Float3(math::pi2, math::pi, -2.5f * math::pi)));
	EXPECT_TRUE(radians.ToDegrees().IsEpsilonEqual(eulerAngles));
}



// Getters:
TEST(EulerDegrees, GetValue)
{
	const EulerDegrees eulerAngles(Float3(45.0f, 90.0f, 180.0f));
	EXPECT_TRUE(eulerAngles.GetValue() == Float3(45.0f, 90.0f, 180.0f));
}
TEST(EulerDegrees, GetComponents)
{
	const EulerDegrees eulerAngles(Float3(45.0f, 90.0f, 180.0f));
	EXPECT_FLOAT_EQ(eulerAngles.GetX().value, 45.0f);
	EXPECT_FLOAT_EQ(eulerAngles.GetY().value, 90.0f);
	EXPECT_FLOAT_EQ(eulerAngles.GetZ().value, 180.0f);
}



// Arithmetic:
TEST(EulerDegrees, OperatorsScalar)
{
	EulerDegrees angles(15.0f, 30.0f, 45.0f);
	EXPECT_EQ(angles * 2.0f, EulerDegrees(30.0f, 60.0f, 90.0f));
	EXPECT_EQ(2.0f * angles, EulerDegrees(30.0f, 60.0f, 90.0f));
	EXPECT_EQ(angles / 3.0f, EulerDegrees(5.0f, 10.0f, 15.0f));
	angles *= 2.0f;
	EXPECT_EQ(angles, EulerDegrees(30.0f, 60.0f, 90.0f));
	angles /= 3.0f;
	EXPECT_EQ(angles, EulerDegrees(10.0f, 20.0f, 30.0f));
}



// Comparison:
TEST(EulerDegrees, IsEpsilonEqual)
{
	EulerDegrees a(Float3(45.0f, 90.0f, 180.0f));
	EulerDegrees b(Float3(45.0f, 90.0f, 180.0f + math::absEpsilon));
	EXPECT_TRUE(a.IsEpsilonEqual(b));
	EXPECT_FALSE(a == b);
}
TEST(EulerDegrees, OperatorsEquality)
{
	EulerDegrees a(Float3(45.0f, 90.0f, 180.0f));
	EulerDegrees b(Float3(45.0f, 90.0f, 180.0f));
	EulerDegrees c(Float3(45.0f, 90.0f, 270.0f));
	EXPECT_TRUE(a == b);
	EXPECT_FALSE(a != b);
	EXPECT_TRUE(a != c);
	EXPECT_FALSE(a == c);
}



// Logging:
TEST(EulerDegrees, ToString)
{
	EXPECT_EQ(EulerDegrees(Float3(45.0f, 90.0f, 180.0f)).ToString(), "(45, 90, 180) deg");
}
TEST(EulerDegrees, OperatorStream)
{
	std::ostringstream stream;
	stream << EulerDegrees(Float3(45.0f, 90.0f, 180.0f));
	EXPECT_EQ(stream.str(), "(45, 90, 180) deg");
}