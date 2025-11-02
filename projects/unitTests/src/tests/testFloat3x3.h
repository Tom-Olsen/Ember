#pragma once



// Static constructors:
TEST(Float3x3, ConstructorRowsScalar)
{
	Float3 row0 = Float3(1.0f, 2.0f, 3.0f);
	Float3 row1 = Float3(4.0f, 5.0f, 6.0f);
	Float3 row2 = Float3(7.0f, 8.0f, 9.0f);
	Float3x3 matrix = Float3x3::Rows(row0.x, row0.y, row0.z, row1.x, row1.y, row1.z, row2.x, row2.y, row2.z);
	EXPECT_FLOAT3_EQ(matrix.GetRow(0), row0);
	EXPECT_FLOAT3_EQ(matrix.GetRow(1), row1);
	EXPECT_FLOAT3_EQ(matrix.GetRow(2), row2);
}
TEST(Float3x3, ConstructorRowsVector)
{
	Float3 row0 = Float3(1.0f, 2.0f, 3.0f);
	Float3 row1 = Float3(4.0f, 5.0f, 6.0f);
	Float3 row2 = Float3(7.0f, 8.0f, 9.0f);
	Float3x3 matrix = Float3x3::Rows(row0, row1, row2);
	EXPECT_FLOAT3_EQ(matrix.GetRow(0), row0);
	EXPECT_FLOAT3_EQ(matrix.GetRow(1), row1);
	EXPECT_FLOAT3_EQ(matrix.GetRow(2), row2);
}
TEST(Float3x3, ConstructorColumnsScalar)
{
	Float3 column0 = Float3(1.0f, 2.0f, 3.0f);
	Float3 column1 = Float3(4.0f, 5.0f, 6.0f);
	Float3 column2 = Float3(7.0f, 8.0f, 9.0f);
	Float3x3 matrix = Float3x3::Columns(column0.x, column0.y, column0.z, column1.x, column1.y, column1.z, column2.x, column2.y, column2.z);
	EXPECT_FLOAT3_EQ(matrix.GetColumn(0), column0);
	EXPECT_FLOAT3_EQ(matrix.GetColumn(1), column1);
	EXPECT_FLOAT3_EQ(matrix.GetColumn(2), column2);
}
TEST(Float3x3, ConstructorColumnsVector)
{
	Float3 column0 = Float3(1.0f, 2.0f, 3.0f);
	Float3 column1 = Float3(4.0f, 5.0f, 6.0f);
	Float3 column2 = Float3(7.0f, 8.0f, 9.0f);
	Float3x3 matrix = Float3x3::Columns(column0, column1, column2);
	EXPECT_FLOAT3_EQ(matrix.GetColumn(0), column0);
	EXPECT_FLOAT3_EQ(matrix.GetColumn(1), column1);
	EXPECT_FLOAT3_EQ(matrix.GetColumn(2), column2);
}

// Math operations:
TEST(Float3x3, Transpose)
{
	Float3 row0 = Float3(1.0f, 2.0f, 3.0f);
	Float3 row1 = Float3(4.0f, 5.0f, 6.0f);
	Float3 row2 = Float3(7.0f, 8.0f, 9.0f);
	Float3x3 matrix = Float3x3::Rows(row0, row1, row2);
	Float3x3 matrixT = Float3x3::Columns(row0, row1, row2);
	Float3x3 transpose = matrix.Transpose();
	Float3x3 zero = transpose - matrixT;
	EXPECT_TRUE(zero.IsEpsilonZero());
}
TEST(Float3x3, Determinant)
{
	Float3 row0 = Float3(1.0f, 2.0f, 3.0f);
	Float3 row1 = Float3(4.0f, 5.0f, 6.0f);
	Float3 row2 = Float3(7.0f, 8.0f, 8.0f);
	Float3x3 matrix = Float3x3::Rows(row0, row1, row2);
	float det = matrix.Determinant();
	EXPECT_NEAR(det, 3.0f, epsilon);
}
TEST(Float3x3, Inverse)
{
	Float3 row0 = Float3(1.0f, 2.0f, 3.0f);
	Float3 row1 = Float3(4.0f, 5.0f, 6.0f);
	Float3 row2 = Float3(7.0f, 8.0f, 8.0f);
	Float3x3 matrix = Float3x3::Rows(row0, row1, row2);
	Float3x3 matrixInv = matrix.Inverse();
	Float3x3 zero = matrix * matrixInv - Float3x3::identity;
	EXPECT_TRUE(zero.IsEpsilonZero());
}
TEST(Float3x3, InverseDet)
{
	Float3 row0 = Float3(1.0f, 2.0f, 3.0f);
	Float3 row1 = Float3(4.0f, 5.0f, 6.0f);
	Float3 row2 = Float3(7.0f, 8.0f, 8.0f);
	Float3x3 matrix = Float3x3::Rows(row0, row1, row2);
	float det = matrix.Determinant();
	Float3x3 matrixInv = matrix.Inverse(det);
	Float3x3 zero = matrix * matrixInv - Float3x3::identity;
	EXPECT_TRUE(zero.IsEpsilonZero());
}
TEST(Float3x3, IsEpsilonZero)
{
	Float3x3 zero = Float3x3::zero;
	EXPECT_TRUE(zero.IsEpsilonZero());
}

