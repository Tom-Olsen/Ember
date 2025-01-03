#ifndef __INCLUDE_GUARD_testFloat2x3_h__
#define __INCLUDE_GUARD_testFloat2x3_h__



// Static constructors:
TEST(Float2x3, ConstructorRowsScalar)
{
	Float3 row0 = Float3(1.0f, 2.0f, 3.0f);
	Float3 row1 = Float3(4.0f, 5.0f, 6.0f);
	Float2x3 matrix = Float2x3::Rows(row0.x, row0.y, row0.z, row1.x, row1.y, row1.z);
	EXPECT_FLOAT3_EQ(matrix.GetRow(0), row0);
	EXPECT_FLOAT3_EQ(matrix.GetRow(1), row1);
}
TEST(Float2x3, ConstructorRowsVector)
{
	Float3 row0 = Float3(1.0f, 2.0f, 3.0f);
	Float3 row1 = Float3(4.0f, 5.0f, 6.0f);
	Float2x3 matrix = Float2x3::Rows(row0, row1);
	EXPECT_FLOAT3_EQ(matrix.GetRow(0), row0);
	EXPECT_FLOAT3_EQ(matrix.GetRow(1), row1);
}
TEST(Float2x3, ConstructorColumnsScalar)
{
	Float2 column0 = Float2(1.0f, 2.0f);
	Float2 column1 = Float2(3.0f, 4.0f);
	Float2 column2 = Float2(5.0f, 6.0f);
	Float2x3 matrix = Float2x3::Columns(column0.x, column0.y, column1.x, column1.y, column2.x, column2.y);
	EXPECT_FLOAT2_EQ(matrix.GetColumn(0), column0);
	EXPECT_FLOAT2_EQ(matrix.GetColumn(1), column1);
	EXPECT_FLOAT2_EQ(matrix.GetColumn(2), column2);
}
TEST(Float2x3, ConstructorColumnsVector)
{
	Float2 column0 = Float2(1.0f, 2.0f);
	Float2 column1 = Float2(3.0f, 4.0f);
	Float2 column2 = Float2(5.0f, 6.0f);
	Float2x3 matrix = Float2x3::Columns(column0, column1, column2);
	EXPECT_FLOAT2_EQ(matrix.GetColumn(0), column0);
	EXPECT_FLOAT2_EQ(matrix.GetColumn(1), column1);
	EXPECT_FLOAT2_EQ(matrix.GetColumn(2), column2);
}

// Math operations:
TEST(Float2x3, Transpose)
{
	Float3 row0 = Float3(1.0f, 2.0f, 3.0f);
	Float3 row1 = Float3(4.0f, 5.0f, 6.0f);
	Float2x3 matrix = Float2x3::Rows(row0, row1);
	Float3x2 transpose = matrix.Transpose();
	EXPECT_FLOAT3_EQ(transpose.GetColumn(0), row0);
	EXPECT_FLOAT3_EQ(transpose.GetColumn(1), row1);
}
TEST(Float2x3, RightInverse)
{
	Float3 row0 = Float3(1.0f, 2.0f, 3.0f);
	Float3 row1 = Float3(4.0f, 5.0f, 6.0f);
	Float2x3 matrix = Float2x3::Rows(row0, row1);
	Float3x2 rightInverse = matrix.RightInverse();
	Float2x2 result = matrix * rightInverse;
	EXPECT_TRUE(result.IsEpsilonEqual(Float2x2::identity));
}
TEST(Float2x3, IsEpsilonZero)
{
	Float2x3 matrix = Float2x3::zero;
	EXPECT_TRUE(matrix.IsEpsilonZero());
}

