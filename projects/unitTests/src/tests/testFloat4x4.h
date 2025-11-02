#pragma once



// Static constructors:
TEST(Float4x4, ConstructorRowsScalar)
{
	Float4 row0 = Float4(1.0f, 2.0f, 3.0f, 4.0f);
	Float4 row1 = Float4(2.0f, 3.0f, 4.0f, 5.0f);
	Float4 row2 = Float4(1.0f, 1.0f, 1.0f, 1.0f);
	Float4 row3 = Float4(4.0f, 3.0f, 2.0f, 1.0f);
	Float4x4 matrix = Float4x4::Rows(
		row0.x, row0.y, row0.z, row0.w,
		row1.x, row1.y, row1.z, row1.w,
		row2.x, row2.y, row2.z, row2.w,
		row3.x, row3.y, row3.z, row3.w);
	EXPECT_FLOAT4_EQ(matrix.GetRow(0), row0);
	EXPECT_FLOAT4_EQ(matrix.GetRow(1), row1);
	EXPECT_FLOAT4_EQ(matrix.GetRow(2), row2);
	EXPECT_FLOAT4_EQ(matrix.GetRow(3), row3);
}
TEST(Float4x4, ConstructorRowsVector)
{
	Float4 row0 = Float4(1.0f, 2.0f, 3.0f, 4.0f);
	Float4 row1 = Float4(2.0f, 3.0f, 4.0f, 5.0f);
	Float4 row2 = Float4(1.0f, 1.0f, 1.0f, 1.0f);
	Float4 row3 = Float4(4.0f, 3.0f, 2.0f, 1.0f);
	Float4x4 matrix = Float4x4::Rows(row0, row1, row2, row3);
	EXPECT_FLOAT4_EQ(matrix.GetRow(0), row0);
	EXPECT_FLOAT4_EQ(matrix.GetRow(1), row1);
	EXPECT_FLOAT4_EQ(matrix.GetRow(2), row2);
	EXPECT_FLOAT4_EQ(matrix.GetRow(3), row3);
}
TEST(Float4x4, ConstructorColumnsScalar)
{
	Float4 column0 = Float4(1.0f, 2.0f, 3.0f, 4.0f);
	Float4 column1 = Float4(2.0f, 3.0f, 4.0f, 5.0f);
	Float4 column2 = Float4(1.0f, 1.0f, 1.0f, 1.0f);
	Float4 column3 = Float4(4.0f, 3.0f, 2.0f, 1.0f);
	Float4x4 matrix = Float4x4::Columns(
		column0.x, column0.y, column0.z, column0.w,
		column1.x, column1.y, column1.z, column1.w,
		column2.x, column2.y, column2.z, column2.w,
		column3.x, column3.y, column3.z, column3.w);
	EXPECT_FLOAT4_EQ(matrix.GetColumn(0), column0);
	EXPECT_FLOAT4_EQ(matrix.GetColumn(1), column1);
	EXPECT_FLOAT4_EQ(matrix.GetColumn(2), column2);
	EXPECT_FLOAT4_EQ(matrix.GetColumn(3), column3);
}
TEST(Float4x4, ConstructorColumnsVector)
{
	Float4 column0 = Float4(1.0f, 2.0f, 3.0f, 4.0f);
	Float4 column1 = Float4(2.0f, 3.0f, 4.0f, 5.0f);
	Float4 column2 = Float4(1.0f, 1.0f, 1.0f, 1.0f);
	Float4 column3 = Float4(4.0f, 3.0f, 2.0f, 1.0f);
	Float4x4 matrix = Float4x4::Columns(column0, column1, column2, column3);
	EXPECT_FLOAT4_EQ(matrix.GetColumn(0), column0);
	EXPECT_FLOAT4_EQ(matrix.GetColumn(1), column1);
	EXPECT_FLOAT4_EQ(matrix.GetColumn(2), column2);
	EXPECT_FLOAT4_EQ(matrix.GetColumn(3), column3);
}