// Static math operations:
TEST(Float3x3, RotateX)
{
	Float3x3 rotMatrix = Float3x3::RotateX(math::pi2);
	Float3 v0 = Float3(0.0f, 1.0f, 0.0f);
	Float3 v1 = rotMatrix * v0;
	EXPECT_NEAR3(v1, Float3(0.0f, 0.0f, 1.0f), epsilon);
}
TEST(Float3x3, RotateY)
{
	Float3x3 rotMatrix = Float3x3::RotateY(math::pi2);
	Float3 v0 = Float3(1.0f, 0.0f, 0.0f);
	Float3 v1 = rotMatrix * v0;
	EXPECT_NEAR3(v1, Float3(0.0f, 0.0f, -1.0f), epsilon);
}
TEST(Float3x3, RotateZ)
{
	Float3x3 rotMatrix = Float3x3::RotateX(math::pi2);
	Float3 v0 = Float3(0.0f, 0.0f, 1.0f);
	Float3 v1 = rotMatrix * v0;
	EXPECT_NEAR3(v1, Float3(0.0f, -1.0f, 0.0f), epsilon);
}
TEST(Float3x3, RotateAroundAxis)
{
	Float3 axis = Float3(1.0f, 1.0f, 0.0f);
	Float3x3 rotMatrix = Float3x3::Rotate(axis, math::pi);
	Float3 v0 = Float3::right;
	Float3 v1 = rotMatrix * v0;
	EXPECT_NEAR3(v1, Float3::forward, epsilon);
}

//TEST(Float3x3, RotateByEulerAngles)
//{
//	not implemented yet
//}
TEST(Float3x3, RotateFromTo)
{
	Float3 from = Float3(1.0f, 0.0f, 0.0f);
	Float3 to = Float3(0.0f, 1.0f, 0.0f);
	Float3x3 rotMatrix = Float3x3::RotateFromTo(from, to);
	Float3 v0 = Float3(1.0f, 0.0f, 0.0f);
	Float3 v1 = rotMatrix * v0;
	EXPECT_NEAR3(v1, to, epsilon);
}
//TEST(Float3x3, RotateThreeLeg)
//{
//  Wrong, but will need changing once world coordinate system has been changed
//	Float3x3 rotMatrix = Float3x3::RotateThreeLeg(Float3::forward, Float3::right, Float3::up, Float3::backward);
//	Float3 right = rotMatrix * Float3::forward;
//	Float3 backward = rotMatrix * Float3::up;
//	EXPECT_NEAR3(Float3::right, right, epsilon);
//	EXPECT_NEAR3(Float3::backward, backward, epsilon);
//}