// Access:
TEST(Float2x3, OperatorBrackets)
{
	float a = 1.0f;
	float b = 2.0f;
	float c = 3.0f;
	float d = 4.0f;
	float e = 5.0f;
	float f = 6.0f;
	Float2x3 matrix;
	matrix[0] = a;
	matrix[1] = b;
	matrix[2] = c;
	matrix[3] = d;
	matrix[4] = e;
	matrix[5] = f;
	float matrix00 = matrix[{0, 0}];
	float matrix01 = matrix[{0, 1}];
	float matrix02 = matrix[{0, 2}];
	float matrix10 = matrix[{1, 0}];
	float matrix11 = matrix[{1, 1}];
	float matrix12 = matrix[{1, 2}];
	EXPECT_FLOAT_EQ(matrix00, a);
	EXPECT_FLOAT_EQ(matrix01, c);
	EXPECT_FLOAT_EQ(matrix02, e);
	EXPECT_FLOAT_EQ(matrix10, b);
	EXPECT_FLOAT_EQ(matrix11, d);
	EXPECT_FLOAT_EQ(matrix12, f);
}
TEST(Float2x3, GetRow)
{
	Float3 row0 = Float3(1.0f, 2.0f, 3.0f);
	Float3 row1 = Float3(4.0f, 5.0f, 6.0f);
	Float2x3 matrix = Float2x3::Rows(row0, row1);
	EXPECT_FLOAT3_EQ(matrix.GetRow(0), row0);
	EXPECT_FLOAT3_EQ(matrix.GetRow(1), row1);
}
TEST(Float2x3, GetColumn)
{
	Float2 column0 = Float2(1.0f, 4.0f);
	Float2 column1 = Float2(2.0f, 5.0f);
	Float2 column2 = Float2(3.0f, 6.0f);
	Float2x3 matrix = Float2x3::Columns(column0, column1, column2);
	EXPECT_FLOAT2_EQ(matrix.GetColumn(0), column0);
	EXPECT_FLOAT2_EQ(matrix.GetColumn(1), column1);
	EXPECT_FLOAT2_EQ(matrix.GetColumn(2), column2);
}

// Assignment:
TEST(Float2x3, OperatorAssignment)
{
	Float3 row0 = Float3(1.0f, 2.0f, 3.0f);
	Float3 row1 = Float3(4.0f, 5.0f, 6.0f);
	Float2x3 matrixA = Float2x3::Rows(row0, row1);
	Float2x3 matrixB = matrixA;
	Float2x3 zero = matrixA - matrixB;
	EXPECT_TRUE(zero.IsEpsilonZero());
}

// Addition:
TEST(Float2x3, OperatorAddition)
{
	Float3 row0A = Float3(1.0f, 2.0f, 3.0f);
	Float3 row1A = Float3(4.0f, 5.0f, 6.0f);
	Float3 row0B = Float3(7.0f, 8.0f, 9.0f);
	Float3 row1B = Float3(10.0f, 11.0f, 12.0f);
	Float2x3 matrixA = Float2x3::Rows(row0A, row1A);
	Float2x3 matrixB = Float2x3::Rows(row0B, row1B);
	Float2x3 add = matrixA + matrixB;
	Float3 row0Add = row0A + row0B;
	Float3 row1Add = row1A + row1B;
	EXPECT_FLOAT3_EQ(add.GetRow(0), row0Add);
	EXPECT_FLOAT3_EQ(add.GetRow(1), row1Add);
}
TEST(Float2x3, OperatorAdditionAssignment)
{
	Float3 row0A = Float3(1.0f, 2.0f, 3.0f);
	Float3 row1A = Float3(4.0f, 5.0f, 6.0f);
	Float3 row0B = Float3(7.0f, 8.0f, 9.0f);
	Float3 row1B = Float3(10.0f, 11.0f, 12.0f);
	Float2x3 add = Float2x3::Rows(row0A, row1A);
	Float2x3 matrixB = Float2x3::Rows(row0B, row1B);
	add += matrixB;
	Float3 row0Add = row0A + row0B;
	Float3 row1Add = row1A + row1B;
	EXPECT_FLOAT3_EQ(add.GetRow(0), row0Add);
	EXPECT_FLOAT3_EQ(add.GetRow(1), row1Add);
}

