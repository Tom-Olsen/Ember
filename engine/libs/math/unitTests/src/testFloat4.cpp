#include "emberMath.h"
#include "macros.h"
#include <gtest/gtest.h>



// Constructors:
TEST(Float4, DirectionConstructor)
{
	Float4 direction = Float4::Direction(math::pi4, math::pi4);
	EXPECT_NEAR4(direction, Float4(0.5f, 0.5f, math::sqrt2Inv, 0.0f), epsilon);
}

// Math operations:
TEST(Float4, LengthSq)
{
	Float4 a(1.0f, 2.0f, 3.0f, 4.0f);
	float lengthSq = a.LengthSq();
	EXPECT_NEAR(lengthSq, 30.0f, epsilon);
}
TEST(Float4, Length)
{
	Float4 a(1.0f, 2.0f, 3.0f, 4.0f);
	float length = a.Length();
	EXPECT_NEAR(length, math::Sqrt(30.0f), epsilon);
}
TEST(Float4, IsEpsilonZero)
{
	Float4 a = Float4(0.9f * math::epsilon, -0.9f * math::epsilon, 0.9f * math::epsilon, -0.9f * math::epsilon);
	EXPECT_TRUE(a.IsEpsilonZero());
}

// Static math operations:
TEST(Float4, Abs)
{
	Float4 a(-1.0f, -2.0f, -3.0f, 4.0f);
	Float4 abs = Float4::Abs(a);
	EXPECT_NEAR4(abs, Float4(1.0f, 2.0f, 3.0f, 4.0f), epsilon);
}
TEST(Float4, Min)
{
	Float4 a(1.0f, 2.0f, 4.0f, 1.0f);
	Float4 b(3.0f, 4.0f, 1.0f, 2.0f);
	Float4 min = Float4::Min(a, b);
	EXPECT_NEAR4(min, Float4(1.0f, 2.0f, 1.0f, 1.0f), epsilon);
}
TEST(Float4, Max)
{
	Float4 a(1.0f, 2.0f, 4.0f, 1.0f);
	Float4 b(3.0f, 4.0f, 1.0f, 2.0f);
	Float4 max = Float4::Max(a, b);
	EXPECT_NEAR4(max, Float4(3.0f, 4.0f, 4.0f, 2.0f), epsilon);
}
TEST(Float4, Clamp)
{
	Float4 value(1.0f, 5.0f, 2.0f, 1.0f);
	Float4 min(2.0f, 3.0f, 1.0f, -2.0f);
	Float4 max(3.0f, 4.0f, 3.0f, -1.0f);
	Float4 clamped = Float4::Clamp(value, min, max);
	EXPECT_NEAR4(clamped, Float4(2.0f, 4.0f, 2.0f, -1.0f), epsilon);
}

// Access:
TEST(Float4, OperatorBrackets)
{
	Float4 a;
	a[0] = 1.0f;
	a[1] = 2.0f;
	a[2] = 3.0f;
	a[3] = 4.0f;
	EXPECT_FLOAT_EQ(a.x, 1.0f);
	EXPECT_FLOAT_EQ(a.y, 2.0f);
	EXPECT_FLOAT_EQ(a.z, 3.0f);
	EXPECT_FLOAT_EQ(a.w, 4.0f);
	EXPECT_FLOAT_EQ(a[0], 1.0f);
	EXPECT_FLOAT_EQ(a[1], 2.0f);
	EXPECT_FLOAT_EQ(a[2], 3.0f);
	EXPECT_FLOAT_EQ(a[3], 4.0f);
}
TEST(Float4, xyz)
{
	Float4 a(1.0f, 2.0f, 3.0f, 4.0f);
	Float3 xyz = a.xyz();
	EXPECT_NEAR3(xyz, Float3(1.0f, 2.0f, 3.0f), epsilon);
}

// Assignment:
TEST(Float4, OperatorAssignment)
{
	Float4 a(1.0f, 2.0f, 3.0f, 4.0f);
	Float4 b(5.0f, 6.0f, 7.0f, 8.0f);
	a = b;
	EXPECT_FLOAT4_EQ(a, b);
}

// Addition:
TEST(Float4, OperatorAddition)
{
	Float4 a(1.0f, 2.0f, 3.0f, 4.0f);
	Float4 b(5.0f, 6.0f, 7.0f, 8.0f);
	Float4 add = a + b;
	EXPECT_NEAR4(add, Float4(6.0f, 8.0f, 10.0f, 12.0f), epsilon);
}
TEST(Float4, OperatorAdditionAssignment)
{
	Float4 add(1.0f, 2.0f, 3.0f, 4.0f);
	Float4 b(5.0f, 6.0f, 7.0f, 8.0f);
	add +=b;
	EXPECT_NEAR4(add, Float4(6.0f, 8.0f, 10.0f, 12.0f), epsilon);
}