// Math operations:
TEST(Float4x4, Transpose)
{
	Float4 row0 = Float4(1.0f, 2.0f, 3.0f, 4.0f);
	Float4 row1 = Float4(2.0f, 3.0f, 4.0f, 5.0f);
	Float4 row2 = Float4(1.0f, 1.0f, 1.0f, 1.0f);
	Float4 row3 = Float4(4.0f, 3.0f, 2.0f, 1.0f);
	Float4x4 matrix = Float4x4::Rows(row0, row1, row2, row3);
	Float4x4 matrixT = Float4x4::Columns(row0, row1, row2, row3);
	Float4x4 transpose = matrix.Transpose();
	Float4x4 zero = transpose - matrixT;
	EXPECT_TRUE(zero.IsEpsilonZero());
}
TEST(Float4x4, Determinant)
{
	Float4 row0 = Float4(4.0f, 1.0f, 1.0f, 1.0f);
	Float4 row1 = Float4(1.0f, 4.0f, 1.0f, 1.0f);
	Float4 row2 = Float4(1.0f, 1.0f, 4.0f, 1.0f);
	Float4 row3 = Float4(1.0f, 1.0f, 1.0f, 4.0f);
	Float4x4 matrix = Float4x4::Rows(row0, row1, row2, row3);
	float det = matrix.Determinant();
	EXPECT_NEAR(det, 189.0f, epsilon);
}
TEST(Float4x4, Inverse)
{
	Float4 row0 = Float4(4.0f, 1.0f, 1.0f, 1.0f);
	Float4 row1 = Float4(1.0f, 4.0f, 1.0f, 1.0f);
	Float4 row2 = Float4(1.0f, 1.0f, 4.0f, 1.0f);
	Float4 row3 = Float4(1.0f, 1.0f, 1.0f, 4.0f);
	Float4x4 matrix = Float4x4::Rows(row0, row1, row2, row3);
	Float4x4 matrixInv = matrix.Inverse();
	Float4x4 zero = matrix * matrixInv - Float4x4::identity;
	EXPECT_TRUE(zero.IsEpsilonZero());
}
TEST(Float4x4, InverseDet)
{
	Float4 row0 = Float4(4.0f, 1.0f, 1.0f, 1.0f);
	Float4 row1 = Float4(1.0f, 4.0f, 1.0f, 1.0f);
	Float4 row2 = Float4(1.0f, 1.0f, 4.0f, 1.0f);
	Float4 row3 = Float4(1.0f, 1.0f, 1.0f, 4.0f);
	Float4x4 matrix = Float4x4::Rows(row0, row1, row2, row3);
	float det = matrix.Determinant();
	Float4x4 matrixInv = matrix.Inverse(det);
	Float4x4 zero = matrix * matrixInv - Float4x4::identity;
	EXPECT_TRUE(zero.IsEpsilonZero());
}
TEST(Float4x4, IsEpsilonZero)
{
	Float4x4 zero = Float4x4::zero;
	EXPECT_TRUE(zero.IsEpsilonZero());
}