// Substraction:
TEST(Float2x3, OperatorSubstraction)
{
	Float3 row0A = Float3(1.0f, 2.0f, 3.0f);
	Float3 row1A = Float3(4.0f, 5.0f, 6.0f);
	Float3 row0B = Float3(7.0f, 8.0f, 9.0f);
	Float3 row1B = Float3(10.0f, 11.0f, 12.0f);
	Float2x3 matrixA = Float2x3::Rows(row0A, row1A);
	Float2x3 matrixB = Float2x3::Rows(row0B, row1B);
	Float2x3 sub = matrixA - matrixB;
	Float3 row0Sub = row0A - row0B;
	Float3 row1Sub = row1A - row1B;
	EXPECT_FLOAT3_EQ(sub.GetRow(0), row0Sub);
	EXPECT_FLOAT3_EQ(sub.GetRow(1), row1Sub);
}
TEST(Float2x3, OperatorSubstractionAssignment)
{
	Float3 row0A = Float3(1.0f, 2.0f, 3.0f);
	Float3 row1A = Float3(4.0f, 5.0f, 6.0f);
	Float3 row0B = Float3(7.0f, 8.0f, 9.0f);
	Float3 row1B = Float3(10.0f, 11.0f, 12.0f);
	Float2x3 sub = Float2x3::Rows(row0A, row1A);
	Float2x3 matrixB = Float2x3::Rows(row0B, row1B);
	sub -= matrixB;
	Float3 row0Sub = row0A - row0B;
	Float3 row1Sub = row1A - row1B;
	EXPECT_FLOAT3_EQ(sub.GetRow(0), row0Sub);
	EXPECT_FLOAT3_EQ(sub.GetRow(1), row1Sub);
}
TEST(Float2x3, OperatorNegation)
{
	Float3 row0 = Float3(1.0f, 2.0f, 3.0f);
	Float3 row1 = Float3(4.0f, 5.0f, 6.0f);
	Float2x3 matrix = Float2x3::Rows(row0, row1);
	Float2x3 negation = -matrix;
	EXPECT_FLOAT3_EQ(negation.GetRow(0), -row0);
	EXPECT_FLOAT3_EQ(negation.GetRow(1), -row1);
}

// Multiplication:
TEST(Float2x3, OperatorMultiplicationScalarAssignment)
{
	Float3 row0 = Float3(1.0f, 2.0f, 3.0f);
	Float3 row1 = Float3(4.0f, 5.0f, 6.0f);
	Float2x3 mult = Float2x3::Rows(row0, row1);
	float scalar = 2.0f;
	mult *= scalar;
	EXPECT_FLOAT3_EQ(mult.GetRow(0), scalar * row0);
	EXPECT_FLOAT3_EQ(mult.GetRow(1), scalar * row1);
}

// Division:
TEST(Float2x3, OperatorDivision)
{
	Float3 row0 = Float3(1.0f, 2.0f, 3.0f);
	Float3 row1 = Float3(4.0f, 5.0f, 6.0f);
	Float2x3 matrixA = Float2x3::Rows(row0, row1);
	float scalar = 2.0f;
	Float2x3 div = matrixA / scalar;
	Float3 row0Div = row0 / scalar;
	Float3 row1Div = row1 / scalar;
	EXPECT_FLOAT3_EQ(div.GetRow(0), row0Div);
	EXPECT_FLOAT3_EQ(div.GetRow(1), row1Div);
}
TEST(Float2x3, OperatorDivisionAssignment)
{
	Float3 row0 = Float3(1.0f, 2.0f, 3.0f);
	Float3 row1 = Float3(4.0f, 5.0f, 6.0f);
	Float2x3 div = Float2x3::Rows(row0, row1);
	float scalar = 2.0f;
	div /= scalar;
	Float3 row0Div = row0 / scalar;
	Float3 row1Div = row1 / scalar;
	EXPECT_FLOAT3_EQ(div.GetRow(0), row0Div);
	EXPECT_FLOAT3_EQ(div.GetRow(1), row1Div);
}