// Access:
TEST(Float3x3, Access)
{
	float a = 1.0f;
	float b = 2.0f;
	float c = 3.0f;
	float d = 4.0f;
	float e = 5.0f;
	float f = 6.0f;
	float g = 7.0f;
	float h = 8.0f;
	float i = 9.0f;
	Float3x3 matrix = Float3x3::Rows(a, b, c, d, e, f, g, h, i);
	float A = matrix[{0, 0}];
	float B = matrix[{0, 1}];
	float C = matrix[{0, 2}];
	float D = matrix[{1, 0}];
	float E = matrix[{1, 1}];
	float F = matrix[{1, 2}];
	float G = matrix[{2, 0}];
	float H = matrix[{2, 1}];
	float I = matrix[{2, 2}];
	EXPECT_FLOAT_EQ(A, a);
	EXPECT_FLOAT_EQ(B, b);
	EXPECT_FLOAT_EQ(C, c);
	EXPECT_FLOAT_EQ(D, d);
	EXPECT_FLOAT_EQ(E, e);
	EXPECT_FLOAT_EQ(F, f);
	EXPECT_FLOAT_EQ(G, g);
	EXPECT_FLOAT_EQ(H, h);
	EXPECT_FLOAT_EQ(I, i);
}
TEST(Float3x3, GetRow)
{
	Float3 row0 = Float3(1.0f, 2.0f, 3.0f);
	Float3 row1 = Float3(4.0f, 5.0f, 6.0f);
	Float3 row2 = Float3(7.0f, 8.0f, 9.0f);
	Float3x3 matrix = Float3x3::Rows(row0, row1, row2);
	EXPECT_FLOAT3_EQ(matrix.GetRow(0), row0);
	EXPECT_FLOAT3_EQ(matrix.GetRow(1), row1);
	EXPECT_FLOAT3_EQ(matrix.GetRow(2), row2);
}
TEST(Float3x3, GetColumn)
{
	Float3 column0 = Float3(1.0f, 2.0f, 3.0f);
	Float3 column1 = Float3(4.0f, 5.0f, 6.0f);
	Float3 column2 = Float3(7.0f, 8.0f, 9.0f);
	Float3x3 matrix = Float3x3::Columns(column0, column1, column2);
	EXPECT_FLOAT3_EQ(matrix.GetColumn(0), column0);
	EXPECT_FLOAT3_EQ(matrix.GetColumn(1), column1);
	EXPECT_FLOAT3_EQ(matrix.GetColumn(2), column2);
}

// Assignment:
TEST(Float3x3, Assignment)
{
	Float3 row0 = Float3(1.0f, 2.0f, 3.0f);
	Float3 row1 = Float3(4.0f, 5.0f, 6.0f);
	Float3 row2 = Float3(7.0f, 8.0f, 9.0f);
	Float3x3 matrixA = Float3x3::Rows(row0, row1, row2);
	Float3x3 matrixB = matrixA;
	Float3x3 zero = matrixA - matrixB;
	EXPECT_TRUE(zero.IsEpsilonZero());
}

// Addition:
TEST(Float3x3, OperatorAddition)
{
	Float3 row0A = Float3(1.0f, 2.0f, 3.0f);
	Float3 row1A = Float3(4.0f, 5.0f, 6.0f);
	Float3 row2A = Float3(7.0f, 8.0f, 9.0f);
	Float3 row0B = Float3(10.0f, 11.0f, 12.0f);
	Float3 row1B = Float3(13.0f, 14.0f, 15.0f);
	Float3 row2B = Float3(16.0f, 17.0f, 18.0f);
	Float3x3 matrixA = Float3x3::Rows(row0A, row1A, row2A);
	Float3x3 matrixB = Float3x3::Rows(row0B, row1B, row2B);
	Float3x3 add = matrixA + matrixB;
	Float3 row0Add = row0A + row0B;
	Float3 row1Add = row1A + row1B;
	Float3 row2Add = row2A + row2B;
	EXPECT_FLOAT3_EQ(add.GetRow(0), row0Add);
	EXPECT_FLOAT3_EQ(add.GetRow(1), row1Add);
	EXPECT_FLOAT3_EQ(add.GetRow(2), row2Add);
}
TEST(Float3x3, OperatorAdditionAssignment)
{
	Float3 row0A = Float3(1.0f, 2.0f, 3.0f);
	Float3 row1A = Float3(4.0f, 5.0f, 6.0f);
	Float3 row2A = Float3(7.0f, 8.0f, 9.0f);
	Float3 row0B = Float3(10.0f, 11.0f, 12.0f);
	Float3 row1B = Float3(13.0f, 14.0f, 15.0f);
	Float3 row2B = Float3(16.0f, 17.0f, 18.0f);
	Float3x3 add = Float3x3::Rows(row0A, row1A, row2A);
	Float3x3 matrixB = Float3x3::Rows(row0B, row1B, row2B);
	add += matrixB;
	Float3 row0Add = row0A + row0B;
	Float3 row1Add = row1A + row1B;
	Float3 row2Add = row2A + row2B;
	EXPECT_FLOAT3_EQ(add.GetRow(0), row0Add);
	EXPECT_FLOAT3_EQ(add.GetRow(1), row1Add);
	EXPECT_FLOAT3_EQ(add.GetRow(2), row2Add);
}

