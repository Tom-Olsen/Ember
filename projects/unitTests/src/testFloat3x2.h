#ifndef __INCLUDE_GUARD_testFloat3x2_h__
#define __INCLUDE_GUARD_testFloat3x2_h__



// Static constructors:
TEST(Float3x2, ConstructorRowsScalar)
{
	Float2 row0 = Float2(1.0f, 2.0f);
	Float2 row1 = Float2(3.0f, 4.0f);
	Float2 row2 = Float2(5.0f, 6.0f);
	Float3x2 matrix = Float3x2::Rows(row0.x, row0.y, row1.x, row1.y, row2.x, row2.y);
	EXPECT_FLOAT2_EQ(matrix.GetRow(0), row0);
	EXPECT_FLOAT2_EQ(matrix.GetRow(1), row1);
	EXPECT_FLOAT2_EQ(matrix.GetRow(2), row2);
}
TEST(Float3x2, ConstructorRowsVector)
{
	Float2 row0 = Float2(1.0f, 2.0f);
	Float2 row1 = Float2(3.0f, 4.0f);
	Float2 row2 = Float2(5.0f, 6.0f);
	Float3x2 matrix = Float3x2::Rows(row0, row1, row2);
	EXPECT_FLOAT2_EQ(matrix.GetRow(0), row0);
	EXPECT_FLOAT2_EQ(matrix.GetRow(1), row1);
	EXPECT_FLOAT2_EQ(matrix.GetRow(2), row2);
}
TEST(Float3x2, ConstructorColumnsScalar)
{
	Float3 column0 = Float3(1.0f, 2.0f, 3.0f);
	Float3 column1 = Float3(4.0f, 5.0f, 6.0f);
	Float3x2 matrix = Float3x2::Columns(column0.x, column0.y, column0.z, column1.x, column1.y, column1.z);
	EXPECT_FLOAT3_EQ(matrix.GetColumn(0), column0);
	EXPECT_FLOAT3_EQ(matrix.GetColumn(1), column1);
}
TEST(Float3x2, ConstructorColumnsVector)
{
	Float3 column0 = Float3(1.0f, 2.0f, 3.0f);
	Float3 column1 = Float3(4.0f, 5.0f, 6.0f);
	Float3x2 matrix = Float3x2::Columns(column0, column1);
	EXPECT_FLOAT3_EQ(matrix.GetColumn(0), column0);
	EXPECT_FLOAT3_EQ(matrix.GetColumn(1), column1);
}

// Math operations:
TEST(Float3x2, Transpose)
{
	Float2 row0 = Float2(1.0f, 2.0f);
	Float2 row1 = Float2(3.0f, 4.0f);
	Float2 row2 = Float2(5.0f, 6.0f);
	Float3x2 matrix = Float3x2::Rows(row0, row1, row2);
	Float2x3 transpose = matrix.Transpose();
	EXPECT_FLOAT2_EQ(transpose.GetColumn(0), row0);
	EXPECT_FLOAT2_EQ(transpose.GetColumn(1), row1);
	EXPECT_FLOAT2_EQ(transpose.GetColumn(2), row2);
}
TEST(Float3x2, LeftInverse)
{
	Float2 row0 = Float2(1.0f, 2.0f);
	Float2 row1 = Float2(3.0f, 4.0f);
	Float2 row2 = Float2(5.0f, 6.0f);
	Float3x2 matrix = Float3x2::Rows(row0, row1, row2);
	Float2x3 leftInverse = matrix.LeftInverse();
	Float2x2 result = leftInverse * matrix;
	EXPECT_TRUE(result.IsEpsilonEqual(Float2x2::identity));
}
TEST(Float3x2, IsEpsilonZero)
{
	Float3x2 matrix = Float3x2::zero;
	EXPECT_TRUE(matrix.IsEpsilonZero());
}