// Static math operations:
TEST(Float4x4, RotateX)
{
	Float4x4 rotMatrix = Float4x4::RotateX(math::pi2);
	Float4 v0 = Float4(0.0f, 1.0f, 0.0f, 1.0f);
	Float4 v1 = rotMatrix * v0;
	EXPECT_NEAR4(v1, Float4(0.0f, 0.0f, 1.0f, 1.0f), epsilon);
}
TEST(Float4x4, RotateY)
{
	Float4x4 rotMatrix = Float4x4::RotateY(math::pi2);
	Float4 v0 = Float4(1.0f, 0.0f, 0.0f, 1.0f);
	Float4 v1 = rotMatrix * v0;
	EXPECT_NEAR4(v1, Float4(0.0f, 0.0f, -1.0f, 1.0f), epsilon);
}
TEST(Float4x4, RotateZ)
{
	Float4x4 rotMatrix = Float4x4::RotateX(math::pi2);
	Float4 v0 = Float4(0.0f, 0.0f, 1.0f, 1.0f);
	Float4 v1 = rotMatrix * v0;
	EXPECT_NEAR4(v1, Float4(0.0f, -1.0f, 0.0f, 1.0f), epsilon);
}
TEST(Float4x4, RotateAroundAxis)
{
	Float3 axis = Float3(1.0f, 1.0f, 0.0f);
	Float4x4 rotMatrix = Float4x4::Rotate(axis, math::pi);
	Float4 v0 = Float4::right;
	Float4 v1 = rotMatrix * v0;
	EXPECT_NEAR4(v1, Float4::forward, epsilon);
}
//TEST(Float4x4, RotateByEulerAngles)
//{
//	// no test implemented yet
//}
TEST(Float4x4, RotateFromTo)
{
	Float3 from = Float3(1.0f, 0.0f, 0.0f);
	Float3 to = Float3(0.0f, 1.0f, 0.0f);
	Float4x4 rotMatrix = Float4x4::RotateFromTo(from, to);
	Float4 v0 = Float4(1.0f, 0.0f, 0.0f, 0.0f);
	Float4 v1 = rotMatrix * v0;
	EXPECT_NEAR4(v1, Float4(to, 0.0f), epsilon);
}
//TEST(Float4x4, RotateThreeLeg)
//{
//	Wrong, but will need changing once world coordinate system has been changed
//	Float4x4 rotMatrix = Float4x4::RotateThreeLeg(Float3::forward, Float3::right, Float3::up, Float3::backward);
//	Float4 right = rotMatrix * Float4::forward;
//	Float4 backward = rotMatrix * Float4::up;
//	EXPECT_NEAR4(Float4::right, right, epsilon);
//	EXPECT_NEAR4(Float4::backward, backward, epsilon);
//}
TEST(Float4x4, Translate)
{
	Float3 translation = Float3(1.0f, 2.0f, 3.0f);
	Float4x4 translateMatrix = Float4x4::Translate(translation);
	Float4 v0 = Float4(1.0f, 1.0f, 1.0f, 1.0f);
	Float4 v1 = translateMatrix * v0;
	EXPECT_NEAR4(v1, Float4(2.0f, 3.0f, 4.0f, 1.0f), epsilon);
}
TEST(Float4x4, ScaleByFloat3)
{
	Float3 scale = Float3(2.0f, 3.0f, 4.0f);
	Float4x4 scaleMatrix = Float4x4::Scale(scale);
	Float4 v0 = Float4(1.0f, 1.0f, 1.0f, 1.0f);
	Float4 v1 = scaleMatrix * v0;
	EXPECT_NEAR4(v1, Float4(2.0f, 3.0f, 4.0f, 1.0f), epsilon);
}
TEST(Float4x4, ScaleByFloat)
{
	float scale = 2.0f;
	Float4x4 scaleMatrix = Float4x4::Scale(scale);
	Float4 v0 = Float4(1.0f, 1.0f, 1.0f, 1.0f);
	Float4 v1 = scaleMatrix * v0;
	EXPECT_NEAR4(v1, Float4(2.0f, 2.0f, 2.0f, 1.0f), epsilon);
}
TEST(Float4x4, TRSFloat3x3)
{
	Float3 position = Float3(1.0f, 2.0f, 3.0f);
	Float3x3 rotationMatrix = Float3x3::RotateY(math::pi2);
	Float3 scale = Float3(2.0f, 3.0f, 4.0f);
	Float4x4 trsMatrix = Float4x4::TRS(position, rotationMatrix, scale);
	Float4 v0 = Float4(1.0f, 1.0f, 1.0f, 1.0f);
	Float4 v1 = trsMatrix * v0;
	Float4 v2 = Float4x4::Translate(position) * Float4x4::RotateY(math::pi2) * Float4x4::Scale(scale) * v0;
	EXPECT_NEAR4(v1, v2, epsilon);
}
TEST(Float4x4, TRSFloat4x4)
{
	Float3 position = Float3(1.0f, 2.0f, 3.0f);
	Float4x4 rotationMatrix = Float4x4::RotateY(math::pi2);
	Float3 scale = Float3(2.0f, 3.0f, 4.0f);
	Float4x4 trsMatrix = Float4x4::TRS(position, rotationMatrix, scale);
	Float4 v0 = Float4(1.0f, 1.0f, 1.0f, 1.0f);
	Float4 v1 = trsMatrix * v0;
	Float4 v2 = Float4x4::Translate(position) * Float4x4::RotateY(math::pi2) * Float4x4::Scale(scale) * v0;
	EXPECT_NEAR4(v1, v2, epsilon);
}
TEST(Float4x4, Perspective)
{
	float fov = math::pi2;
	float aspectRatio = 1920.0f / 1080.0f;
	float nearClip = 1.0f;
	float farClip = 10.0f;
	Float4x4 projectionMatrix = Float4x4::Perspective(fov, aspectRatio, nearClip, farClip);

	float xNear = aspectRatio * nearClip * math::Tan(fov / 2.0f);
	float yNear = xNear / aspectRatio;
	float xFar = aspectRatio * farClip * math::Tan(fov / 2.0f);
	float yFar = xFar / aspectRatio;

	Float3 cornersWorld[8] =
	{
		Float3(-xNear,  yNear, -nearClip),
		Float3( xNear,  yNear, -nearClip),
		Float3(-xNear, -yNear, -nearClip),
		Float3( xNear, -yNear, -nearClip),
		Float3( -xFar,   yFar, -farClip ),
		Float3(  xFar,   yFar, -farClip ),
		Float3( -xFar,  -yFar, -farClip ),
		Float3(  xFar,  -yFar, -farClip )
	};
	Float3 ndc[8] =
	{
		Float3(-1.0f, -1.0f, 0.0f),
		Float3( 1.0f, -1.0f, 0.0f),
		Float3(-1.0f,  1.0f, 0.0f),
		Float3( 1.0f,  1.0f, 0.0f),
		Float3(-1.0f, -1.0f, 1.0f),
		Float3( 1.0f, -1.0f, 1.0f),
		Float3(-1.0f,  1.0f, 1.0f),
		Float3( 1.0f,  1.0f, 1.0f)
	};

	for (int i = 0; i < 8; i++)
	{
		Float4 cornerClip = projectionMatrix * Float4(cornersWorld[i], 1.0f);
		cornerClip /= cornerClip.w;
		EXPECT_NEAR3(Float3(cornerClip), ndc[i], epsilon);
		//std::cout << cornersWorld[i].ToString() << " -> " << Float3(cornerClip).ToString() << " ?= " << ndc[i].ToString() << std::endl;
	}
}
TEST(Float4x4, Orthographic)
{
	float left = -1.0f;
	float right = 1.0f;
	float bottom = -1.0f;
	float top = 1.0f;
	float nearClip = 1.0f;
	float farClip = 10.0f;
	Float4x4 projectionMatrix = Float4x4::Orthographic(left, right, bottom, top, nearClip, farClip);

	Float3 cornersWorld[8] =
	{
		Float3( left,    top, -nearClip),
		Float3(right,    top, -nearClip),
		Float3( left, bottom, -nearClip),
		Float3(right, bottom, -nearClip),
		Float3( left,    top, -farClip),
		Float3(right,    top, -farClip),
		Float3( left, bottom, -farClip),
		Float3(right, bottom, -farClip)
	};
	Float3 ndc[8] =
	{
		Float3(-1.0f, -1.0f, 0.0f),
		Float3( 1.0f, -1.0f, 0.0f),
		Float3(-1.0f,  1.0f, 0.0f),
		Float3( 1.0f,  1.0f, 0.0f),
		Float3(-1.0f, -1.0f, 1.0f),
		Float3( 1.0f, -1.0f, 1.0f),
		Float3(-1.0f,  1.0f, 1.0f),
		Float3( 1.0f,  1.0f, 1.0f)
	};

	for (int i = 0; i < 8; i++)
	{
		Float4 cornerClip = projectionMatrix * Float4(cornersWorld[i], 1.0f);
		cornerClip /= cornerClip.w;
		EXPECT_NEAR3(Float3(cornerClip), ndc[i], epsilon);
		//std::cout << cornersWorld[i].ToString() << " -> " << Float3(cornerClip).ToString() << " ?= " << ndc[i].ToString() << std::endl;
	}
}