// Substraction:
TEST(Float3x3, OperatorSubstraction)
{
	Float3 row0A = Float3(1.0f, 2.0f, 3.0f);
	Float3 row1A = Float3(4.0f, 5.0f, 6.0f);
	Float3 row2A = Float3(7.0f, 8.0f, 9.0f);
	Float3 row0B = Float3(10.0f, 11.0f, 12.0f);
	Float3 row1B = Float3(13.0f, 14.0f, 15.0f);
	Float3 row2B = Float3(16.0f, 17.0f, 18.0f);
	Float3x3 matrixA = Float3x3::Rows(row0A, row1A, row2A);
	Float3x3 matrixB = Float3x3::Rows(row0B, row1B, row2B);
	Float3x3 sub = matrixA - matrixB;
	Float3 row0Sub = row0A - row0B;
	Float3 row1Sub = row1A - row1B;
	Float3 row2Sub = row2A - row2B;
	EXPECT_FLOAT3_EQ(sub.GetRow(0), row0Sub);
	EXPECT_FLOAT3_EQ(sub.GetRow(1), row1Sub);
	EXPECT_FLOAT3_EQ(sub.GetRow(2), row2Sub);
}
TEST(Float3x3, OperatorSubstractionAssignment)
{
	Float3 row0A = Float3(1.0f, 2.0f, 3.0f);
	Float3 row1A = Float3(4.0f, 5.0f, 6.0f);
	Float3 row2A = Float3(7.0f, 8.0f, 9.0f);
	Float3 row0B = Float3(10.0f, 11.0f, 12.0f);
	Float3 row1B = Float3(13.0f, 14.0f, 15.0f);
	Float3 row2B = Float3(16.0f, 17.0f, 18.0f);
	Float3x3 sub = Float3x3::Rows(row0A, row1A, row2A);
	Float3x3 matrixB = Float3x3::Rows(row0B, row1B, row2B);
	sub -= matrixB;
	Float3 row0Sub = row0A - row0B;
	Float3 row1Sub = row1A - row1B;
	Float3 row2Sub = row2A - row2B;
	EXPECT_FLOAT3_EQ(sub.GetRow(0), row0Sub);
	EXPECT_FLOAT3_EQ(sub.GetRow(1), row1Sub);
	EXPECT_FLOAT3_EQ(sub.GetRow(2), row2Sub);
}
TEST(Float3x3, OperatorNegation)
{
	Float3 row0 = Float3(1.0f, 2.0f, 3.0f);
	Float3 row1 = Float3(4.0f, 5.0f, 6.0f);
	Float3 row2 = Float3(7.0f, 8.0f, 9.0f);
	Float3x3 matrix = Float3x3::Rows(row0, row1, row2);
	Float3x3 negation = -matrix;
	EXPECT_FLOAT3_EQ(negation.GetRow(0), -row0);
	EXPECT_FLOAT3_EQ(negation.GetRow(1), -row1);
	EXPECT_FLOAT3_EQ(negation.GetRow(2), -row2);
}

