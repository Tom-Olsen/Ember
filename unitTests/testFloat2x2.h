#ifndef __INCLUDE_GUARD_testFloat2x2_h__
#define __INCLUDE_GUARD_testFloat2x2_h__



// Static constructors:
TEST(Float2x2, ConstructorRowsScalar)
{
	Float2 row0 = Float2(1.0f, 2.0f);
	Float2 row1 = Float2(3.0f, 4.0f);
	Float2x2 matrix = Float2x2::Rows(row0.x, row0.y, row1.x, row1.y);
	EXPECT_FLOAT2_EQ(matrix.GetRow(0), row0);
	EXPECT_FLOAT2_EQ(matrix.GetRow(1), row1);
}
TEST(Float2x2, ConstructorRowsVector)
{
	Float2 row0 = Float2(1.0f, 2.0f);
	Float2 row1 = Float2(3.0f, 4.0f);
	Float2x2 matrix = Float2x2::Rows(row0, row1);
	EXPECT_FLOAT2_EQ(matrix.GetRow(0),row0);
	EXPECT_FLOAT2_EQ(matrix.GetRow(1),row1);
}
TEST(Float2x2, ConstructorColumnsScalar)
{
	Float2 column0 = Float2(1.0f, 2.0f);
	Float2 column1 = Float2(3.0f, 4.0f);
	Float2x2 matrix = Float2x2::Columns(column0.x, column0.y, column1.x, column1.y);
	EXPECT_FLOAT2_EQ(matrix.GetColumn(0), column0);
	EXPECT_FLOAT2_EQ(matrix.GetColumn(1), column1);
}
TEST(Float2x2, ConstructorColumnsVector)
{
	Float2 column0 = Float2(1.0f, 2.0f);
	Float2 column1 = Float2(3.0f, 4.0f);
	Float2x2 matrix = Float2x2::Columns(column0, column1);
	EXPECT_FLOAT2_EQ(matrix.GetColumn(0), column0);
	EXPECT_FLOAT2_EQ(matrix.GetColumn(1), column1);
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
	Float2x2 rotMatrix = Float2x2::Rotate(mathf::pi2);
	Float2 v0 = Float2(1.0f, 0.0f);
	Float2 v1 = rotMatrix * v0;
	EXPECT_NEAR2(v1, Float2(0.0f, 1.0f), epsilon);
}

// Access:
TEST(Float2x2, Access)
{
	float a = 1.0f;
	float b = 2.0f;
	float c = 3.0f;
	float d = 4.0f;
	Float2x2 matrix;
	matrix[0] = a;
	matrix[1] = b;
	matrix[2] = c;
	matrix[3] = d;
	float matrix00 = matrix[{0, 0}];
	float matrix10 = matrix[{1, 0}];
	float matrix01 = matrix[{0, 1}];
	float matrix11 = matrix[{1, 1}];
	EXPECT_FLOAT_EQ(matrix00, a);
	EXPECT_FLOAT_EQ(matrix10, b);
	EXPECT_FLOAT_EQ(matrix01, c);
	EXPECT_FLOAT_EQ(matrix11, d);
}
TEST(Float2x2, GetRow)
{
	Float2 row0 = Float2(1.0f, 2.0f);
	Float2 row1 = Float2(3.0f, 4.0f);
	Float2x2 matrix = Float2x2::Rows(row0, row1);
	EXPECT_FLOAT2_EQ(matrix.GetRow(0), row0);
	EXPECT_FLOAT2_EQ(matrix.GetRow(1), row1);
}
TEST(Float2x2, GetColumn)
{
	Float2 column0 = Float2(1.0f, 3.0f);
	Float2 column1 = Float2(2.0f, 4.0f);
	Float2x2 matrix = Float2x2::Columns(column0, column1);
	EXPECT_FLOAT2_EQ(matrix.GetColumn(0), column0);
	EXPECT_FLOAT2_EQ(matrix.GetColumn(1), column1);
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
	EXPECT_FLOAT2_EQ(add.GetRow(0), row0Add);
	EXPECT_FLOAT2_EQ(add.GetRow(1), row1Add);
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
	EXPECT_FLOAT2_EQ(add.GetRow(0), row0Add);
	EXPECT_FLOAT2_EQ(add.GetRow(1), row1Add);
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
	EXPECT_FLOAT2_EQ(sub.GetRow(0), row0Sub);
	EXPECT_FLOAT2_EQ(sub.GetRow(1), row1Sub);
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
	EXPECT_FLOAT2_EQ(sub.GetRow(0), row0Sub);
	EXPECT_FLOAT2_EQ(sub.GetRow(1), row1Sub);
}
TEST(Float2x2, OperatorNegation)
{
	Float2 row0 = Float2(1.0f, 2.0f);
	Float2 row1 = Float2(3.0f, 4.0f);
	Float2x2 matrixA = Float2x2::Rows(row0, row1);
	Float2x2 negation = -matrixA;
	EXPECT_FLOAT2_EQ(negation.GetRow(0), -row0);
	EXPECT_FLOAT2_EQ(negation.GetRow(1), -row1);
}