// Access:
TEST(Float4x4, Access)
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
	float j = 10.0f;
	float k = 11.0f;
	float l = 12.0f;
	float m = 13.0f;
	float n = 14.0f;
	float o = 15.0f;
	float p = 16.0f;
	Float4x4 matrix = Float4x4::Rows(a,b,c,d, e,f,g,h, i,j,k,l, m,n,o,p);
	float A = matrix[{0, 0}];
	float B = matrix[{0, 1}];
	float C = matrix[{0, 2}];
	float D = matrix[{0, 3}];
	float E = matrix[{1, 0}];
	float F = matrix[{1, 1}];
	float G = matrix[{1, 2}];
	float H = matrix[{1, 3}];
	float I = matrix[{2, 0}];
	float J = matrix[{2, 1}];
	float K = matrix[{2, 2}];
	float L = matrix[{2, 3}];
	float M = matrix[{3, 0}];
	float N = matrix[{3, 1}];
	float O = matrix[{3, 2}];
	float P = matrix[{3, 3}];
	EXPECT_FLOAT_EQ(A, a);
	EXPECT_FLOAT_EQ(B, b);
	EXPECT_FLOAT_EQ(C, c);
	EXPECT_FLOAT_EQ(D, d);
	EXPECT_FLOAT_EQ(E, e);
	EXPECT_FLOAT_EQ(F, f);
	EXPECT_FLOAT_EQ(G, g);
	EXPECT_FLOAT_EQ(H, h);
	EXPECT_FLOAT_EQ(I, i);
	EXPECT_FLOAT_EQ(J, j);
	EXPECT_FLOAT_EQ(K, k);
	EXPECT_FLOAT_EQ(L, l);
	EXPECT_FLOAT_EQ(M, m);
	EXPECT_FLOAT_EQ(N, n);
	EXPECT_FLOAT_EQ(O, o);
	EXPECT_FLOAT_EQ(P, p);
}
TEST(Float4x4, GetRow)
{
	Float4 row0 = Float4(1.0f, 2.0f, 3.0f, 4.0f);
	Float4 row1 = Float4(2.0f, 3.0f, 4.0f, 5.0f);
	Float4 row2 = Float4(1.0f, 1.0f, 1.0f, 1.0f);
	Float4 row3 = Float4(4.0f, 3.0f, 2.0f, 1.0f);
	Float4x4 matrix = Float4x4::Rows(row0, row1, row2, row3);
	EXPECT_FLOAT4_EQ(matrix.GetRow(0), row0);
	EXPECT_FLOAT4_EQ(matrix.GetRow(1), row1);
	EXPECT_FLOAT4_EQ(matrix.GetRow(2), row2);
	EXPECT_FLOAT4_EQ(matrix.GetRow(3), row3);
}
TEST(Float4x4, GetColumn)
{
	Float4 column0 = Float4(1.0f, 2.0f, 3.0f, 4.0f);
	Float4 column1 = Float4(2.0f, 3.0f, 4.0f, 5.0f);
	Float4 column2 = Float4(1.0f, 1.0f, 1.0f, 1.0f);
	Float4 column3 = Float4(4.0f, 3.0f, 2.0f, 1.0f);
	Float4x4 matrix = Float4x4::Columns(column0, column1, column2, column3);
	EXPECT_FLOAT4_EQ(matrix.GetColumn(0), column0);
	EXPECT_FLOAT4_EQ(matrix.GetColumn(1), column1);
	EXPECT_FLOAT4_EQ(matrix.GetColumn(2), column2);
	EXPECT_FLOAT4_EQ(matrix.GetColumn(3), column3);
}
TEST(Float4x4, GetTranslation)
{
	Float3 position = Float3(1.0f, 2.0f, 3.0f);
	Float4x4 rotationMatrix = Float4x4::RotateY(math::pi2);
	Float3 scale = Float3(2.0f, 3.0f, 4.0f);
	Float4x4 trsMatrix = Float4x4::TRS(position, rotationMatrix, scale);
	Float3 translationResult = trsMatrix.GetTranslation();
	EXPECT_NEAR3(translationResult, position, epsilon);
}
TEST(Float4x4, GetScale)
{
	Float3 position = Float3(1.0f, 2.0f, 3.0f);
	Float4x4 rotationMatrix = Float4x4::RotateY(math::pi2);
	Float3 scale = Float3(2.0f, 3.0f, 4.0f);
	Float4x4 trsMatrix = Float4x4::TRS(position, rotationMatrix, scale);
	Float3 scaleResult = trsMatrix.GetScale();
	EXPECT_NEAR3(scaleResult, scale, epsilon);
}
TEST(Float4x4, GetRotation3x3)
{
	Float3 position = Float3(1.0f, 2.0f, 3.0f);
	Float4x4 rotationMatrix = Float4x4::RotateY(math::pi2);
	Float3 scale = Float3(2.0f, 3.0f, 4.0f);
	Float4x4 trsMatrix = Float4x4::TRS(position, rotationMatrix, scale);
	Float3x3 rotationResult = trsMatrix.GetRotation3x3();
	EXPECT_TRUE(rotationResult.IsEpsilonEqual(Float3x3(rotationMatrix)));
}
TEST(Float4x4, GetRotation4x4)
{
	Float3 position = Float3(1.0f, 2.0f, 3.0f);
	Float4x4 rotationMatrix = Float4x4::RotateY(math::pi2);
	Float3 scale = Float3(2.0f, 3.0f, 4.0f);
	Float4x4 trsMatrix = Float4x4::TRS(position, rotationMatrix, scale);
	Float4x4 rotationResult = trsMatrix.GetRotation4x4();
	EXPECT_TRUE(rotationResult.IsEpsilonEqual(rotationMatrix));
}

