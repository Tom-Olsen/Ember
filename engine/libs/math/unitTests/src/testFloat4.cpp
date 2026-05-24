#include "emberMath.h"
#include <gtest/gtest.h>



// Constructors:
TEST(Float4, DirectionConstructor)
{
	Float4 direction = Float4::Direction(math::pi4, math::pi4);
	EXPECT_TRUE(direction.IsEpsilonEqual(Float4(0.5f, 0.5f, math::sqrt2Inv, 0.0f)));
}

// Math operations:
TEST(Float4, LengthSq)
{
	Float4 a(1.0f, 2.0f, 3.0f, 4.0f);
	float lengthSq = a.LengthSq();
	EXPECT_TRUE(math::IsEpsilonEqual(lengthSq, 30.0f));
}
TEST(Float4, Length)
{
	Float4 a(1.0f, 2.0f, 3.0f, 4.0f);
	float length = a.Length();
	EXPECT_TRUE(math::IsEpsilonEqual(length, math::Sqrt(30.0f)));
}
TEST(Float4, IsEpsilonZero)
{
	Float4 a = Float4(0.9f * math::absEpsilon, -0.9f * math::absEpsilon, 0.9f * math::absEpsilon, -0.9f * math::absEpsilon);
	EXPECT_TRUE(a.IsEpsilonZero());
}

// Static math operations:
TEST(Float4, Abs)
{
	Float4 a(-1.0f, -2.0f, -3.0f, 4.0f);
	Float4 abs = Float4::Abs(a);
	EXPECT_TRUE(abs.IsEpsilonEqual(Float4(1.0f, 2.0f, 3.0f, 4.0f)));
}
TEST(Float4, Min)
{
	Float4 a(1.0f, 2.0f, 4.0f, 1.0f);
	Float4 b(3.0f, 4.0f, 1.0f, 2.0f);
	Float4 min = Float4::Min(a, b);
	EXPECT_TRUE(min.IsEpsilonEqual(Float4(1.0f, 2.0f, 1.0f, 1.0f)));
}
TEST(Float4, Max)
{
	Float4 a(1.0f, 2.0f, 4.0f, 1.0f);
	Float4 b(3.0f, 4.0f, 1.0f, 2.0f);
	Float4 max = Float4::Max(a, b);
	EXPECT_TRUE(max.IsEpsilonEqual(Float4(3.0f, 4.0f, 4.0f, 2.0f)));
}
TEST(Float4, Clamp)
{
	Float4 value(1.0f, 5.0f, 2.0f, 1.0f);
	Float4 min(2.0f, 3.0f, 1.0f, -2.0f);
	Float4 max(3.0f, 4.0f, 3.0f, -1.0f);
	Float4 clamped = Float4::Clamp(value, min, max);
	EXPECT_TRUE(clamped.IsEpsilonEqual(Float4(2.0f, 4.0f, 2.0f, -1.0f)));
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
	EXPECT_TRUE(xyz.IsEpsilonEqual(Float3(1.0f, 2.0f, 3.0f)));
}

// Assignment:
TEST(Float4, OperatorAssignment)
{
	Float4 a(1.0f, 2.0f, 3.0f, 4.0f);
	Float4 b(5.0f, 6.0f, 7.0f, 8.0f);
	a = b;
	EXPECT_TRUE(a == b);
}

// Addition:
TEST(Float4, OperatorAddition)
{
	Float4 a(1.0f, 2.0f, 3.0f, 4.0f);
	Float4 b(5.0f, 6.0f, 7.0f, 8.0f);
	Float4 add = a + b;
	EXPECT_TRUE(add.IsEpsilonEqual(Float4(6.0f, 8.0f, 10.0f, 12.0f)));
}
TEST(Float4, OperatorAdditionAssignment)
{
	Float4 add(1.0f, 2.0f, 3.0f, 4.0f);
	Float4 b(5.0f, 6.0f, 7.0f, 8.0f);
	add +=b;
	EXPECT_TRUE(add.IsEpsilonEqual(Float4(6.0f, 8.0f, 10.0f, 12.0f)));
}

// Substraction:
TEST(Float4, OperatorSubstraction)
{
	Float4 a(1.0f, 2.0f, 3.0f, 4.0f);
	Float4 b(5.0f, 6.0f, 7.0f, 8.0f);
	Float4 sub = a - b;
	EXPECT_TRUE(sub.IsEpsilonEqual(Float4(-4.0f, -4.0f, -4.0f, -4.0f)));
}
TEST(Float4, OperatorSubstractionAssignment)
{
	Float4 sub(1.0f, 2.0f, 3.0f, 4.0f);
	Float4 b(5.0f, 6.0f, 7.0f, 8.0f);
	sub -= b;
	EXPECT_TRUE(sub.IsEpsilonEqual(Float4(-4.0f, -4.0f, -4.0f, -4.0f)));
}
TEST(Float4, OperatorNegation)
{
	Float4 a(1.0f, 2.0f, 3.0f, 4.0f);
	Float4 negation = -a;
	EXPECT_TRUE(negation.IsEpsilonEqual(Float4(-1.0f, -2.0f, -3.0f, -4.0f)));
}

