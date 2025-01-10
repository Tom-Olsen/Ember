#ifndef __INCLUDE_GUARD_testFloat3_h__
#define __INCLUDE_GUARD_testFloat3_h__



// Constructors:
TEST(Float3, DirectionConstructor)
{
	Float3 direction = Float3::Direction(mathf::pi4, mathf::pi4);
	EXPECT_NEAR3(direction, Float3(0.5f, 0.5f, mathf::sqrt2Inv), epsilon);
}

// Math operations:
TEST(Float3, LengthSq)
{
	Float3 a(3.0f, 4.0f, 12.0f);
	float lengthSq = a.LengthSq();
	EXPECT_NEAR(lengthSq, 169.0f, epsilon);
}
TEST(Float3, Length)
{
	Float3 a(3.0f, 4.0f, 12.0f);
	float length = a.Length();
	EXPECT_NEAR(length, 13.0f, epsilon);
}
TEST(Float3, Theta)
{
	Float3 a(1.0f, 1.0f, mathf::sqrt2);
	float theta = a.Theta();
	EXPECT_NEAR(theta, mathf::pi4, epsilon);
}
TEST(Float3, Phi)
{
	Float3 a = Float3::one;
	float phi = a.Phi();
	EXPECT_NEAR(phi, mathf::pi4, epsilon);
}
TEST(Float3, Angles)
{
	Float3 a(1.0f, 1.0f, mathf::sqrt2);
	Float2 angles = a.Angles();
	EXPECT_NEAR2(angles, mathf::pi4 * Float2::one, epsilon);
}
TEST(Float3, Normalize)
{
	Float3 a(3.0f, 4.0f, 12.0f);
	Float3 normalized = a.Normalize();
	EXPECT_NEAR3(normalized, Float3(3.0f / 13.0f, 4.0f / 13.0f, 12.0f / 13.0f), epsilon);
}
TEST(Float3, Rotate)
{
	Float3 a(1.0f, 1.0f, mathf::sqrt2);
	Float3 rotated = a.Rotate(mathf::pi4, mathf::pi4);
	EXPECT_NEAR3(rotated, Float3(0.0f, 2.0f, 0.0f), epsilon);
}
TEST(Float3, IsEpsilonZero)
{
	Float3 a = Float3(0.9f * mathf::epsilon, -0.9f * mathf::epsilon, 0.0f);
	EXPECT_TRUE(a.IsEpsilonZero());
}

// Static math operations:
TEST(Float3, Abs)
{
	Float3 a(-1.0f, -2.0f, -3.0f);
	Float3 abs = Float3::Abs(a);
	EXPECT_NEAR3(abs, Float3(1.0f, 2.0f, 3.0f), epsilon);
}
TEST(Float3, Dot)
{
	Float3 a(1.0f, 3.0f, 4.0f);
	Float3 b(2.0f, 6.0f, 5.0f);
	float dot = Float3::Dot(a, b);
	EXPECT_NEAR(dot, 40.0f, epsilon);
}
TEST(Float3, Cross)
{
	Float3 a(1.0f, 3.0f, 4.0f);
	Float3 b(2.0f, 6.0f, 5.0f);
	Float3 cross = Float3::Cross(a, b);
	EXPECT_NEAR3(cross, Float3(-9.0f, 3.0f, 0.0f), epsilon);
}
TEST(Float3, DistanceSq)
{
	Float3 a(1.0f, 3.0f, 4.0f);
	Float3 b(2.0f, 6.0f, 5.0f);
	float distanceSq = Float3::DistanceSq(a, b);
	EXPECT_NEAR(distanceSq, 11.0f, epsilon);
}
TEST(Float3, Distance)
{
	Float3 a(1.0f, 3.0f, 4.0f);
	Float3 b(2.0f, 6.0f, 5.0f);
	float distance = Float3::Distance(a, b);
	EXPECT_NEAR(distance, mathf::Sqrt(11.0f), epsilon);
}
TEST(Float3, static_Angle)
{
	Float3 a(2.0f, 0.0f, 0.0f);
	Float3 b(-2.0f, 2.0f, 0.0f);
	float angle = Float3::Angle(a, b);
	EXPECT_NEAR(angle, mathf::deg2rad * 135.0f, epsilon);
}
TEST(Float3, Min)
{
	Float3 a(1.0f, 2.0f, 4.0f);
	Float3 b(3.0f, 4.0f, 1.0f);
	Float3 min = Float3::Min(a, b);
	EXPECT_NEAR3(min, Float3(1.0f, 2.0f, 1.0f), epsilon);
}
TEST(Float3, Max)
{
	Float3 a(1.0f, 2.0f, 4.0f);
	Float3 b(3.0f, 4.0f, 1.0f);
	Float3 max = Float3::Max(a, b);
	EXPECT_NEAR3(max, Float3(3.0f, 4.0f, 4.0f), epsilon);
}
TEST(Float3, Clamp)
{
	Float3 value(1.0f, 5.0f, 2.0f);
	Float3 min(2.0f, 3.0f, 1.0f);
	Float3 max(3.0f, 4.0f, 3.0f);
	Float3 clamped = Float3::Clamp(value, min, max);
	EXPECT_NEAR3(clamped, Float3(2.0f, 4.0f, 2.0f), epsilon);
}

// Access:
TEST(Float3, OperatorBrackets)
{
	Float3 a;
	a[0] = 1.0f;
	a[1] = 2.0f;
	a[2] = 3.0f;
	EXPECT_FLOAT_EQ(a.x, 1.0f);
	EXPECT_FLOAT_EQ(a.y, 2.0f);
	EXPECT_FLOAT_EQ(a.z, 3.0f);
	EXPECT_FLOAT_EQ(a[0], 1.0f);
	EXPECT_FLOAT_EQ(a[1], 2.0f);
	EXPECT_FLOAT_EQ(a[2], 3.0f);
}