// Access:
TEST(Float3x2, Access)
{
	float a = 1.0f;
	float b = 2.0f;
	float c = 3.0f;
	float d = 4.0f;
	float e = 5.0f;
	float f = 6.0f;
	Float3x2 matrix = Float3x2::Rows(a, b, c, d, e, f);
	float A = matrix[{0, 0}];
	float B = matrix[{0, 1}];
	float C = matrix[{1, 0}];
	float D = matrix[{1, 1}];
	float E = matrix[{2, 0}];
	float F = matrix[{2, 1}];
	EXPECT_FLOAT_EQ(A, a);
	EXPECT_FLOAT_EQ(B, b);
	EXPECT_FLOAT_EQ(C, c);
	EXPECT_FLOAT_EQ(D, d);
	EXPECT_FLOAT_EQ(E, e);
	EXPECT_FLOAT_EQ(F, f);
}
TEST(Float3x2, GetRow)
{
	Float2 row0 = Float2(1.0f, 2.0f);
	Float2 row1 = Float2(3.0f, 4.0f);
	Float2 row2 = Float2(5.0f, 6.0f);
	Float3x2 matrix = Float3x2::Rows(row0, row1, row2);
	EXPECT_FLOAT2_EQ(matrix.GetRow(0), row0);
	EXPECT_FLOAT2_EQ(matrix.GetRow(1), row1);
	EXPECT_FLOAT2_EQ(matrix.GetRow(2), row2);
}
TEST(Float3x2, GetColumn)
{
	Float3 column0 = Float3(1.0f, 2.0f, 3.0f);
	Float3 column1 = Float3(4.0f, 5.0f, 6.0f);
	Float3x2 matrix = Float3x2::Columns(column0, column1);
	EXPECT_FLOAT3_EQ(matrix.GetColumn(0), column0);
	EXPECT_FLOAT3_EQ(matrix.GetColumn(1), column1);
}

// Assignment:
TEST(Float3x2, OperatorAssignment)
{
	Float2 row0 = Float2(1.0f, 2.0f);
	Float2 row1 = Float2(3.0f, 4.0f);
	Float2 row2 = Float2(5.0f, 6.0f);
	Float3x2 matrixA = Float3x2::Rows(row0, row1, row2);
	Float3x2 matrixB = matrixA;
	Float3x2 zero = matrixA - matrixB;
	EXPECT_TRUE(zero.IsEpsilonZero());
}

// Addition:
TEST(Float3x2, OperatorAddition)
{
	Float3 column0A = Float3(1.0f, 2.0f, 3.0f);
	Float3 column1A = Float3(4.0f, 5.0f, 6.0f);
	Float3 column0B = Float3(7.0f, 8.0f, 9.0f);
	Float3 column1B = Float3(10.0f, 11.0f, 12.0f);
	Float3x2 matrixA = Float3x2::Columns(column0A, column1A);
	Float3x2 matrixB = Float3x2::Columns(column0B, column1B);
	Float3x2 add = matrixA + matrixB;
	Float3 column0Add = column0A + column0B;
	Float3 column1Add = column1A + column1B;
	EXPECT_FLOAT3_EQ(add.GetColumn(0), column0Add);
	EXPECT_FLOAT3_EQ(add.GetColumn(1), column1Add);
}
TEST(Float3x2, OperatorAdditionAssignment)
{
	Float3 column0A = Float3(1.0f, 2.0f, 3.0f);
	Float3 column1A = Float3(4.0f, 5.0f, 6.0f);
	Float3 column0B = Float3(7.0f, 8.0f, 9.0f);
	Float3 column1B = Float3(10.0f, 11.0f, 12.0f);
	Float3x2 add = Float3x2::Columns(column0A, column1A);
	Float3x2 matrixB = Float3x2::Columns(column0B, column1B);
	add += matrixB;
	Float3 column0Add = column0A + column0B;
	Float3 column1Add = column1A + column1B;
	EXPECT_FLOAT3_EQ(add.GetColumn(0), column0Add);
	EXPECT_FLOAT3_EQ(add.GetColumn(1), column1Add);
}

