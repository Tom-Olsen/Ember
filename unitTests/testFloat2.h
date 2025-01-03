#ifndef __INCLUDE_GUARD_testFloat2_h__
#define __INCLUDE_GUARD_testFloat2_h__



// Constructors:
TEST(Float2, DirectionConstructor)
{
	Float2 direction = Float2::Direction(mathf::PI_4);
	EXPECT_NEAR2(direction, mathf::SQRT2_INV * Float2::one, epsilon);
}

// Math operations:
TEST(Float2, LengthSq)
{
	Float2 a(3.0f, 4.0f);
	float lengthSq = a.LengthSq();
	EXPECT_NEAR(lengthSq, 25.0f, epsilon);
}
TEST(Float2, Length)
{
	Float2 a(3.0f, 4.0f);
	float length = a.Length();
	EXPECT_NEAR(length, 5.0f, epsilon);
}
TEST(Float2, Angle)
{
	Float2 a(-5.0f, -5.0f);
	float angle = a.Angle();
	EXPECT_NEAR(angle, -3.0f / 4.0f * mathf::PI, epsilon);
}
TEST(Float2, Normalize)
{
	Float2 a(3.0f, 4.0f);
	Float2 normalized = a.Normalize();
	EXPECT_NEAR2(normalized, Float2(0.6f, 0.8f), epsilon);
}
TEST(Float2, Rotate)
{
	Float2 a(1.0f, 0.0f);
	Float2 rotated = a.Rotate(mathf::PI_4);
	EXPECT_NEAR2(rotated, Float2(mathf::SQRT2_INV, mathf::SQRT2_INV), epsilon);
}
TEST(Float2, IsEpsilonZero)
{
	Float2 a(0.9f * mathf::EPSILON, -0.9f * mathf::EPSILON);
	EXPECT_TRUE(a.IsEpsilonZero());
}

// Static math operations:
TEST(Float2, Abs)
{
	Float2 a(-1.0f, -2.0f);
	Float2 abs = Float2::Abs(a);
	EXPECT_NEAR2(abs, Float2(1.0f, 2.0f), epsilon);
}
TEST(Float2, Dot)
{
	Float2 a(1.0f, 2.0f);
	Float2 b(3.0f, 4.0f);
	float dot = Float2::Dot(a, b);
	EXPECT_NEAR(dot, 11.0f, epsilon);
}
TEST(Float2, Cross)
{
	Float2 a(1.0f, 2.0f);
	Float2 b(3.0f, 4.0f);
	float cross = Float2::Cross(a, b);
	EXPECT_NEAR(cross, 2.0f, epsilon);
}
TEST(Float2, DistanceSq)
{
	Float2 a(1.0f, 2.0f);
	Float2 b(3.0f, 4.0f);
	float distanceSq = Float2::DistanceSq(a, b);
	EXPECT_NEAR(distanceSq, 8.0f, epsilon);
}
TEST(Float2, Distance)
{
	Float2 a(1.0f, 2.0f);
	Float2 b(3.0f, 4.0f);
	float distance = Float2::Distance(a, b);
	EXPECT_NEAR(distance, mathf::Sqrt(8.0f), epsilon);
}
TEST(Float2, static_Angle)
{
	Float2 a(2.0f, 1.0f);
	Float2 b(-2.0f, 4.0f);
	float angle = Float2::Angle(a, b);
	EXPECT_NEAR(angle, mathf::PI_2, epsilon);
}
TEST(Float2, Min)
{
	Float2 a(1.0f, 2.0f);
	Float2 b(3.0f, 4.0f);
	Float2 min = Float2::Min(a, b);
	EXPECT_NEAR2(min, Float2(1.0f, 2.0f), epsilon);
}
TEST(Float2, Max)
{
	Float2 a(1.0f, 2.0f);
	Float2 b(3.0f, 4.0f);
	Float2 max = Float2::Max(a, b);
	EXPECT_NEAR2(max, Float2(3.0f, 4.0f), epsilon);
}
TEST(Float2, Clamp)
{
	Float2 value(1.0f, 5.0f);
	Float2 min(2.0f, 3.0f);
	Float2 max(3.0f, 4.0f);
	Float2 clamped = Float2::Clamp(value, min, max);
	EXPECT_NEAR2(clamped, Float2(2.0f, 4.0f), epsilon);
}

// Access:
TEST(Float2, OperatorBrackets)
{
	Float2 a;
	a[0] = 1;
	a[1] = 2;
	EXPECT_FLOAT_EQ(a.x, 1.0f);
	EXPECT_FLOAT_EQ(a.y, 2.0f);
	EXPECT_FLOAT_EQ(a[0], 1.0f);
	EXPECT_FLOAT_EQ(a[1], 2.0f);
}

