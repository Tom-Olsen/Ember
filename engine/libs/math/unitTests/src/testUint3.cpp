#include "emberMath.h"
#include "macros.h"
#include <gtest/gtest.h>



// Access:
TEST(Uint3, OperatorBrackets)
{
	Uint3 a;
	a[0] = 1;
	a[1] = 2;
	a[2] = 3;
	EXPECT_TRUE(a.x == 1);
	EXPECT_TRUE(a.y == 2);
	EXPECT_TRUE(a.z == 3);
	EXPECT_TRUE(a[0] == 1);
	EXPECT_TRUE(a[1] == 2);
	EXPECT_TRUE(a[2] == 3);
}


// Assignment:
TEST(Uint3, OperatorAssignment)
{
	Uint3 a(1, 2, 3);
	Uint3 b(3, 4, 5);
	a = b;
	EXPECT_TRUE(a == b);
}

// Addition:
TEST(Uint3, OperatorAddition)
{
	Uint3 a(1, 2, 3);
	Uint3 b(3, 4, 5);
	Uint3 add = a + b;
	EXPECT_TRUE(add == Uint3(4, 6, 8));
}
TEST(Uint3, OperatorAdditionAssignment)
{
	Uint3 add(1, 2, 3);
	Uint3 b(3, 4, 5);
	add += b;
	EXPECT_TRUE(add == Uint3(4, 6, 8));
}

// Substraction:
TEST(Uint3, OperatorSubstraction)
{
	Uint3 a(3, 4, 5);
	Uint3 b(1, 2, 3);
	Uint3 sub = a - b;
	EXPECT_TRUE(sub == Uint3(2, 2, 2));
}
TEST(Uint3, OperatorSubstractionAssignment)
{
	Uint3 sub(3, 4, 5);
	Uint3 b(1, 2, 3);
	sub -= b;
	EXPECT_TRUE(sub == Uint3(2, 2, 2));
}


// Comparison:
TEST(Uint3, OperatorEqual)
{
	Uint3 a(1, 2, 3);
	Uint3 b(1, 2, 3);
	EXPECT_TRUE(a == b);
}
TEST(Uint3, OperatorNotEqual)
{
	Uint3 a(1, 2, 3);
	Uint3 b(1, 2, 4);
	EXPECT_TRUE(a != b);
}