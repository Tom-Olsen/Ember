#ifndef __INCLUDE_GUARD_testInt2_h__
#define __INCLUDE_GUARD_testInt2_h__



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

// Friend functions:
TEST(Int2, OperatorMultiplicationScalarLeft)
{
	Int2 a(1, 2);
	float b = 3;
	Int2 mult = b * a;
	EXPECT_NEAR2(mult, Int2(3, 6), epsilon);
}
TEST(Int2, OperatorMultiplicationScalarRight)
{
	Int2 a(1, 2);
	float b = 3;
	Int2 mult = a * b;
	EXPECT_NEAR2(mult, Int2(3, 6), epsilon);
}



#endif // __INCLUDE_GUARD_testInt2_h__