// Assignment:
TEST(Float2, OperatorAssignment)
{
	Float2 a(1.0f, 2.0f);
	Float2 b(3.0f, 4.0f);
	a = b;
	EXPECT_FLOAT2_EQ(a, b);
}

// Addition:
TEST(Float2, OperatorAddition)
{
	Float2 a(1.0f, 2.0f);
	Float2 b(3.0f, 4.0f);
	Float2 add = a + b;
	EXPECT_NEAR2(add, Float2(4.0f, 6.0f), epsilon);
}
TEST(Float2, OperatorAdditionAssignment)
{
	Float2 add(1.0f, 2.0f);
	Float2 b(3.0f, 4.0f);
	add += b;
	EXPECT_NEAR2(add, Float2(4.0f, 6.0f), epsilon);
}

// Substraction:
TEST(Float2, OperatorSubstraction)
{
	Float2 a(1.0f, 2.0f);
	Float2 b(3.0f, 4.0f);
	Float2 sub = a - b;
	EXPECT_NEAR2(sub, Float2(-2.0f, -2.0f), epsilon);
}
TEST(Float2, OperatorSubstractionAssignment)
{
	Float2 sub(1.0f, 2.0f);
	Float2 b(3.0f, 4.0f);
	sub -= b;
	EXPECT_NEAR2(sub, Float2(-2.0f, -2.0f), epsilon);
}
TEST(Float2, OperatorNegation)
{
	Float2 a(1.0f, 2.0f);
	Float2 negation = -a;
	EXPECT_NEAR2(negation, Float2(-1.0f, -2.0f), epsilon);
}

// Multiplication:
TEST(Float2, OperatorMultiplication)
{
	Float2 a(1.0f, 2.0f);
	Float2 b(3.0f, 4.0f);
	Float2 mult = a * b;
	EXPECT_NEAR2(mult, Float2(3.0f, 8.0f), epsilon);
}
TEST(Float2, OperatorMultiplicationAssignment)
{
	Float2 mult(1.0f, 2.0f);
	Float2 b(3.0f, 4.0f);
	mult *= b;
	EXPECT_NEAR2(mult, Float2(3.0f, 8.0f), epsilon);
}
TEST(Float2, OperatorMultiplicationScalarAssignment)
{
	Float2 mult(1.0f, 2.0f);
	float b = 3.0f;
	mult *= b;
	EXPECT_NEAR2(mult, Float2(3.0f, 6.0f), epsilon);
}

// Division:
TEST(Float2, OperatorDivision)
{
	Float2 a(1.0f, 2.0f);
	Float2 b(3.0f, 4.0f);
	Float2 div = a / b;
	EXPECT_NEAR2(div, Float2(1.0f / 3.0f, 2.0f / 4.0f), epsilon);
}
TEST(Float2, OperatorDivisionAssignment)
{
	Float2 div(1.0f, 2.0f);
	Float2 b(3.0f, 4.0f);
	div /= b;
	EXPECT_NEAR2(div, Float2(1.0f / 3.0f, 2.0f / 4.0f), epsilon);
}
TEST(Float2, OperatorDivisionScalar)
{
	Float2 a(1.0f, 2.0f);
	float b = 3.0f;
	Float2 div = a / b;
	EXPECT_NEAR2(div, Float2(1.0f / 3.0f, 2.0f / 3.0f), epsilon);
}
TEST(Float2, OperatorDivisionScalarAssignment)
{
	Float2 div(1.0f, 2.0f);
	float b = 3.0f;
	div /= b;
	EXPECT_NEAR2(div, Float2(1.0f / 3.0f, 2.0f / 3.0f), epsilon);
}

// Comparison:
TEST(Float2, IsEpsilonEqual)
{
	Float2 a(1.0f, 2.0f);
	Float2 b(1.0f + epsilon, 2.0f - epsilon);
	EXPECT_TRUE(a.IsEpsilonEqual(b));
}
TEST(Float2, OperatorEqual)
{
	Float2 a(1.0f, 2.0f);
	Float2 b(1.0f, 2.0f);
	EXPECT_TRUE(a == b);
}
TEST(Float2, OperatorNotEqual)
{
	Float2 a(1.0f, 2.0f);
	Float2 b(1.0f, 3.0f);
	EXPECT_TRUE(a != b);
}

// Friend functions:
TEST(Float2, OperatorMultiplicationScalarLeft)
{
	Float2 a(1.0f, 2.0f);
	float b = 3.0f;
	Float2 mult = b * a;
	EXPECT_NEAR2(mult, Float2(3.0f, 6.0f), epsilon);
}
TEST(Float2, OperatorMultiplicationScalarRight)
{
	Float2 a(1.0f, 2.0f);
	float b = 3.0f;
	Float2 mult = a * b;
	EXPECT_NEAR2(mult, Float2(3.0f, 6.0f), epsilon);
}



#endif // __INCLUDE_GUARD_testFloat2_h__