// Substraction:
TEST(Float3x2, OperatorSubstraction)
{
	Float3 column0A = Float3(1.0f, 2.0f, 3.0f);
	Float3 column1A = Float3(4.0f, 5.0f, 6.0f);
	Float3 column0B = Float3(7.0f, 8.0f, 9.0f);
	Float3 column1B = Float3(10.0f, 11.0f, 12.0f);
	Float3x2 matrixA = Float3x2::Columns(column0A, column1A);
	Float3x2 matrixB = Float3x2::Columns(column0B, column1B);
	Float3x2 sub = matrixA - matrixB;
	Float3 column0Sub = column0A - column0B;
	Float3 column1Sub = column1A - column1B;
	EXPECT_FLOAT3_EQ(sub.GetColumn(0), column0Sub);
	EXPECT_FLOAT3_EQ(sub.GetColumn(1), column1Sub);
}
TEST(Float3x2, OperatorSubstractionAssignment)
{
	Float3 column0A = Float3(1.0f, 2.0f, 3.0f);
	Float3 column1A = Float3(4.0f, 5.0f, 6.0f);
	Float3 column0B = Float3(7.0f, 8.0f, 9.0f);
	Float3 column1B = Float3(10.0f, 11.0f, 12.0f);
	Float3x2 sub = Float3x2::Columns(column0A, column1A);
	Float3x2 matrixB = Float3x2::Columns(column0B, column1B);
	sub -= matrixB;
	Float3 column0Sub = column0A - column0B;
	Float3 column1Sub = column1A - column1B;
	EXPECT_FLOAT3_EQ(sub.GetColumn(0), column0Sub);
	EXPECT_FLOAT3_EQ(sub.GetColumn(1), column1Sub);
}
TEST(Float3x2, OperatorNegation)
{
	Float2 row0 = Float2(1.0f, 2.0f);
	Float2 row1 = Float2(3.0f, 4.0f);
	Float2 row2 = Float2(5.0f, 6.0f);
	Float3x2 matrix = Float3x2::Rows(row0, row1, row2);
	Float3x2 negation = -matrix;
	EXPECT_FLOAT2_EQ(negation.GetRow(0), -row0);
	EXPECT_FLOAT2_EQ(negation.GetRow(1), -row1);
	EXPECT_FLOAT2_EQ(negation.GetRow(2), -row2);
}

// Multiplication:
TEST(Float3x2, OperatorMultiplicationScalarAssignment)
{
	Float2 row0 = Float2(1.0f, 2.0f);
	Float2 row1 = Float2(3.0f, 4.0f);
	Float2 row2 = Float2(5.0f, 6.0f);
	Float3x2 mult = Float3x2::Rows(row0, row1, row2);
	float scalar = 2.0f;
	mult *= scalar;
	EXPECT_FLOAT2_EQ(mult.GetRow(0), scalar * row0);
	EXPECT_FLOAT2_EQ(mult.GetRow(1), scalar * row1);
	EXPECT_FLOAT2_EQ(mult.GetRow(2), scalar * row2);
}

// Division:
TEST(Float3x2, OperatorDivision)
{
	Float2 row0 = Float2(1.0f, 2.0f);
	Float2 row1 = Float2(3.0f, 4.0f);
	Float2 row2 = Float2(5.0f, 6.0f);
	Float3x2 matrixA = Float3x2::Rows(row0, row1, row2);
	float scalar = 2.0f;
	Float3x2 div = matrixA / scalar;
	Float2 row0Div = row0 / scalar;
	Float2 row1Div = row1 / scalar;
	Float2 row2Div = row2 / scalar;
	EXPECT_FLOAT2_EQ(div.GetRow(0), row0Div);
	EXPECT_FLOAT2_EQ(div.GetRow(1), row1Div);
	EXPECT_FLOAT2_EQ(div.GetRow(2), row2Div);
}
TEST(Float3x2, OperatorDivisionAssignment)
{
	Float2 row0 = Float2(1.0f, 2.0f);
	Float2 row1 = Float2(3.0f, 4.0f);
	Float2 row2 = Float2(5.0f, 6.0f);
	Float3x2 div = Float3x2::Rows(row0, row1, row2);
	float scalar = 2.0f;
	div /= scalar;
	Float2 row0Div = row0 / scalar;
	Float2 row1Div = row1 / scalar;
	Float2 row2Div = row2 / scalar;
	EXPECT_FLOAT2_EQ(div.GetRow(0), row0Div);
	EXPECT_FLOAT2_EQ(div.GetRow(1), row1Div);
	EXPECT_FLOAT2_EQ(div.GetRow(2), row2Div);
}