// Multiplication:
TEST(Float2x2, OperatorMultiplication)
{
	Float2x2 matrixA = Float2x2::Rows(Float2(1.0f, 2.0f), Float2(3.0f, 4.0f));
	Float2x2 matrixB = Float2x2::Rows(Float2(5.0f, 6.0f), Float2(7.0f, 8.0f));
	Float2x2 mult = matrixA * matrixB;
	EXPECT_FLOAT_EQ(mult[0], 19.0f);
	EXPECT_FLOAT_EQ(mult[1], 43.0f);
	EXPECT_FLOAT_EQ(mult[2], 22.0f);
	EXPECT_FLOAT_EQ(mult[3], 50.0f);
}
TEST(Float2x2, OperatorMultiplicationAssignment)
{
	Float2x2 mult = Float2x2::Rows(1.0f, 2.0f, 3.0f, 4.0f);
	Float2x2 matrixB = Float2x2::Rows(5.0f, 6.0f, 7.0f, 8.0f);
	mult *= matrixB;
	EXPECT_FLOAT_EQ(mult[0], 19.0f);
	EXPECT_FLOAT_EQ(mult[1], 43.0f);
	EXPECT_FLOAT_EQ(mult[2], 22.0f);
	EXPECT_FLOAT_EQ(mult[3], 50.0f);
}
TEST(Float2x2, OperatorMultiplicationScalarAssignment)
{
	Float2 row0 = Float2(1.0f, 2.0f);
	Float2 row1 = Float2(3.0f, 4.0f);
	Float2x2 mult = Float2x2::Rows(row0, row1);
	float scalar = 2.0f;
	mult *= scalar;
	EXPECT_FLOAT2_EQ(mult.GetRow(0), scalar * row0);
	EXPECT_FLOAT2_EQ(mult.GetRow(1), scalar * row1);
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
	EXPECT_FLOAT2_EQ(div.GetRow(0), row0Div);
	EXPECT_FLOAT2_EQ(div.GetRow(1), row1Div);
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
	EXPECT_FLOAT2_EQ(div.GetRow(0), row0Div);
	EXPECT_FLOAT2_EQ(div.GetRow(1), row1Div);
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
	EXPECT_FLOAT2_EQ(mult.GetRow(0), scalar * matrixA.GetRow(0));
	EXPECT_FLOAT2_EQ(mult.GetRow(1), scalar * matrixA.GetRow(1));
}
TEST(Float2x2, OperatorMultiplicationScalarLeft)
{
	Float2x2 matrixA = Float2x2::Rows(Float2(1.0f, 2.0f), Float2(3.0f, 4.0f));
	float scalar = 2.0f;
	Float2x2 mult = scalar * matrixA;
	EXPECT_FLOAT2_EQ(mult.GetRow(0), scalar * matrixA.GetRow(0));
	EXPECT_FLOAT2_EQ(mult.GetRow(1), scalar * matrixA.GetRow(1));
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



#endif // __INCLUDE_GUARD_testFloat2x2_h__