// Assignment:
TEST(Float4x4, Assignment)
{
	Float4 row0 = Float4(1.0f, 2.0f, 3.0f, 4.0f);
	Float4 row1 = Float4(2.0f, 3.0f, 4.0f, 5.0f);
	Float4 row2 = Float4(1.0f, 1.0f, 1.0f, 1.0f);
	Float4 row3 = Float4(4.0f, 3.0f, 2.0f, 1.0f);
	Float4x4 matrixA = Float4x4::Rows(row0, row1, row2, row3);
	Float4x4 matrixB = matrixA;
	Float4x4 zero = matrixA - matrixB;
	EXPECT_TRUE(zero.IsEpsilonZero());
}

// Addition:
TEST(Float4x4, OperatorAddition)
{
	Float4 row0A = Float4(1.0f, 2.0f, 3.0f, 4.0f);
	Float4 row1A = Float4(2.0f, 3.0f, 4.0f, 5.0f);
	Float4 row2A = Float4(1.0f, 1.0f, 1.0f, 1.0f);
	Float4 row3A = Float4(4.0f, 3.0f, 2.0f, 1.0f);
	Float4 row0B = Float4(1.0f, 3.0f, 5.0f, 4.0f);
	Float4 row1B = Float4(2.0f, 1.0f, 2.0f, 2.0f);
	Float4 row2B = Float4(3.0f, 4.0f, 6.0f, 3.0f);
	Float4 row3B = Float4(4.0f, 2.0f, 8.0f, 7.0f);
	Float4x4 matrixA = Float4x4::Rows(row0A, row1A, row2A, row3A);
	Float4x4 matrixB = Float4x4::Rows(row0B, row1B, row2B, row3B);
	Float4x4 add = matrixA + matrixB;
	Float4 row0Add = row0A + row0B;
	Float4 row1Add = row1A + row1B;
	Float4 row2Add = row2A + row2B;
	Float4 row3Add = row3A + row3B;
	EXPECT_FLOAT4_EQ(add.GetRow(0), row0Add);
	EXPECT_FLOAT4_EQ(add.GetRow(1), row1Add);
	EXPECT_FLOAT4_EQ(add.GetRow(2), row2Add);
	EXPECT_FLOAT4_EQ(add.GetRow(3), row3Add);
}
TEST(Float4x4, OperatorAdditionAssignment)
{
	Float4 row0A = Float4(1.0f, 2.0f, 3.0f, 4.0f);
	Float4 row1A = Float4(2.0f, 3.0f, 4.0f, 5.0f);
	Float4 row2A = Float4(1.0f, 1.0f, 1.0f, 1.0f);
	Float4 row3A = Float4(4.0f, 3.0f, 2.0f, 1.0f);
	Float4 row0B = Float4(1.0f, 3.0f, 5.0f, 4.0f);
	Float4 row1B = Float4(2.0f, 1.0f, 2.0f, 2.0f);
	Float4 row2B = Float4(3.0f, 4.0f, 6.0f, 3.0f);
	Float4 row3B = Float4(4.0f, 2.0f, 8.0f, 7.0f);
	Float4x4 add = Float4x4::Rows(row0A, row1A, row2A, row3A);
	Float4x4 matrixB = Float4x4::Rows(row0B, row1B, row2B, row3B);
	add += matrixB;
	Float4 row0Add = row0A + row0B;
	Float4 row1Add = row1A + row1B;
	Float4 row2Add = row2A + row2B;
	Float4 row3Add = row3A + row3B;
	EXPECT_FLOAT4_EQ(add.GetRow(0), row0Add);
	EXPECT_FLOAT4_EQ(add.GetRow(1), row1Add);
	EXPECT_FLOAT4_EQ(add.GetRow(2), row2Add);
	EXPECT_FLOAT4_EQ(add.GetRow(3), row3Add);
}

