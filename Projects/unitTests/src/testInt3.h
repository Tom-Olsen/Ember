#ifndef __INCLUDE_GUARD_testInt3_h__
#define __INCLUDE_GUARD_testInt3_h__



// Access:
TEST(Int3, OperatorBrackets)
{
	Int3 a;
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
TEST(Int3, OperatorAssignment)
{
	Int3 a(1, 2, 3);
	Int3 b(3, 4, 5);
	a = b;
	EXPECT_TRUE(a == b);
}

// Addition:
TEST(Int3, OperatorAddition)
{
	Int3 a(1, 2, 3);
	Int3 b(3, 4, 5);
	Int3 add = a + b;
	EXPECT_TRUE(add == Int3(4, 6, 8));
}
TEST(Int3, OperatorAdditionAssignment)
{
	Int3 add(1, 2, 3);
	Int3 b(3, 4, 5);
	add += b;
	EXPECT_TRUE(add == Int3(4, 6, 8));
}

// Substraction:
TEST(Int3, OperatorSubstraction)
{
	Int3 a(1, 2, 3);
	Int3 b(3, 4, 5);
	Int3 sub = a - b;
	EXPECT_TRUE(sub == Int3(-2, -2, -2));
}
TEST(Int3, OperatorSubstractionAssignment)
{
	Int3 sub(1, 2, 3);
	Int3 b(3, 4, 5);
	sub -= b;
	EXPECT_TRUE(sub == Int3(-2, -2, -2));
}
TEST(Int3, OperatorNegation)
{
	Int3 a(1, 2, 3);
	Int3 negation = -a;
	EXPECT_TRUE(negation == Int3(-1, -2, -3));
}

// Multiplication:
TEST(Int3, OperatorMultiplication)
{
	Int3 a(1, 2, 3);
	Int3 b(3, 4, 5);
	Int3 mult = a * b;
	EXPECT_TRUE(mult == Int3(3, 8, 15));
}
TEST(Int3, OperatorMultiplicationAssignment)
{
	Int3 mult(1, 2, 3);
	Int3 b(3, 4, 5);
	mult *= b;
	EXPECT_TRUE(mult == Int3(3, 8, 15));
}
TEST(Int3, OperatorMultiplicationScalarAssignment)
{
	Int3 mult(1, 2, 3);
	float b = 3;
	mult *= b;
	EXPECT_TRUE(mult == Int3(3, 6, 9));
}


// Comparison:
TEST(Int3, OperatorEqual)
{
	Int3 a(1, 2, 3);
	Int3 b(1, 2, 3);
	EXPECT_TRUE(a == b);
}
TEST(Int3, OperatorNotEqual)
{
	Int3 a(1, 2, 3);
	Int3 b(1, 2, 4);
	EXPECT_TRUE(a != b);
}

// Friend functions:
TEST(Int3, OperatorMultiplicationScalarLeft)
{
	Int3 a(1, 2, 3);
	float b = 3;
	Int3 mult = b * a;
	EXPECT_NEAR2(mult, Int3(3, 6, 9), epsilon);
}
TEST(Int3, OperatorMultiplicationScalarRight)
{
	Int3 a(1, 2, 3);
	float b = 3;
	Int3 mult = a * b;
	EXPECT_NEAR2(mult, Int3(3, 6, 9), epsilon);
}



#endif // __INCLUDE_GUARD_testInt3_h__