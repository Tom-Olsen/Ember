#include "emberMath.h"
#include "unitTestHelper.h"
#include <gtest/gtest.h>



// Static constructors:
TEST(Float2x2, ConstructorRowsScalar)
{
	Float2 row0 = Float2(1.0f, 2.0f);
	Float2 row1 = Float2(3.0f, 4.0f);
	Float2x2 matrix = Float2x2::Rows(row0.x, row0.y, row1.x, row1.y);
	ExpectEqualVec(matrix.GetRow(0), row0);
	ExpectEqualVec(matrix.GetRow(1), row1);
}
TEST(Float2x2, ConstructorRowsVector)
{
	Float2 row0 = Float2(1.0f, 2.0f);
	Float2 row1 = Float2(3.0f, 4.0f);
	Float2x2 matrix = Float2x2::Rows(row0, row1);
	ExpectEqualVec(matrix.GetRow(0),row0);
	ExpectEqualVec(matrix.GetRow(1),row1);
}
TEST(Float2x2, ConstructorColumnsScalar)
{
	Float2 column0 = Float2(1.0f, 2.0f);
	Float2 column1 = Float2(3.0f, 4.0f);
	Float2x2 matrix = Float2x2::Columns(column0.x, column0.y, column1.x, column1.y);
	ExpectEqualVec(matrix.GetColumn(0), column0);
	ExpectEqualVec(matrix.GetColumn(1), column1);
}
TEST(Float2x2, ConstructorColumnsVector)
{
	Float2 column0 = Float2(1.0f, 2.0f);
	Float2 column1 = Float2(3.0f, 4.0f);
	Float2x2 matrix = Float2x2::Columns(column0, column1);
	ExpectEqualVec(matrix.GetColumn(0), column0);
	ExpectEqualVec(matrix.GetColumn(1), column1);
}

// Math operations:
TEST(Float2x2, Transpose)
{
	Float2 row0 = Float2(1.0f, 2.0f);
	Float2 row1 = Float2(3.0f, 4.0f);
	Float2x2 matrix = Float2x2::Rows(row0, row1);
	Float2x2 matrixT = Float2x2::Columns(row0, row1);
	Float2x2 transpose = matrix.Transpose();
	Float2x2 zero = transpose - matrixT;
	EXPECT_TRUE(zero.IsEpsilonZero());
}
TEST(Float2x2, Determinant)
{
	Float2 row0 = Float2(1.0f, 2.0f);
	Float2 row1 = Float2(3.0f, 4.0f);
	Float2x2 matrix = Float2x2::Rows(row0, row1);
	float det = matrix.Determinant();
	EXPECT_NEAR(det, -2.0f, epsilon);
}
TEST(Float2x2, Inverse)
{
	Float2 row0 = Float2(1.0f, 2.0f);
	Float2 row1 = Float2(3.0f, 4.0f);
	Float2x2 matrix = Float2x2::Rows(row0, row1);
	Float2x2 matrixInv = matrix.Inverse();
	Float2x2 zero = matrix * matrixInv - Float2x2::identity;
	EXPECT_TRUE(zero.IsEpsilonZero());
}
TEST(Float2x2, InverseDet)
{
	Float2 row0 = Float2(1.0f, 2.0f);
	Float2 row1 = Float2(3.0f, 4.0f);
	Float2x2 matrix = Float2x2::Rows(row0, row1);
	float det = matrix.Determinant();
	Float2x2 matrixInv = matrix.Inverse(det);
	Float2x2 zero = matrix * matrixInv - Float2x2::identity;
	EXPECT_TRUE(zero.IsEpsilonZero());
}
TEST(Float2x2, IsEpsilonZero)
{
	Float2x2 zero = Float2x2::zero;
	EXPECT_TRUE(zero.IsEpsilonZero());
}

// Static math operations:
TEST(Float2x2, Rotate)
{
	Float2x2 rotMatrix = Float2x2::Rotate(math::pi2);
	Float2 v0 = Float2(1.0f, 0.0f);
	Float2 v1 = rotMatrix * v0;
	ExpectNearVec(v1, Float2(0.0f, 1.0f), epsilon);
}