// Substraction:
TEST(Float4x4, OperatorSubstraction)
{
	Float4 row0A = Float4(1.0f, 2.0f, 3.0f, 4.0f);
	Float4 row1A = Float4(2.0f, 3.0f, 4.0f, 5.0f);
	Float4 row2A = Float4(1.0f, 1.0f, 1.0f, 1.0f);
	Float4 row3A = Float4(4.0f, 3.0f, 2.0f, 1.0f);
	Float4 row0B = Float4(1.0f, 3.0f, 5.0f, 4.0f);
	Float4 row1B = Float4(2.0f, 1.0f, 2.0f, 2.0f);
	Float4 row2B = Float4(3.0f, 4.0f, 6.0f, 3.0f);
	Float4 row3B = Float4(4.0f, 2.0f, 8.0f, 7.0f);
	Float4x4 matrixA = Float4x4::Rows(row0A, row1A, row2A, row3A);
	Float4x4 matrixB = Float4x4::Rows(row0B, row1B, row2B, row3B);
	Float4x4 sub = matrixA - matrixB;
	Float4 row0Sub = row0A - row0B;
	Float4 row1Sub = row1A - row1B;
	Float4 row2Sub = row2A - row2B;
	Float4 row3Sub = row3A - row3B;
	EXPECT_FLOAT4_EQ(sub.GetRow(0), row0Sub);
	EXPECT_FLOAT4_EQ(sub.GetRow(1), row1Sub);
	EXPECT_FLOAT4_EQ(sub.GetRow(2), row2Sub);
	EXPECT_FLOAT4_EQ(sub.GetRow(3), row3Sub);
}
TEST(Float4x4, OperatorSubstractionAssignment)
{
	Float4 row0A = Float4(1.0f, 2.0f, 3.0f, 4.0f);
	Float4 row1A = Float4(2.0f, 3.0f, 4.0f, 5.0f);
	Float4 row2A = Float4(1.0f, 1.0f, 1.0f, 1.0f);
	Float4 row3A = Float4(4.0f, 3.0f, 2.0f, 1.0f);
	Float4 row0B = Float4(1.0f, 3.0f, 5.0f, 4.0f);
	Float4 row1B = Float4(2.0f, 1.0f, 2.0f, 2.0f);
	Float4 row2B = Float4(3.0f, 4.0f, 6.0f, 3.0f);
	Float4 row3B = Float4(4.0f, 2.0f, 8.0f, 7.0f);
	Float4x4 sub = Float4x4::Rows(row0A, row1A, row2A, row3A);
	Float4x4 matrixB = Float4x4::Rows(row0B, row1B, row2B, row3B);
	sub -= matrixB;
	Float4 row0Sub = row0A - row0B;
	Float4 row1Sub = row1A - row1B;
	Float4 row2Sub = row2A - row2B;
	Float4 row3Sub = row3A - row3B;
	EXPECT_FLOAT4_EQ(sub.GetRow(0), row0Sub);
	EXPECT_FLOAT4_EQ(sub.GetRow(1), row1Sub);
	EXPECT_FLOAT4_EQ(sub.GetRow(2), row2Sub);
	EXPECT_FLOAT4_EQ(sub.GetRow(3), row3Sub);
}
TEST(Float4x4, OperatorNegation)
{
	Float4 row0 = Float4(1.0f, 2.0f, 3.0f, 4.0f);
	Float4 row1 = Float4(2.0f, 3.0f, 4.0f, 5.0f);
	Float4 row2 = Float4(1.0f, 1.0f, 1.0f, 1.0f);
	Float4 row3 = Float4(4.0f, 3.0f, 2.0f, 1.0f);
	Float4x4 matrix = Float4x4::Rows(row0, row1, row2, row3);
	Float4x4 negation = -matrix;
	EXPECT_FLOAT4_EQ(negation.GetRow(0), -row0);
	EXPECT_FLOAT4_EQ(negation.GetRow(1), -row1);
	EXPECT_FLOAT4_EQ(negation.GetRow(2), -row2);
	EXPECT_FLOAT4_EQ(negation.GetRow(3), -row3);
}

