#pragma once



// Basic math:
TEST(Mathf, Abs)
{
	EXPECT_FLOAT_EQ(math::Abs( 0.0f), 0.0f);
	EXPECT_FLOAT_EQ(math::Abs(-0.0f), 0.0f);
	EXPECT_FLOAT_EQ(math::Abs( 1.0f), 1.0f);
	EXPECT_FLOAT_EQ(math::Abs(-1.0f), 1.0f);
}
TEST(Mathf, Clamp)
{
	float min = 0.5f;
	float max = 1.5f;
	EXPECT_NEAR(math::Clamp(0.0f, min, max), 0.5f, epsilon);
	EXPECT_NEAR(math::Clamp(1.0f, min, max), 1.0f, epsilon);
	EXPECT_NEAR(math::Clamp(2.0f, min, max), 1.5f, epsilon);
}
TEST(Mathf, Max)
{
	EXPECT_NEAR(math::Max(0.0f, 1.0f), 1.0f, epsilon);
	EXPECT_NEAR(math::Max(1.0f, 0.0f), 1.0f, epsilon);
}
TEST(Mathf, Min)
{
	EXPECT_NEAR(math::Min(0.0f, 1.0f), 0.0f, epsilon);
	EXPECT_NEAR(math::Min(1.0f, 0.0f), 0.0f, epsilon);
}
TEST(Mathf, Sign)
{
	EXPECT_FLOAT_EQ(math::Sign(0.0f), 0.0f);
	EXPECT_FLOAT_EQ(math::Sign(-0.0f), 0.0f);
	EXPECT_FLOAT_EQ(math::Sign(4.0f), 1.0f);
	EXPECT_FLOAT_EQ(math::Sign(-4.0f), -1.0f);
}
TEST(Mathf, Sqrt)
{
	EXPECT_NEAR(math::Sqrt(0.0f), 0.0f, epsilon);
	EXPECT_NEAR(math::Sqrt(1.0f), 1.0f, epsilon);
	EXPECT_NEAR(math::Sqrt(4.0f), 2.0f, epsilon);
	EXPECT_NEAR(math::Sqrt(9.0f), 3.0f, epsilon);
	EXPECT_TRUE(std::isnan(math::Sqrt(-9.0f)));
}
TEST(Mathf, Factorial)
{
	int factorial = 1;
	for (int i = 0; i < 10; i++)
	{
		if (i > 0)
			factorial *= i;
		EXPECT_EQ(math::Factorial(i), factorial);
	}
}

// Trigonometry:
TEST(Mathf, Sin)
{
	EXPECT_NEAR(math::Sin(0.0f), 0.0f, epsilon);
	EXPECT_NEAR(math::Sin(math::pi2), 1.0f, epsilon);
	EXPECT_NEAR(math::Sin(math::pi), 0.0f, epsilon);
	EXPECT_NEAR(math::Sin(3.0f * math::pi2), -1.0f, epsilon);
	EXPECT_NEAR(math::Sin(2.0f * math::pi), 0.0f, epsilon);
}
TEST(Mathf, Cos)
{
	EXPECT_NEAR(math::Cos(0.0f), 1.0f, epsilon);
	EXPECT_NEAR(math::Cos(math::pi2), 0.0f, epsilon);
	EXPECT_NEAR(math::Cos(math::pi), -1.0f, epsilon);
	EXPECT_NEAR(math::Cos(3.0f * math::pi2), 0.0f, epsilon);
	EXPECT_NEAR(math::Cos(2.0f * math::pi), 1.0f, epsilon);
}
TEST(Mathf, Tan)
{
	EXPECT_NEAR(math::Tan(0.0f), 0.0f, epsilon);
	EXPECT_NEAR(math::Tan(math::pi4), 1.0f, epsilon);
	EXPECT_NEAR(math::Tan(-math::pi4), -1.0f, epsilon);
	EXPECT_NEAR(math::Tan(3.0f * math::pi4), -1.0f, epsilon);
	EXPECT_NEAR(math::Tan(-3.0f * math::pi4), 1.0f, epsilon);
}
TEST(Mathf, Asin)
{
	EXPECT_NEAR(math::Asin(0.0f), 0.0f, epsilon);
	EXPECT_NEAR(math::Asin(1.0f), math::pi2, epsilon);
	EXPECT_NEAR(math::Asin(-1.0f), -math::pi2, epsilon);
}
TEST(Mathf, Acos)
{
	EXPECT_NEAR(math::Acos(1.0f), 0.0f, epsilon);
	EXPECT_NEAR(math::Acos(0.0f), math::pi2, epsilon);
	EXPECT_NEAR(math::Acos(-1.0f), math::pi, epsilon);
}
TEST(Mathf, Atan)
{
	EXPECT_NEAR(math::Atan(0.0f), 0.0f, epsilon);
	EXPECT_NEAR(math::Atan(1.0f), math::pi4, epsilon);
	EXPECT_NEAR(math::Atan(-1.0f), -math::pi4, epsilon);
}
TEST(Mathf, Atan2)
{
	EXPECT_NEAR(math::Atan2(0.0f, 0.0f), 0.0f, epsilon);
	EXPECT_NEAR(math::Atan2(0.0f, 1.0f), 0.0f, epsilon);
	EXPECT_NEAR(math::Atan2(1.0f, 1.0f), math::pi4, epsilon);
	EXPECT_NEAR(math::Atan2(1.0f, 0.0f), math::pi2, epsilon);
	EXPECT_NEAR(math::Atan2(1.0f, -1.0f), 3.0f * math::pi4, epsilon);
	EXPECT_NEAR(math::Atan2(0.0f, -1.0f), math::pi, epsilon);
	EXPECT_NEAR(math::Atan2(-1.0f, -1.0f), -3.0f * math::pi4, epsilon);
	EXPECT_NEAR(math::Atan2(-1.0f, 0.0f), -math::pi2, epsilon);
	EXPECT_NEAR(math::Atan2(-1.0f, 1.0f), -math::pi4, epsilon);
}