// Assignment:
TEST(Float3, OperatorAssignment)
{
	Float3 a(1.0f, 2.0f, 3.0f);
	Float3 b(3.0f, 4.0f, 5.0f);
	a = b;
	EXPECT_FLOAT3_EQ(a, b);
}

// Addition:
TEST(Float3, OperatorAddition)
{
	Float3 a(1.0f, 2.0f, 3.0f);
	Float3 b(4.0f, 5.0f, 6.0f);
	Float3 add = a + b;
	EXPECT_NEAR3(add, Float3(5.0f, 7.0f, 9.0f), epsilon);
}
TEST(Float3, OperatorAdditionAssignment)
{
	Float3 add(1.0f, 2.0f, 3.0f);
	Float3 b(4.0f, 5.0f, 6.0f);
	add += b;
	EXPECT_NEAR3(add, Float3(5.0f, 7.0f, 9.0f), epsilon);
}

// Substraction:
TEST(Float3, OperatorSubstraction)
{
	Float3 a(1.0f, 2.0f, 3.0f);
	Float3 b(4.0f, 5.0f, 6.0f);
	Float3 sub = a - b;
	EXPECT_NEAR3(sub, Float3(-3.0f, -3.0f, -3.0f), epsilon);
}
TEST(Float3, OperatorSubstractionAssignment)
{
	Float3 sub(1.0f, 2.0f, 3.0f);
	Float3 b(4.0f, 5.0f, 6.0f);
	sub -= b;
	EXPECT_NEAR3(sub, Float3(-3.0f, -3.0f, -3.0f), epsilon);
}
TEST(Float3, OperatorNegation)
{
	Float3 a(1.0f, 2.0f, 3.0f);
	Float3 negation = -a;
	EXPECT_NEAR3(negation, Float3(-1.0f, -2.0f, -3.0f), epsilon);
}

// Multiplication:
TEST(Float3, OperatorMultiplication)
{
	Float3 a(1.0f, 2.0f, 3.0f);
	Float3 b(4.0f, 5.0f, 6.0f);
	Float3 mult = a * b;
	EXPECT_NEAR3(mult, Float3(4.0f, 10.0f, 18.0f), epsilon);
}
TEST(Float3, OperatorMultiplicationAssignment)
{
	Float3 mult(1.0f, 2.0f, 3.0f);
	Float3 b(4.0f, 5.0f, 6.0f);
	mult *= b;
	EXPECT_NEAR3(mult, Float3(4.0f, 10.0f, 18.0f), epsilon);
}
TEST(Float3, OperatorMultiplicationScalarAssignment)
{
	Float3 mult(1.0f, 2.0f, 3.0f);
	float b = 3.0f;
	mult *= b;
	EXPECT_NEAR3(mult, Float3(3.0f, 6.0f, 9.0f), epsilon);
}

// Division:
TEST(Float3, OperatorDivision)
{
	Float3 a(1.0f, 2.0f, 3.0f);
	Float3 b(4.0f, 5.0f, 6.0f);
	Float3 div = a / b;
	EXPECT_NEAR3(div, Float3(1.0f / 4.0f, 2.0f / 5.0f, 3.0f / 6.0f), epsilon);
}
TEST(Float3, OperatorDivisionAssignment)
{
	Float3 div(1.0f, 2.0f, 3.0f);
	Float3 b(4.0f, 5.0f, 6.0f);
	div /= b;
	EXPECT_NEAR3(div, Float3(1.0f / 4.0f, 2.0f / 5.0f, 3.0f / 6.0f), epsilon);
}
TEST(Float3, OperatorDivisionScalar)
{
	Float3 a(1.0f, 2.0f, 3.0f);
	float b = 3.0f;
	Float3 div = a / b;
	EXPECT_NEAR3(div, Float3(1.0f / 3.0f, 2.0f / 3.0f, 1.0f), epsilon);
}
TEST(Float3, OperatorDivisionScalarAssignment)
{
	Float3 div(1.0f, 2.0f, 3.0f);
	float b = 3.0f;
	div /= b;
	EXPECT_NEAR3(div, Float3(1.0f / 3.0f, 2.0f / 3.0f, 1.0f), epsilon);
}

// Comparison:
TEST(Float3, IsEpsilonEqual)
{
	Float3 a(1.0f, 2.0f, 3.0f);
	Float3 b(1.0f + epsilon, 2.0f - epsilon, 3.0f);
	EXPECT_TRUE(a.IsEpsilonEqual(b));
}
TEST(Float3, OperatorEqual)
{
	Float3 a(1.0f, 2.0f, 3.0f);
	Float3 b(1.0f, 2.0f, 3.0f);
	EXPECT_TRUE(a == b);
}
TEST(Float3, OperatorNotEqual)
{
	Float3 a(1.0f, 2.0f, 3.0f);
	Float3 b(1.0f, 2.0f, 3.1f);
	EXPECT_TRUE(a != b);
}

// Friend functions:
TEST(Float3, OperatorMultiplicationScalarLeft)
{
	Float3 a(1.0f, 2.0f, 3.0f);
	float b = 3.0f;
	Float3 mult = b * a;
	EXPECT_NEAR3(mult, Float3(3.0f, 6.0f, 9.0f), epsilon);
}
TEST(Float3, OperatorMultiplicationScalarRight)
{
	Float3 a(1.0f, 2.0f, 3.0f);
	float b = 3.0f;
	Float3 mult = a * b;
	EXPECT_NEAR3(mult, Float3(3.0f, 6.0f, 9.0f), epsilon);
}



#endif // __INCLUDE_GUARD_testFloat3_h__