// Multiplication:
TEST(Float3x3, OperatorMultiplication)
{
	Float3x3 matrixA = Float3x3::Rows(Float3(1, 2, 3), Float3(4, 5, 6), Float3(7, 8, 9));
	Float3x3 matrixB = Float3x3::Rows(Float3(9, 8, 7), Float3(6, 5, 4), Float3(3, 2, 1));
	Float3x3 mult = matrixA * matrixB;
	EXPECT_FLOAT3_EQ(mult.GetRow(0), Float3(30, 24, 18));
	EXPECT_FLOAT3_EQ(mult.GetRow(1), Float3(84, 69, 54));
	EXPECT_FLOAT3_EQ(mult.GetRow(2), Float3(138, 114, 90));
}
TEST(Float3x3, OperatorMultiplicationAssignment)
{
	Float3x3 mult = Float3x3::Rows(Float3(1, 2, 3), Float3(4, 5, 6), Float3(7, 8, 9));
	Float3x3 matrixB = Float3x3::Rows(Float3(9, 8, 7), Float3(6, 5, 4), Float3(3, 2, 1));
	mult *= matrixB;
	EXPECT_FLOAT3_EQ(mult.GetRow(0), Float3(30, 24, 18));
	EXPECT_FLOAT3_EQ(mult.GetRow(1), Float3(84, 69, 54));
	EXPECT_FLOAT3_EQ(mult.GetRow(2), Float3(138, 114, 90));
}
TEST(Float3x3, OperatorMultiplicationScalarAssignment)
{
	Float3 row0 = Float3(1.0f, 2.0f, 3.0f);
	Float3 row1 = Float3(4.0f, 5.0f, 6.0f);
	Float3 row2 = Float3(7.0f, 8.0f, 9.0f);
	Float3x3 mult = Float3x3::Rows(row0, row1, row2);
	float scalar = 2.0f;
	mult *= scalar;
	EXPECT_FLOAT3_EQ(mult.GetRow(0), scalar * row0);
	EXPECT_FLOAT3_EQ(mult.GetRow(1), scalar * row1);
	EXPECT_FLOAT3_EQ(mult.GetRow(2), scalar * row2);
}

// Division:
TEST(Float3x3, OperatorDivision)
{
	Float3 row0 = Float3(1.0f, 2.0f, 3.0f);
	Float3 row1 = Float3(4.0f, 5.0f, 6.0f);
	Float3 row2 = Float3(7.0f, 8.0f, 9.0f);
	Float3x3 matrixA = Float3x3::Rows(row0, row1, row2);
	float scalar = 2.0f;
	Float3x3 div = matrixA / scalar;
	Float3 row0Div = row0 / scalar;
	Float3 row1Div = row1 / scalar;
	Float3 row2Div = row2 / scalar;
	EXPECT_FLOAT3_EQ(div.GetRow(0), row0Div);
	EXPECT_FLOAT3_EQ(div.GetRow(1), row1Div);
	EXPECT_FLOAT3_EQ(div.GetRow(2), row2Div);
}
TEST(Float3x3, OperatorDivisionAssignment)
{
	Float3 row0 = Float3(1.0f, 2.0f, 3.0f);
	Float3 row1 = Float3(4.0f, 5.0f, 6.0f);
	Float3 row2 = Float3(7.0f, 8.0f, 9.0f);
	Float3x3 div = Float3x3::Rows(row0, row1, row2);
	float scalar = 2.0f;
	div /= scalar;
	Float3 row0Div = row0 / scalar;
	Float3 row1Div = row1 / scalar;
	Float3 row2Div = row2 / scalar;
	EXPECT_FLOAT3_EQ(div.GetRow(0), row0Div);
	EXPECT_FLOAT3_EQ(div.GetRow(1), row1Div);
	EXPECT_FLOAT3_EQ(div.GetRow(2), row2Div);
}

// Comparison:
TEST(Float3x3, IsEpsilonEqual)
{
	Float3x3 matrixA = Float3x3::Rows(Float3(1.0f, 2.0f, 3.0f), Float3(4.0f, 5.0f, 6.0f), Float3(7.0f, 8.0f, 9.0f));
	Float3x3 matrixB = Float3x3::Rows(Float3(1.0f, 2.0f, 3.0f), Float3(4.0f, 5.0f, 6.0f), Float3(7.0f, 8.0f, 9.0f));
	EXPECT_TRUE(matrixA.IsEpsilonEqual(matrixB));
}
TEST(Float3x3, OperatorEquality)
{
	Float3x3 matrixA = Float3x3::Rows(Float3(1.0f, 2.0f, 3.0f), Float3(4.0f, 5.0f, 6.0f), Float3(7.0f, 8.0f, 9.0f));
	Float3x3 matrixB = Float3x3::Rows(Float3(1.0f, 2.0f, 3.0f), Float3(4.0f, 5.0f, 6.0f), Float3(7.0f, 8.0f, 9.0f));
	EXPECT_TRUE(matrixA == matrixB);
}
TEST(Float3x3, OperatorInequality)
{
	Float3x3 matrixA = Float3x3::Rows(Float3(1.0f, 2.0f, 3.0f), Float3(4.0f, 5.0f, 6.0f), Float3(7.0f, 8.0f, 9.0f));
	Float3x3 matrixB = Float3x3::Rows(Float3(1.0f, 2.0f, 3.0f), Float3(4.0f, 5.0f, 6.0f), Float3(7.0f, 8.0f, 9.1f));
	EXPECT_TRUE(matrixA != matrixB);
}