// Access:
TEST(Float2x2, Access)
{
	float a = 1.0f;
	float b = 2.0f;
	float c = 3.0f;
	float d = 4.0f;
	Float2x2 matrix = Float2x2::Rows(a, b, c, d);
	float A = matrix[{0, 0}];
	float B = matrix[{0, 1}];
	float C = matrix[{1, 0}];
	float D = matrix[{1, 1}];
	EXPECT_FLOAT_EQ(A, a);
	EXPECT_FLOAT_EQ(B, b);
	EXPECT_FLOAT_EQ(C, c);
	EXPECT_FLOAT_EQ(D, d);
}
TEST(Float2x2, GetRow)
{
	Float2 row0 = Float2(1.0f, 2.0f);
	Float2 row1 = Float2(3.0f, 4.0f);
	Float2x2 matrix = Float2x2::Rows(row0, row1);
	ExpectEqualVec(matrix.GetRow(0), row0);
	ExpectEqualVec(matrix.GetRow(1), row1);
}
TEST(Float2x2, GetColumn)
{
	Float2 column0 = Float2(1.0f, 3.0f);
	Float2 column1 = Float2(2.0f, 4.0f);
	Float2x2 matrix = Float2x2::Columns(column0, column1);
	ExpectEqualVec(matrix.GetColumn(0), column0);
	ExpectEqualVec(matrix.GetColumn(1), column1);
}

// Assignment:
TEST(Float2x2, Assignment)
{
	Float2 row0 = Float2(1.0f, 2.0f);
	Float2 row1 = Float2(3.0f, 4.0f);
	Float2x2 matrixA = Float2x2::Rows(row0, row1);
	Float2x2 matrixB = matrixA;
	Float2x2 zero = matrixA - matrixB;
	EXPECT_TRUE(zero.IsEpsilonZero());
}

// Addition:
TEST(Float2x2, OperatorAddition)
{
	Float2 row0A = Float2(1.0f, 2.0f);
	Float2 row1A = Float2(3.0f, 4.0f);
	Float2 row0B = Float2(5.0f, 6.0f);
	Float2 row1B = Float2(7.0f, 8.0f);
	Float2x2 matrixA = Float2x2::Rows(row0A, row1A);
	Float2x2 matrixB = Float2x2::Rows(row0B, row1B);
	Float2x2 add = matrixA + matrixB;
	Float2 row0Add = row0A + row0B;
	Float2 row1Add = row1A + row1B;
	ExpectEqualVec(add.GetRow(0), row0Add);
	ExpectEqualVec(add.GetRow(1), row1Add);
}
TEST(Float2x2, OperatorAdditionAssignment)
{
	Float2 row0A = Float2(1.0f, 2.0f);
	Float2 row1A = Float2(3.0f, 4.0f);
	Float2 row0B = Float2(5.0f, 6.0f);
	Float2 row1B = Float2(7.0f, 8.0f);
	Float2x2 add = Float2x2::Rows(row0A, row1A);
	Float2x2 matrixB = Float2x2::Rows(row0B, row1B);
	add += matrixB;
	Float2 row0Add = row0A + row0B;
	Float2 row1Add = row1A + row1B;
	ExpectEqualVec(add.GetRow(0), row0Add);
	ExpectEqualVec(add.GetRow(1), row1Add);
}