// Multiplication:
TEST(Float4x4, OperatorMultiplication)
{
	Float4x4 matrixA = Float4x4::Rows(Float4(1, 2, 3, 4), Float4(4, 5, 6, 7), Float4(7, 8, 9, 1), Float4(2, 3, 4, 5));
	Float4x4 matrixB = Float4x4::Rows(Float4(4, 3, 2, 1), Float4(4, 3, 2, 1), Float4(4, 3, 2, 1), Float4(4, 3, 2, 1));
	Float4x4 mult = matrixA * matrixB;
	EXPECT_FLOAT4_EQ(mult.GetRow(0), Float4(40, 30, 20, 10));
	EXPECT_FLOAT4_EQ(mult.GetRow(1), Float4(88, 66, 44, 22));
	EXPECT_FLOAT4_EQ(mult.GetRow(2), Float4(100, 75, 50, 25));
	EXPECT_FLOAT4_EQ(mult.GetRow(3), Float4(56, 42, 28, 14));
}
TEST(Float4x4, OperatorMultiplicationAssignment)
{
	Float4x4 mult = Float4x4::Rows(Float4(1, 2, 3, 4), Float4(4, 5, 6, 7), Float4(7, 8, 9, 1), Float4(2, 3, 4, 5));
	Float4x4 matrixB = Float4x4::Rows(Float4(4, 3, 2, 1), Float4(4, 3, 2, 1), Float4(4, 3, 2, 1), Float4(4, 3, 2, 1));
	mult *= matrixB;
	EXPECT_FLOAT4_EQ(mult.GetRow(0), Float4(40, 30, 20, 10));
	EXPECT_FLOAT4_EQ(mult.GetRow(1), Float4(88, 66, 44, 22));
	EXPECT_FLOAT4_EQ(mult.GetRow(2), Float4(100, 75, 50, 25));
	EXPECT_FLOAT4_EQ(mult.GetRow(3), Float4(56, 42, 28, 14));
}
TEST(Float4x4, OperatorMultiplicationScalarAssignment)
{
	Float4 row0 = Float4(1.0f, 2.0f, 3.0f, 4.0f);
	Float4 row1 = Float4(2.0f, 3.0f, 4.0f, 5.0f);
	Float4 row2 = Float4(1.0f, 1.0f, 1.0f, 1.0f);
	Float4 row3 = Float4(4.0f, 3.0f, 2.0f, 1.0f);
	Float4x4 mult = Float4x4::Rows(row0, row1, row2, row3);
	float scalar = 2.0f;
	mult *= scalar;
	EXPECT_FLOAT4_EQ(mult.GetRow(0), scalar * row0);
	EXPECT_FLOAT4_EQ(mult.GetRow(1), scalar * row1);
	EXPECT_FLOAT4_EQ(mult.GetRow(2), scalar * row2);
	EXPECT_FLOAT4_EQ(mult.GetRow(3), scalar * row3);
}

// Division:
TEST(Float4x4, OperatorDivision)
{
	Float4 row0 = Float4(1.0f, 2.0f, 3.0f, 4.0f);
	Float4 row1 = Float4(2.0f, 3.0f, 4.0f, 5.0f);
	Float4 row2 = Float4(1.0f, 1.0f, 1.0f, 1.0f);
	Float4 row3 = Float4(4.0f, 3.0f, 2.0f, 1.0f);
	Float4x4 matrix = Float4x4::Rows(row0, row1, row2, row3);
	float scalar = 2.0f;
	Float4x4 div = matrix / scalar;
	Float4 row0Div = row0 / scalar;
	Float4 row1Div = row1 / scalar;
	Float4 row2Div = row2 / scalar;
	Float4 row3Div = row3 / scalar;
	EXPECT_FLOAT4_EQ(div.GetRow(0), row0Div);
	EXPECT_FLOAT4_EQ(div.GetRow(1), row1Div);
	EXPECT_FLOAT4_EQ(div.GetRow(2), row2Div);
	EXPECT_FLOAT4_EQ(div.GetRow(3), row3Div);
}
TEST(Float4x4, OperatorDivisionAssignment)
{
	Float4 row0 = Float4(1.0f, 2.0f, 3.0f, 4.0f);
	Float4 row1 = Float4(2.0f, 3.0f, 4.0f, 5.0f);
	Float4 row2 = Float4(1.0f, 1.0f, 1.0f, 1.0f);
	Float4 row3 = Float4(4.0f, 3.0f, 2.0f, 1.0f);
	Float4x4 div = Float4x4::Rows(row0, row1, row2, row3);
	float scalar = 2.0f;
	div /= scalar;
	Float4 row0Div = row0 / scalar;
	Float4 row1Div = row1 / scalar;
	Float4 row2Div = row2 / scalar;
	Float4 row3Div = row3 / scalar;
	EXPECT_FLOAT4_EQ(div.GetRow(0), row0Div);
	EXPECT_FLOAT4_EQ(div.GetRow(1), row1Div);
	EXPECT_FLOAT4_EQ(div.GetRow(2), row2Div);
	EXPECT_FLOAT4_EQ(div.GetRow(3), row3Div);
}

// Comparison:
TEST(Float4x4, IsEpsilonEqual)
{
	Float4x4 matrixA = Float4x4::Rows(Float4(1, 2, 3, 4), Float4(4, 5, 6, 7), Float4(7, 8, 9, 1), Float4(2, 3, 4, 5));
	Float4x4 matrixB = Float4x4::Rows(Float4(1, 2, 3, 4), Float4(4, 5, 6, 7), Float4(7, 8, 9, 1), Float4(2, 3, 4, 5));
	EXPECT_TRUE(matrixA.IsEpsilonEqual(matrixB));
}
TEST(Float4x4, OperatorEquality)
{
	Float4x4 matrixA = Float4x4::Rows(Float4(1, 2, 3, 4), Float4(4, 5, 6, 7), Float4(7, 8, 9, 1), Float4(2, 3, 4, 5));
	Float4x4 matrixB = Float4x4::Rows(Float4(1, 2, 3, 4), Float4(4, 5, 6, 7), Float4(7, 8, 9, 1), Float4(2, 3, 4, 5));
	EXPECT_TRUE(matrixA == matrixB);
}
TEST(Float4x4, OperatorInequality)
{
	Float4x4 matrixA = Float4x4::Rows(Float4(1, 2, 3, 4), Float4(4, 5, 6, 7), Float4(7, 8, 9, 1), Float4(2, 3, 4, 5));
	Float4x4 matrixB = Float4x4::Rows(Float4(1, 2, 3, 4), Float4(4, 5, 6, 7), Float4(7, 8, 9, 1), Float4(2, 3, 4, 5.1f));
	EXPECT_TRUE(matrixA != matrixB);
}