// Comparison:
TEST(Float3x2, IsEpsilonEqual)
{
	Float3x2 matrixA = Float3x2::Columns(Float3(1.0f, 2.0f, 3.0f), Float3(4.0f, 5.0f, 6.0f));
	Float3x2 matrixB = Float3x2::Columns(Float3(1.0f, 2.0f, 3.0f), Float3(4.0f, 5.0f, 6.0f));
	EXPECT_TRUE(matrixA.IsEpsilonEqual(matrixB));
}
TEST(Float3x2, OperatorEquality)
{
	Float3x2 matrixA = Float3x2::Columns(Float3(1.0f, 2.0f, 3.0f), Float3(4.0f, 5.0f, 6.0f));
	Float3x2 matrixB = Float3x2::Columns(Float3(1.0f, 2.0f, 3.0f), Float3(4.0f, 5.0f, 6.0f));
	EXPECT_TRUE(matrixA == matrixB);
}
TEST(Float3x2, OperatorInequality)
{
	Float3x2 matrixA = Float3x2::Columns(Float3(1.0f, 2.0f, 3.0f), Float3(4.0f, 5.0f, 6.0f));
	Float3x2 matrixB = Float3x2::Columns(Float3(1.0f, 2.0f, 3.0f), Float3(4.0f, 5.0f, 6.1f));
	EXPECT_TRUE(matrixA != matrixB);
}

// Friend functions:
TEST(Float3x2, OperatorMultiplicationScalarRight)
{
	Float3x2 matrixA = Float3x2::Columns(Float3(1.0f, 2.0f, 3.0f), Float3(4.0f, 5.0f, 6.0f));
	float scalar = 2.0f;
	Float3x2 mult = matrixA * scalar;
	EXPECT_FLOAT2_EQ(mult.GetRow(0), scalar * matrixA.GetRow(0));
	EXPECT_FLOAT2_EQ(mult.GetRow(1), scalar * matrixA.GetRow(1));
	EXPECT_FLOAT2_EQ(mult.GetRow(2), scalar * matrixA.GetRow(2));
}
TEST(Float3x2, OperatorMultiplicationScalarLeft)
{
	Float3x2 matrixA = Float3x2::Columns(Float3(1.0f, 2.0f, 3.0f), Float3(4.0f, 5.0f, 6.0f));
	float scalar = 2.0f;
	Float3x2 mult = scalar * matrixA;
	EXPECT_FLOAT2_EQ(mult.GetRow(0), scalar * matrixA.GetRow(0));
	EXPECT_FLOAT2_EQ(mult.GetRow(1), scalar * matrixA.GetRow(1));
	EXPECT_FLOAT2_EQ(mult.GetRow(2), scalar * matrixA.GetRow(2));
}
TEST(Float3x2, OperatorMultiplicationFloat2Right)
{
	float a = 1.0f;
	float b = 2.0f;
	float c = 3.0f;
	float d = 4.0f;
	float e = 5.0f;
	float f = 6.0f;
	Float3x2 matrixA = Float3x2::Rows(Float2(a, b), Float2(c, d), Float2(e, f));
	Float2 vector = Float2(1.2f, 2.1f);
	Float3 mult = matrixA * vector;
	EXPECT_FLOAT_EQ(mult.x, a * vector.x + b * vector.y);
	EXPECT_FLOAT_EQ(mult.y, c * vector.x + d * vector.y);
	EXPECT_FLOAT_EQ(mult.z, e * vector.x + f * vector.y);
}
TEST(Float3x2, OperatorMultiplicationFloat3Left)
{
	float a = 1.0f;
	float b = 2.0f;
	float c = 3.0f;
	float d = 4.0f;
	float e = 5.0f;
	float f = 6.0f;
	Float3x2 matrixA = Float3x2::Rows(Float2(a, b), Float2(c, d), Float2(e, f));
	Float3 vector = Float3(1.3f, 2.1f, 3.2f);
	Float2 mult = vector * matrixA;
	EXPECT_FLOAT_EQ(mult.x, a * vector.x + c * vector.y + e * vector.z);
	EXPECT_FLOAT_EQ(mult.y, b * vector.x + d * vector.y + f * vector.z);
}



#endif // __INCLUDE_GUARD_testFloat3x2_h__