// Substraction:
TEST(Float2x2, OperatorSubstraction)
{
	Float2 row0A = Float2(1.0f, 2.0f);
	Float2 row1A = Float2(3.0f, 4.0f);
	Float2 row0B = Float2(5.0f, 6.0f);
	Float2 row1B = Float2(7.0f, 8.0f);
	Float2x2 matrixA = Float2x2::Rows(row0A, row1A);
	Float2x2 matrixB = Float2x2::Rows(row0B, row1B);
	Float2x2 sub = matrixA - matrixB;
	Float2 row0Sub = row0A - row0B;
	Float2 row1Sub = row1A - row1B;
	ExpectEqualVec(sub.GetRow(0), row0Sub);
	ExpectEqualVec(sub.GetRow(1), row1Sub);
}
TEST(Float2x2, OperatorSubstractionAssignment)
{
	Float2 row0A = Float2(1.0f, 2.0f);
	Float2 row1A = Float2(3.0f, 4.0f);
	Float2 row0B = Float2(5.0f, 6.0f);
	Float2 row1B = Float2(7.0f, 8.0f);
	Float2x2 sub = Float2x2::Rows(row0A, row1A);
	Float2x2 matrixB = Float2x2::Rows(row0B, row1B);
	sub -= matrixB;
	Float2 row0Sub = row0A - row0B;
	Float2 row1Sub = row1A - row1B;
	ExpectEqualVec(sub.GetRow(0), row0Sub);
	ExpectEqualVec(sub.GetRow(1), row1Sub);
}
TEST(Float2x2, OperatorNegation)
{
	Float2 row0 = Float2(1.0f, 2.0f);
	Float2 row1 = Float2(3.0f, 4.0f);
	Float2x2 matrixA = Float2x2::Rows(row0, row1);
	Float2x2 negation = -matrixA;
	ExpectEqualVec(negation.GetRow(0), -row0);
	ExpectEqualVec(negation.GetRow(1), -row1);
}

// Multiplication:
TEST(Float2x2, OperatorMultiplication)
{
	Float2x2 matrixA = Float2x2::Rows(Float2(1.0f, 2.0f), Float2(3.0f, 4.0f));
	Float2x2 matrixB = Float2x2::Rows(Float2(5.0f, 6.0f), Float2(7.0f, 8.0f));
	Float2x2 mult = matrixA * matrixB;
	float a = mult[{0, 0}];
	float b = mult[{1, 0}];
	float c = mult[{0, 1}];
	float d = mult[{1, 1}];
	EXPECT_FLOAT_EQ(a, 19.0f);
	EXPECT_FLOAT_EQ(b, 43.0f);
	EXPECT_FLOAT_EQ(c, 22.0f);
	EXPECT_FLOAT_EQ(d, 50.0f);
}
TEST(Float2x2, OperatorMultiplicationAssignment)
{
	Float2x2 mult = Float2x2::Rows(1.0f, 2.0f, 3.0f, 4.0f);
	Float2x2 matrixB = Float2x2::Rows(5.0f, 6.0f, 7.0f, 8.0f);
	mult *= matrixB;
	float a = mult[{0, 0}];
	float b = mult[{1, 0}];
	float c = mult[{0, 1}];
	float d = mult[{1, 1}];
	EXPECT_FLOAT_EQ(a, 19.0f);
	EXPECT_FLOAT_EQ(b, 43.0f);
	EXPECT_FLOAT_EQ(c, 22.0f);
	EXPECT_FLOAT_EQ(d, 50.0f);
}
TEST(Float2x2, OperatorMultiplicationScalarAssignment)
{
	Float2 row0 = Float2(1.0f, 2.0f);
	Float2 row1 = Float2(3.0f, 4.0f);
	Float2x2 mult = Float2x2::Rows(row0, row1);
	float scalar = 2.0f;
	mult *= scalar;
	ExpectEqualVec(mult.GetRow(0), scalar * row0);
	ExpectEqualVec(mult.GetRow(1), scalar * row1);
}

