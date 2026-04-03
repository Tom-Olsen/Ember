#include "emberMath.h"
#include "unitTestHelper.h"
#include <gtest/gtest.h>



// Static math operations:
TEST(Uint2, Min)
{
	Uint2 a(1, 2);
	Uint2 b(3, 4);
	Uint2 min = Uint2::Min(a, b);
	ExpectNearVec(min, Uint2(1, 2), epsilon);
}
TEST(Uint2, Max)
{
	Uint2 a(1, 2);
	Uint2 b(3, 4);
	Uint2 max = Uint2::Max(a, b);
	ExpectNearVec(max, Uint2(3, 4), epsilon);
}
TEST(Uint2, Clamp)
{
	Uint2 value(1, 5);
	Uint2 min(2, 3);
	Uint2 max(3, 4);
	Uint2 clamped = Uint2::Clamp(value, min, max);
	ExpectNearVec(clamped, Uint2(2, 4), epsilon);
}

// Access:
TEST(Uint2, OperatorBrackets)
{
	Uint2 a;
	a[0] = 1;
	a[1] = 2;
	EXPECT_TRUE(a.x == 1);
	EXPECT_TRUE(a.y == 2);
	EXPECT_TRUE(a[0] == 1);
	EXPECT_TRUE(a[1] == 2);
}

// Assignment:
TEST(Uint2, OperatorAssignment)
{
	Uint2 a(1, 2);
	Uint2 b(3, 4);
	a = b;
	EXPECT_TRUE(a == b);
}

// Addition:
TEST(Uint2, OperatorAddition)
{
	Uint2 a(1, 2);
	Uint2 b(3, 4);
	Uint2 add = a + b;
	EXPECT_TRUE(add == Uint2(4, 6));
}
TEST(Uint2, OperatorAdditionAssignment)
{
	Uint2 add(1, 2);
	Uint2 b(3, 4);
	add += b;
	EXPECT_TRUE(add == Uint2(4, 6));
}

// Substraction:
TEST(Uint2, OperatorSubstraction)
{
	Uint2 a(3, 4);
	Uint2 b(1, 2);
	Uint2 sub = a - b;
	EXPECT_TRUE(sub == Uint2(2, 2));
}
TEST(Uint2, OperatorSubstractionAssignment)
{
	Uint2 sub(3, 4);
	Uint2 b(1, 2);
	sub -= b;
	EXPECT_TRUE(sub == Uint2(2, 2));
}


// Comparison:
TEST(Uint2, OperatorEqual)
{
	Uint2 a(1, 2);
	Uint2 b(1, 2);
	EXPECT_TRUE(a == b);
}
TEST(Uint2, OperatorNotEqual)
{
	Uint2 a(1, 2);
	Uint2 b(1, 3);
	EXPECT_TRUE(a != b);
}
TEST(Uint2, OperatorSmaller)
{
	Uint2 a(1, 2);
	Uint2 b(4, 5);
	EXPECT_TRUE(a < b);
}
TEST(Uint2, OperatorSmallerEqual)
{
	Uint2 a(1, 2);
	Uint2 b(1, 2);
	EXPECT_TRUE(a <= b);
}
TEST(Uint2, OperatorBigger)
{
	Uint2 a(4, 5);
	Uint2 b(1, 2);
	EXPECT_TRUE(a > b);
}
TEST(Uint2, OperatorBiggerEqual)
{
	Uint2 a(1, 2);
	Uint2 b(1, 2);
	EXPECT_TRUE(a >= b);
}