// Friend functions:
TEST(Float4x4, OperatorMultiplicationScalarRight)
{
	Float4x4 matrix = Float4x4::Rows(Float4(1, 2, 3, 4), Float4(4, 5, 6, 7), Float4(7, 8, 9, 1), Float4(2, 3, 4, 5));
	float scalar = 2.0f;
	Float4x4 mult = matrix * scalar;
	EXPECT_FLOAT4_EQ(mult.GetRow(0), scalar * matrix.GetRow(0));
	EXPECT_FLOAT4_EQ(mult.GetRow(1), scalar * matrix.GetRow(1));
	EXPECT_FLOAT4_EQ(mult.GetRow(2), scalar * matrix.GetRow(2));
	EXPECT_FLOAT4_EQ(mult.GetRow(3), scalar * matrix.GetRow(3));
}
TEST(Float4x4, OperatorMultiplicationScalarLeft)
{
	Float4x4 matrix = Float4x4::Rows(Float4(1, 2, 3, 4), Float4(4, 5, 6, 7), Float4(7, 8, 9, 1), Float4(2, 3, 4, 5));
	float scalar = 2.0f;
	Float4x4 mult = scalar * matrix;
	EXPECT_FLOAT4_EQ(mult.GetRow(0), scalar * matrix.GetRow(0));
	EXPECT_FLOAT4_EQ(mult.GetRow(1), scalar * matrix.GetRow(1));
	EXPECT_FLOAT4_EQ(mult.GetRow(2), scalar * matrix.GetRow(2));
	EXPECT_FLOAT4_EQ(mult.GetRow(3), scalar * matrix.GetRow(3));
}
TEST(Float4x4, OperatorMultiplicationFloat4Right)
{
	float m00 = 1.0f;
	float m01 = 2.0f;
	float m02 = 3.0f;
	float m03 = 4.0f;
	float m10 = 5.0f;
	float m11 = 6.0f;
	float m12 = 7.0f;
	float m13 = 8.0f;
	float m20 = 9.0f;
	float m21 = 10.0f;
	float m22 = 11.0f;
	float m23 = 12.0f;
	float m30 = 13.0f;
	float m31 = 14.0f;
	float m32 = 15.0f;
	float m33 = 16.0f;
	Float4x4 matrix = Float4x4::Rows(Float4(m00, m01, m02, m03), Float4(m10, m11, m12, m13), Float4(m20, m21, m22, m23), Float4(m30, m31, m32, m33));
	Float4 vector = Float4(1.4f, 2.1f, 3.2f, 4.1f);
	Float4 mult = matrix * vector;
	EXPECT_FLOAT_EQ(mult.x, m00 * vector.x + m01 * vector.y + m02 * vector.z + m03 * vector.w);
	EXPECT_FLOAT_EQ(mult.y, m10 * vector.x + m11 * vector.y + m12 * vector.z + m13 * vector.w);
	EXPECT_FLOAT_EQ(mult.z, m20 * vector.x + m21 * vector.y + m22 * vector.z + m23 * vector.w);
	EXPECT_FLOAT_EQ(mult.w, m30 * vector.x + m31 * vector.y + m32 * vector.z + m33 * vector.w);
}
TEST(Float4x4, OperatorMultiplicationFloat4Left)
{
	float m00 = 1.0f;
	float m01 = 2.0f;
	float m02 = 3.0f;
	float m03 = 4.0f;
	float m10 = 5.0f;
	float m11 = 6.0f;
	float m12 = 7.0f;
	float m13 = 8.0f;
	float m20 = 9.0f;
	float m21 = 10.0f;
	float m22 = 11.0f;
	float m23 = 12.0f;
	float m30 = 13.0f;
	float m31 = 14.0f;
	float m32 = 15.0f;
	float m33 = 16.0f;
	Float4x4 matrix = Float4x4::Rows(Float4(m00, m01, m02, m03), Float4(m10, m11, m12, m13), Float4(m20, m21, m22, m23), Float4(m30, m31, m32, m33));
	Float4 vector = Float4(1.4f, 2.1f, 3.2f, 4.1f);
	Float4 mult = vector * matrix;
	EXPECT_FLOAT_EQ(mult.x, m00 * vector.x + m10 * vector.y + m20 * vector.z + m30 * vector.w);
	EXPECT_FLOAT_EQ(mult.y, m01 * vector.x + m11 * vector.y + m21 * vector.z + m31 * vector.w);
	EXPECT_FLOAT_EQ(mult.z, m02 * vector.x + m12 * vector.y + m22 * vector.z + m32 * vector.w);
	EXPECT_FLOAT_EQ(mult.w, m03 * vector.x + m13 * vector.y + m23 * vector.z + m33 * vector.w);
}