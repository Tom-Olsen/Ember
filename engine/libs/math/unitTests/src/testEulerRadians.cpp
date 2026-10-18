#include "emberMath.h"
#include <gtest/gtest.h>
#include <limits>
#include <sstream>
#include <type_traits>



static_assert(std::is_constructible_v<EulerRadians, const Float3&>);
static_assert(!std::is_convertible_v<Float3, EulerRadians>);
static_assert(!std::is_convertible_v<EulerRadians, Float3>);
static_assert(sizeof(EulerRadians) == sizeof(Float3));
static_assert(std::is_constructible_v<EulerRadians, const Float3x3&>);
static_assert(std::is_constructible_v<EulerRadians, const Float4x4&>);
static_assert(!std::is_convertible_v<Float3x3, EulerRadians>);
static_assert(!std::is_convertible_v<Float4x4, EulerRadians>);



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
TEST(EulerRadians, RotationMatrixConstructors)
{
	EXPECT_EQ(EulerRadians(Float3x3::identity), EulerRadians());
	EXPECT_EQ(EulerRadians(Float4x4::identity), EulerRadians());
	for (float x : {-170.0f, -90.0f, -89.999f, -89.99f, -30.0f, 0.0f, 45.0f, 89.99f, 89.999f, 90.0f, 170.0f})
	{
		for (float y : {-150.0f, -20.0f, 0.0f, 70.0f, 180.0f})
		{
			for (float z : {-180.0f, -80.0f, 0.0f, 35.0f, 160.0f})
			{
				SCOPED_TRACE(EulerDegrees(x, y, z).ToString());
				Float3x3 rotationMatrix = Float3x3::Rotate(EulerDegrees(x, y, z).ToRadians());
				EulerRadians from3x3(rotationMatrix);
				EulerRadians from4x4{Float4x4(rotationMatrix)};
				EXPECT_TRUE(Float3x3::Rotate(from3x3).IsEpsilonEqual(rotationMatrix, 2e-5f, 2e-5f));
				EXPECT_TRUE(from4x4.IsEpsilonEqual(from3x3));
			}
		}
	}
}
TEST(EulerRadians, RotationMatrixConstructorPrincipalAngles)
{
	EulerRadians expected(0.4f, -0.7f, 1.2f);
	Float3x3 rotationMatrix = Float3x3::Rotate(expected);
	EXPECT_TRUE(EulerRadians(rotationMatrix).IsEpsilonEqual(expected));
}
TEST(EulerRadians, RotationMatrixConstructorGimbalLock)
{
	for (float x : {-90.0f, 90.0f})
	{
		Float3x3 rotationMatrix = Float3x3::Rotate(EulerDegrees(x, 35.0f, -60.0f).ToRadians());
		EulerRadians angles(rotationMatrix);
		EXPECT_FLOAT_EQ(angles.GetZ().value, 0.0f);
		EXPECT_TRUE(Float3x3::Rotate(angles).IsEpsilonEqual(rotationMatrix));
	}
}

#ifndef NDEBUG
TEST(EulerRadians, RotationMatrixConstructorsRejectInvalidRotations)
{
	EXPECT_DEATH((void)EulerRadians(Float3x3::zero), "Assertion");
	EXPECT_DEATH((void)EulerRadians(Float3x3::Scale(2.0f)), "Assertion");
	// A reflection is orthogonal, but its determinant is -1.
	EXPECT_DEATH((void)EulerRadians(Float3x3::Scale(Float3(-1.0f, 1.0f, 1.0f))), "Assertion");
	Float3x3 shearMatrix = Float3x3::identity;
	shearMatrix.data[3] = 0.25f;
	EXPECT_DEATH((void)EulerRadians(shearMatrix), "Assertion");
	Float3x3 invalidMatrix = Float3x3::identity;
	invalidMatrix.data[0] = std::numeric_limits<float>::quiet_NaN();
	EXPECT_DEATH((void)EulerRadians(invalidMatrix), "Assertion");
	invalidMatrix.data[0] = std::numeric_limits<float>::infinity();
	EXPECT_DEATH((void)EulerRadians(invalidMatrix), "Assertion");
	EXPECT_DEATH((void)EulerRadians(Float4x4::Scale(2.0f)), "Assertion");
	EXPECT_DEATH((void)EulerRadians(Float4x4::Scale(Float3(-1.0f, 1.0f, 1.0f))), "Assertion");
}
TEST(EulerRadians, RotationMatrixConstructorRejectsNonRotationHomogeneousEntries)
{
	for (int index : {3, 7, 11, 12, 13, 14, 15})
	{
		Float4x4 invalidMatrix = Float4x4::identity;
		invalidMatrix.data[index] += 0.5f;
		EXPECT_DEATH((void)EulerRadians(invalidMatrix), "Assertion");
		invalidMatrix.data[index] = std::numeric_limits<float>::infinity();
		EXPECT_DEATH((void)EulerRadians(invalidMatrix), "Assertion");
	}
}
#endif



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