// Division:
TEST(Float2x2, OperatorDivision)
{
	Float2 row0 = Float2(1.0f, 2.0f);
	Float2 row1 = Float2(3.0f, 4.0f);
	Float2x2 matrixA = Float2x2::Rows(row0, row1);
	float scalar = 2.0f;
	Float2x2 div = matrixA / scalar;
	Float2 row0Div = row0 / scalar;
	Float2 row1Div = row1 / scalar;
	ExpectEqualVec(div.GetRow(0), row0Div);
	ExpectEqualVec(div.GetRow(1), row1Div);
}
TEST(Float2x2, OperatorDivisionAssignment)
{
	Float2 row0 = Float2(1.0f, 2.0f);
	Float2 row1 = Float2(3.0f, 4.0f);
	Float2x2 div = Float2x2::Rows(row0, row1);
	float scalar = 2.0f;
	div /= scalar;
	Float2 row0Div = row0 / scalar;
	Float2 row1Div = row1 / scalar;
	ExpectEqualVec(div.GetRow(0), row0Div);
	ExpectEqualVec(div.GetRow(1), row1Div);
}

// Comparison:
TEST(Float2x2, IsEpsilonEqual)
{
	Float2x2 matrixA = Float2x2::Rows(Float2(1.0f, 2.0f), Float2(3.0f, 4.0f));
	Float2x2 matrixB = Float2x2::Rows(Float2(1.0f, 2.0f), Float2(3.0f, 4.0f));
	EXPECT_TRUE(matrixA.IsEpsilonEqual(matrixB));
}
TEST(Float2x2, OperatorEquality)
{
	Float2x2 matrixA = Float2x2::Rows(Float2(1.0f, 2.0f), Float2(3.0f, 4.0f));
	Float2x2 matrixB = Float2x2::Rows(Float2(1.0f, 2.0f), Float2(3.0f, 4.0f));
	EXPECT_TRUE(matrixA == matrixB);
}
TEST(Float2x2, OperatorInequality)
{
	Float2x2 matrixA = Float2x2::Rows(Float2(1.0f, 2.0f), Float2(3.0f, 4.0f));
	Float2x2 matrixB = Float2x2::Rows(Float2(1.0f, 2.0f), Float2(3.0f, 4.1f));
	EXPECT_TRUE(matrixA != matrixB);
}

// Friend functions:
TEST(Float2x2, OperatorMultiplicationScalarRight)
{
	Float2x2 matrixA = Float2x2::Rows(Float2(1.0f, 2.0f), Float2(3.0f, 4.0f));
	float scalar = 2.0f;
	Float2x2 mult = matrixA * scalar;
	ExpectEqualVec(mult.GetRow(0), scalar * matrixA.GetRow(0));
	ExpectEqualVec(mult.GetRow(1), scalar * matrixA.GetRow(1));
}
TEST(Float2x2, OperatorMultiplicationScalarLeft)
{
	Float2x2 matrixA = Float2x2::Rows(Float2(1.0f, 2.0f), Float2(3.0f, 4.0f));
	float scalar = 2.0f;
	Float2x2 mult = scalar * matrixA;
	ExpectEqualVec(mult.GetRow(0), scalar * matrixA.GetRow(0));
	ExpectEqualVec(mult.GetRow(1), scalar * matrixA.GetRow(1));
}
TEST(Float2x2, OperatorMultiplicationFloat2Right)
{
	float a = 1.0f;
	float b = 2.0f;
	float c = 3.0f;
	float d = 4.0f;
	Float2x2 matrixA = Float2x2::Rows(Float2(a,b), Float2(c,d));
	Float2 vector = Float2(1.2f, 2.1f);
	Float2 mult = matrixA * vector;
	EXPECT_FLOAT_EQ(mult.x, a * vector.x + b * vector.y);
	EXPECT_FLOAT_EQ(mult.y, c * vector.x + d * vector.y);
}
TEST(Float2x2, OperatorMultiplicationFloat2Left)
{
	float a = 1.0f;
	float b = 2.0f;
	float c = 3.0f;
	float d = 4.0f;
	Float2x2 matrixA = Float2x2::Rows(Float2(a, b), Float2(c, d));
	Float2 vector = Float2(1.2f, 2.1f);
	Float2 mult = vector * matrixA;
	EXPECT_FLOAT_EQ(mult.x, a * vector.x + c * vector.y);
	EXPECT_FLOAT_EQ(mult.y, b * vector.x + d * vector.y);
}