// Friend functions:
TEST(Float3x3, OperatorMultiplicationScalarRight)
{
	Float3x3 matrix = Float3x3::Rows(Float3(1.0f, 2.0f, 3.0f), Float3(4.0f, 5.0f, 6.0f), Float3(7.0f, 8.0f, 9.0f));
	float scalar = 2.0f;
	Float3x3 mult = matrix * scalar;
	EXPECT_FLOAT3_EQ(mult.GetRow(0), scalar * matrix.GetRow(0));
	EXPECT_FLOAT3_EQ(mult.GetRow(1), scalar * matrix.GetRow(1));
	EXPECT_FLOAT3_EQ(mult.GetRow(2), scalar * matrix.GetRow(2));
}
TEST(Float3x3, OperatorMultiplicationScalarLeft)
{
	Float3x3 matrix = Float3x3::Rows(Float3(1.0f, 2.0f, 3.0f), Float3(4.0f, 5.0f, 6.0f), Float3(7.0f, 8.0f, 9.0f));
	float scalar = 2.0f;
	Float3x3 mult = scalar * matrix;
	EXPECT_FLOAT3_EQ(mult.GetRow(0), scalar * matrix.GetRow(0));
	EXPECT_FLOAT3_EQ(mult.GetRow(1), scalar * matrix.GetRow(1));
	EXPECT_FLOAT3_EQ(mult.GetRow(2), scalar * matrix.GetRow(2));
}
TEST(Float3x3, OperatorMultiplicationFloat3Right)
{
	float m00 = 1.0f;
	float m01 = 2.0f;
	float m02 = 3.0f;
	float m10 = 4.0f;
	float m11 = 5.0f;
	float m12 = 6.0f;
	float m20 = 7.0f;
	float m21 = 8.0f;
	float m22 = 9.0f;
	Float3x3 matrix = Float3x3::Rows(Float3(m00, m01, m02), Float3(m10, m11, m12), Float3(m20, m21, m22));
	Float3 vector = Float3(1.3f, 2.1f, 3.2f);
	Float3 mult = matrix * vector;
	EXPECT_FLOAT_EQ(mult.x, m00 * vector.x + m01 * vector.y + m02 * vector.z);
	EXPECT_FLOAT_EQ(mult.y, m10 * vector.x + m11 * vector.y + m12 * vector.z);
	EXPECT_FLOAT_EQ(mult.z, m20 * vector.x + m21 * vector.y + m22 * vector.z);
}
TEST(Float3x3, OperatorMultiplicationFloat3Left)
{
	float m00 = 1.0f;
	float m01 = 2.0f;
	float m02 = 3.0f;
	float m10 = 4.0f;
	float m11 = 5.0f;
	float m12 = 6.0f;
	float m20 = 7.0f;
	float m21 = 8.0f;
	float m22 = 9.0f;
	Float3x3 matrix = Float3x3::Rows(Float3(m00, m01, m02), Float3(m10, m11, m12), Float3(m20, m21, m22));
	Float3 vector = Float3(1.3f, 2.1f, 3.2f);
	Float3 mult = vector * matrix;
	EXPECT_FLOAT_EQ(mult.x, m00 * vector.x + m10 * vector.y + m20 * vector.z);
	EXPECT_FLOAT_EQ(mult.y, m01 * vector.x + m11 * vector.y + m21 * vector.z);
	EXPECT_FLOAT_EQ(mult.z, m02 * vector.x + m12 * vector.y + m22 * vector.z);
}