// Substraction:
TEST(Float4, OperatorSubstraction)
{
	Float4 a(1.0f, 2.0f, 3.0f, 4.0f);
	Float4 b(5.0f, 6.0f, 7.0f, 8.0f);
	Float4 sub = a - b;
	EXPECT_NEAR4(sub, Float4(-4.0f, -4.0f, -4.0f, -4.0f), epsilon);
}
TEST(Float4, OperatorSubstractionAssignment)
{
	Float4 sub(1.0f, 2.0f, 3.0f, 4.0f);
	Float4 b(5.0f, 6.0f, 7.0f, 8.0f);
	sub -= b;
	EXPECT_NEAR4(sub, Float4(-4.0f, -4.0f, -4.0f, -4.0f), epsilon);
}
TEST(Float4, OperatorNegation)
{
	Float4 a(1.0f, 2.0f, 3.0f, 4.0f);
	Float4 negation = -a;
	EXPECT_NEAR4(negation, Float4(-1.0f, -2.0f, -3.0f, -4.0f), epsilon);
}

// Multiplication:
TEST(Float4, OperatorMultiplication)
{
	Float4 a(1.0f, 2.0f, 3.0f, 4.0f);
	Float4 b(5.0f, 6.0f, 7.0f, 8.0f);
	Float4 mult = a * b;
	EXPECT_NEAR4(mult, Float4(5.0f, 12.0f, 21.0f, 32.0f), epsilon);
}
TEST(Float4, OperatorMultiplicationAssignment)
{
	Float4 mult(1.0f, 2.0f, 3.0f, 4.0f);
	Float4 b(5.0f, 6.0f, 7.0f, 8.0f);
	mult *=b;
	EXPECT_NEAR4(mult, Float4(5.0f, 12.0f, 21.0f, 32.0f), epsilon);
}
TEST(Float4, OperatorMultiplicationScalarAssignment)
{
	Float4 mult(1.0f, 2.0f, 3.0f, 4.0f);
	float b = 3.0f;
	mult *= b;
	EXPECT_NEAR4(mult, Float4(3.0f, 6.0f, 9.0f, 12.0f), epsilon);
}

// Division:
TEST(Float4, OperatorDivision)
{
	Float4 a(1.0f, 2.0f, 3.0f, 4.0f);
	Float4 b(5.0f, 6.0f, 7.0f, 8.0f);
	Float4 div = a / b;
	EXPECT_NEAR4(div, Float4(1.0f / 5.0f, 2.0f / 6.0f, 3.0f / 7.0f, 4.0f / 8.0f), epsilon);
}
TEST(Float4, OperatorDivisionAssignment)
{
	Float4 div(1.0f, 2.0f, 3.0f, 4.0f);
	Float4 b(5.0f, 6.0f, 7.0f, 8.0f);
	div /= b;
	EXPECT_NEAR4(div, Float4(1.0f / 5.0f, 2.0f / 6.0f, 3.0f / 7.0f, 4.0f / 8.0f), epsilon);
}
TEST(Float4, OperatorDivisionScalarAssignment)
{
	Float4 div(1.0f, 2.0f, 3.0f, 4.0f);
	float b = 4.0f;
	div /= b;
	EXPECT_NEAR4(div, Float4(1.0f / 4.0f, 2.0f / 4.0f, 3.0f / 4.0f, 1.0f), epsilon);
}

// Comparison:
TEST(Float4, IsEpsilonEqual)
{
	Float4 a(1.0f, 2.0f, 3.0f, 4.0f);
	Float4 b(1.0f + epsilon, 2.0f - epsilon, 3.0f + epsilon, 4.0 - epsilon);
	EXPECT_TRUE(a.IsEpsilonEqual(b));
}
TEST(Float4, OperatorEqual)
{
	Float4 a(1.0f, 2.0f, 3.0f, 4.0f);
	Float4 b(1.0f, 2.0f, 3.0f, 4.0f);
	EXPECT_TRUE(a == b);
}
TEST(Float4, OperatorNotEqual)
{
	Float4 a(1.0f, 2.0f, 3.0f, 4.0f);
	Float4 b(1.0f, 2.0f, 3.0f, 4.1f);
	EXPECT_TRUE(a != b);
}

// Friend functions:
TEST(Float4, OperatorMultiplicationScalarLeft)
{
	Float4 a(1.0f, 2.0f, 3.0f, 4.0f);
	float b = 3.0f;
	Float4 mult = b * a;
	EXPECT_NEAR4(mult, Float4(3.0f, 6.0f, 9.0f, 12.0f), epsilon);
}
TEST(Float4, OperatorMultiplicationScalarRight)
{
	Float4 a(1.0f, 2.0f, 3.0f, 4.0f);
	float b = 3.0f;
	Float4 mult = a * b;
	EXPECT_NEAR4(mult, Float4(3.0f, 6.0f, 9.0f, 12.0f), epsilon);
}
TEST(Float4, OperatorDivisionScalarLeft)
{
	Float4 a(1.0f, 2.0f, 3.0f, 4.0f);
	float b = 4.0f;
	Float4 div = b / a;
	EXPECT_NEAR4(div, Float4(4.0f / 1.0f, 4.0f / 2.0f, 4.0f / 3.0f, 4.0f / 4.0f), epsilon);
}
TEST(Float4, OperatorDivisionScalarRight)
{
	Float4 a(1.0f, 2.0f, 3.0f, 4.0f);
	float b = 4.0f;
	Float4 div = a / b;
	EXPECT_NEAR4(div, Float4(1.0f / 4.0f, 2.0f / 4.0f, 3.0f / 4.0f, 4.0f / 4.0f), epsilon);
}