// Comparison:
TEST(Float2x3, IsEpsilonEqual)
{
	Float2x3 matrixA = Float2x3::Rows(Float3(1.0f, 2.0f, 3.0f), Float3(4.0f, 5.0f, 6.0f));
	Float2x3 matrixB = Float2x3::Rows(Float3(1.0f, 2.0f, 3.0f), Float3(4.0f, 5.0f, 6.0f));
	EXPECT_TRUE(matrixA.IsEpsilonEqual(matrixB));
}
TEST(Float2x3, OperatorEquality)
{
	Float2x3 matrixA = Float2x3::Rows(Float3(1.0f, 2.0f, 3.0f), Float3(4.0f, 5.0f, 6.0f));
	Float2x3 matrixB = Float2x3::Rows(Float3(1.0f, 2.0f, 3.0f), Float3(4.0f, 5.0f, 6.0f));
	EXPECT_TRUE(matrixA == matrixB);
}
TEST(Float2x3, OperatorInequality)
{
	Float2x3 matrixA = Float2x3::Rows(Float3(1.0f, 2.0f, 3.0f), Float3(4.0f, 5.0f, 6.0f));
	Float2x3 matrixB = Float2x3::Rows(Float3(1.0f, 2.0f, 3.0f), Float3(4.0f, 5.0f, 6.1f));
	EXPECT_TRUE(matrixA != matrixB);
}

// Friend functions:
TEST(Float2x3, OperatorMultiplicationScalarRight)
{
	Float2x3 matrixA = Float2x3::Rows(Float3(1.0f, 2.0f, 3.0f), Float3(4.0f, 5.0f, 6.0f));
	float scalar = 2.0f;
	Float2x3 mult = matrixA * scalar;
	EXPECT_FLOAT3_EQ(mult.GetRow(0), scalar * matrixA.GetRow(0));
	EXPECT_FLOAT3_EQ(mult.GetRow(1), scalar * matrixA.GetRow(1));
}
TEST(Float2x3, OperatorMultiplicationScalarLeft)
{
	Float2x3 matrixA = Float2x3::Rows(Float3(1.0f, 2.0f, 3.0f), Float3(4.0f, 5.0f, 6.0f));
	float scalar = 2.0f;
	Float2x3 mult = scalar * matrixA;
	EXPECT_FLOAT3_EQ(mult.GetRow(0), scalar * matrixA.GetRow(0));
	EXPECT_FLOAT3_EQ(mult.GetRow(1), scalar * matrixA.GetRow(1));
}
TEST(Float2x3, OperatorMultiplicationFloat2Right)
{
	float a = 1.0f;
	float b = 2.0f;
	float c = 3.0f;
	float d = 4.0f;
	float e = 5.0f;
	float f = 6.0f;
	Float2x3 matrixA = Float2x3::Rows(Float3(a,b,c), Float3(d,e,f));
	Float3 vector = Float3(1.3f, 2.1f, 3.2f);
	Float2 mult = matrixA * vector;
	EXPECT_FLOAT_EQ(mult.x, a * vector.x + b * vector.y + c * vector.z);
	EXPECT_FLOAT_EQ(mult.y, d * vector.x + e * vector.y + f * vector.z);
}
TEST(Float2x3, OperatorMultiplicationFloat2Left)
{
	float a = 1.0f;
	float b = 2.0f;
	float c = 3.0f;
	float d = 4.0f;
	float e = 5.0f;
	float f = 6.0f;
	Float2x3 matrixA = Float2x3::Rows(Float3(a, b, c), Float3(d, e, f));
	Float2 vector = Float2(1.2f, 2.1f);
	Float3 mult = vector * matrixA;
	EXPECT_FLOAT_EQ(mult.x, a * vector.x + d * vector.y);
	EXPECT_FLOAT_EQ(mult.y, b * vector.x + e * vector.y);
	EXPECT_FLOAT_EQ(mult.z, c * vector.x + f * vector.y);
}



#endif // __INCLUDE_GUARD_testFloat2x3_h__