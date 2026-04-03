#include "emberMath.h"
#include "unitTestHelper.h"
#include <gtest/gtest.h>



// Static math operations:
TEST(Int2, Min)
{
	Int2 a(1, 2);
	Int2 b(3, 4);
	Int2 min = Int2::Min(a, b);
	ExpectNearVec(min, Int2(1, 2), epsilon);
}
TEST(Int2, Max)
{
	Int2 a(1, 2);
	Int2 b(3, 4);
	Int2 max = Int2::Max(a, b);
	ExpectNearVec(max, Int2(3, 4), epsilon);
}
TEST(Int2, Clamp)
{
	Int2 value(1, 5);
	Int2 min(2, 3);
	Int2 max(3, 4);
	Int2 clamped = Int2::Clamp(value, min, max);
	ExpectNearVec(clamped, Int2(2, 4), epsilon);
}

// Access:
TEST(Int2, OperatorBrackets)
{
	Int2 a;
	a[0] = 1;
	a[1] = 2;
	EXPECT_TRUE(a.x == 1);
	EXPECT_TRUE(a.y == 2);
	EXPECT_TRUE(a[0] == 1);
	EXPECT_TRUE(a[1] == 2);
}


// Assignment:
TEST(Int2, OperatorAssignment)
{
	Int2 a(1, 2);
	Int2 b(3, 4);
	a = b;
	EXPECT_TRUE(a == b);
}

// Addition:
TEST(Int2, OperatorAddition)
{
	Int2 a(1, 2);
	Int2 b(3, 4);
	Int2 add = a + b;
	EXPECT_TRUE(add == Int2(4, 6));
}
TEST(Int2, OperatorAdditionAssignment)
{
	Int2 add(1, 2);
	Int2 b(3, 4);
	add += b;
	EXPECT_TRUE(add == Int2(4, 6));
}

// Substraction:
TEST(Int2, OperatorSubstraction)
{
	Int2 a(1, 2);
	Int2 b(3, 4);
	Int2 sub = a - b;
	EXPECT_TRUE(sub == Int2(-2, -2));
}
TEST(Int2, OperatorSubstractionAssignment)
{
	Int2 sub(1, 2);
	Int2 b(3, 4);
	sub -= b;
	EXPECT_TRUE(sub == Int2(-2, -2));
}
TEST(Int2, OperatorNegation)
{
	Int2 a(1, 2);
	Int2 negation = -a;
	EXPECT_TRUE(negation == Int2(-1, -2));
}

// Multiplication:
TEST(Int2, OperatorMultiplication)
{
	Int2 a(1, 2);
	Int2 b(3, 4);
	Int2 mult = a * b;
	EXPECT_TRUE(mult == Int2(3, 8));
}
TEST(Int2, OperatorMultiplicationAssignment)
{
	Int2 mult(1, 2);
	Int2 b(3, 4);
	mult *= b;
	EXPECT_TRUE(mult == Int2(3, 8));
}
TEST(Int2, OperatorMultiplicationScalarAssignment)
{
	Int2 mult(1, 2);
	float b = 3;
	mult *= b;
	EXPECT_TRUE(mult == Int2(3, 6));
}


// Comparison:
TEST(Int2, OperatorEqual)
{
	Int2 a(1, 2);
	Int2 b(1, 2);
	EXPECT_TRUE(a == b);
}
TEST(Int2, OperatorNotEqual)
{
	Int2 a(1, 2);
	Int2 b(1, 3);
	EXPECT_TRUE(a != b);
}
TEST(Int2, OperatorSmaller)
{
	Int2 a(1, 2);
	Int2 b(3, 4);
	EXPECT_TRUE(a < b);
}
TEST(Int2, OperatorSmallerEqual)
{
	Int2 a(1, 2);
	Int2 b(1, 4);
	EXPECT_TRUE(a <= b);
}
TEST(Int2, OperatorBigger)
{
	Int2 a(3, 4);
	Int2 b(1, 2);
	EXPECT_TRUE(a > b);
}
TEST(Int2, OperatorBiggerEqual)
{
	Int2 a(1, 4);
	Int2 b(1, 2);
	EXPECT_TRUE(a >= b);
}

// Friend functions:
TEST(Int2, OperatorMultiplicationScalarLeft)
{
	Int2 a(1, 2);
	float b = 3;
	Int2 mult = b * a;
	ExpectNearVec(mult, Int2(3, 6), epsilon);
}
TEST(Int2, OperatorMultiplicationScalarRight)
{
	Int2 a(1, 2);
	float b = 3;
	Int2 mult = a * b;
	ExpectNearVec(mult, Int2(3, 6), epsilon);
}