// Multiplication:
TEST(Float4, OperatorMultiplication)
{
	Float4 a(1.0f, 2.0f, 3.0f, 4.0f);
	Float4 b(5.0f, 6.0f, 7.0f, 8.0f);
	Float4 mult = a * b;
	EXPECT_TRUE(mult.IsEpsilonEqual(Float4(5.0f, 12.0f, 21.0f, 32.0f)));
}
TEST(Float4, OperatorMultiplicationAssignment)
{
	Float4 mult(1.0f, 2.0f, 3.0f, 4.0f);
	Float4 b(5.0f, 6.0f, 7.0f, 8.0f);
	mult *=b;
	EXPECT_TRUE(mult.IsEpsilonEqual(Float4(5.0f, 12.0f, 21.0f, 32.0f)));
}
TEST(Float4, OperatorMultiplicationScalarAssignment)
{
	Float4 mult(1.0f, 2.0f, 3.0f, 4.0f);
	float b = 3.0f;
	mult *= b;
	EXPECT_TRUE(mult.IsEpsilonEqual(Float4(3.0f, 6.0f, 9.0f, 12.0f)));
}

// Division:
TEST(Float4, OperatorDivision)
{
	Float4 a(1.0f, 2.0f, 3.0f, 4.0f);
	Float4 b(5.0f, 6.0f, 7.0f, 8.0f);
	Float4 div = a / b;
	EXPECT_TRUE(div.IsEpsilonEqual(Float4(1.0f / 5.0f, 2.0f / 6.0f, 3.0f / 7.0f, 4.0f / 8.0f)));
}
TEST(Float4, OperatorDivisionAssignment)
{
	Float4 div(1.0f, 2.0f, 3.0f, 4.0f);
	Float4 b(5.0f, 6.0f, 7.0f, 8.0f);
	div /= b;
	EXPECT_TRUE(div.IsEpsilonEqual(Float4(1.0f / 5.0f, 2.0f / 6.0f, 3.0f / 7.0f, 4.0f / 8.0f)));
}
TEST(Float4, OperatorDivisionScalarAssignment)
{
	Float4 div(1.0f, 2.0f, 3.0f, 4.0f);
	float b = 4.0f;
	div /= b;
	EXPECT_TRUE(div.IsEpsilonEqual(Float4(1.0f / 4.0f, 2.0f / 4.0f, 3.0f / 4.0f, 1.0f)));
}

// Comparison:
TEST(Float4, IsEpsilonEqual)
{
    float e = 1e-6f;
	Float4 a(1.0f, 2.0f, 3.0f, 4.0f);
	Float4 b(1.0f + e, 2.0f - e, 3.0f + e, 4.0 - e);
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
TEST(Float4, OperatorSmaller)
{
	Float4 a(1.0f, 2.0f, 3.0f, 4.0f);
	Float4 b(5.0f, 6.0f, 7.0f, 8.0f);
	EXPECT_TRUE(a < b);
}
TEST(Float4, OperatorSmallerEqual)
{
	Float4 a(1.0f, 2.0f, 3.0f, 4.0f);
	Float4 b(1.0f, 2.0f, 3.0f, 8.0f);
	EXPECT_TRUE(a <= b);
}
TEST(Float4, OperatorBigger)
{
	Float4 a(5.0f, 6.0f, 7.0f, 8.0f);
	Float4 b(1.0f, 2.0f, 3.0f, 4.0f);
	EXPECT_TRUE(a > b);
}
TEST(Float4, OperatorBiggerEqual)
{
	Float4 a(1.0f, 2.0f, 3.0f, 8.0f);
	Float4 b(1.0f, 2.0f, 3.0f, 4.0f);
	EXPECT_TRUE(a >= b);
}

// Friend functions:
TEST(Float4, OperatorMultiplicationScalarLeft)
{
	Float4 a(1.0f, 2.0f, 3.0f, 4.0f);
	float b = 3.0f;
	Float4 mult = b * a;
	EXPECT_TRUE(mult.IsEpsilonEqual(Float4(3.0f, 6.0f, 9.0f, 12.0f)));
}
TEST(Float4, OperatorMultiplicationScalarRight)
{
	Float4 a(1.0f, 2.0f, 3.0f, 4.0f);
	float b = 3.0f;
	Float4 mult = a * b;
	EXPECT_TRUE(mult.IsEpsilonEqual(Float4(3.0f, 6.0f, 9.0f, 12.0f)));
}
TEST(Float4, OperatorDivisionScalarLeft)
{
	Float4 a(1.0f, 2.0f, 3.0f, 4.0f);
	float b = 4.0f;
	Float4 div = b / a;
	EXPECT_TRUE(div.IsEpsilonEqual(Float4(4.0f / 1.0f, 4.0f / 2.0f, 4.0f / 3.0f, 4.0f / 4.0f)));
}
TEST(Float4, OperatorDivisionScalarRight)
{
	Float4 a(1.0f, 2.0f, 3.0f, 4.0f);
	float b = 4.0f;
	Float4 div = a / b;
	EXPECT_TRUE(div.IsEpsilonEqual(Float4(1.0f / 4.0f, 2.0f / 4.0f, 3.0f / 4.0f, 4.0f / 4.0f)));
}