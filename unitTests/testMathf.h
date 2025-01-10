#ifndef __INCLUDE_GUARD_testMathf_h__
#define __INCLUDE_GUARD_testMathf_h__



// Basic math:
TEST(Mathf, Abs)
{
	EXPECT_FLOAT_EQ(mathf::Abs( 0.0f), 0.0f);
	EXPECT_FLOAT_EQ(mathf::Abs(-0.0f), 0.0f);
	EXPECT_FLOAT_EQ(mathf::Abs( 1.0f), 1.0f);
	EXPECT_FLOAT_EQ(mathf::Abs(-1.0f), 1.0f);
}
TEST(Mathf, Clamp)
{
	float min = 0.5f;
	float max = 1.5f;
	EXPECT_NEAR(mathf::Clamp(0.0f, min, max), 0.5f, epsilon);
	EXPECT_NEAR(mathf::Clamp(1.0f, min, max), 1.0f, epsilon);
	EXPECT_NEAR(mathf::Clamp(2.0f, min, max), 1.5f, epsilon);
}
TEST(Mathf, Max)
{
	EXPECT_NEAR(mathf::Max(0.0f, 1.0f), 1.0f, epsilon);
	EXPECT_NEAR(mathf::Max(1.0f, 0.0f), 1.0f, epsilon);
}
TEST(Mathf, Min)
{
	EXPECT_NEAR(mathf::Min(0.0f, 1.0f), 0.0f, epsilon);
	EXPECT_NEAR(mathf::Min(1.0f, 0.0f), 0.0f, epsilon);
}
TEST(Mathf, Sign)
{
	EXPECT_FLOAT_EQ(mathf::Sign(0.0f), 0.0f);
	EXPECT_FLOAT_EQ(mathf::Sign(-0.0f), 0.0f);
	EXPECT_FLOAT_EQ(mathf::Sign(4.0f), 1.0f);
	EXPECT_FLOAT_EQ(mathf::Sign(-4.0f), -1.0f);
}
TEST(Mathf, Sqrt)
{
	EXPECT_NEAR(mathf::Sqrt(0.0f), 0.0f, epsilon);
	EXPECT_NEAR(mathf::Sqrt(1.0f), 1.0f, epsilon);
	EXPECT_NEAR(mathf::Sqrt(4.0f), 2.0f, epsilon);
	EXPECT_NEAR(mathf::Sqrt(9.0f), 3.0f, epsilon);
	EXPECT_TRUE(std::isnan(mathf::Sqrt(-9.0f)));
}
TEST(Mathf, Factorial)
{
	int factorial = 1;
	for (int i = 0; i < 10; i++)
	{
		if (i > 0)
			factorial *= i;
		EXPECT_EQ(mathf::Factorial(i), factorial);
	}
}

// Trigonometry:
TEST(Mathf, Sin)
{
	EXPECT_NEAR(mathf::Sin(0.0f), 0.0f, epsilon);
	EXPECT_NEAR(mathf::Sin(mathf::pi2), 1.0f, epsilon);
	EXPECT_NEAR(mathf::Sin(mathf::pi), 0.0f, epsilon);
	EXPECT_NEAR(mathf::Sin(3.0f * mathf::pi2), -1.0f, epsilon);
	EXPECT_NEAR(mathf::Sin(2.0f * mathf::pi), 0.0f, epsilon);
}
TEST(Mathf, Cos)
{
	EXPECT_NEAR(mathf::Cos(0.0f), 1.0f, epsilon);
	EXPECT_NEAR(mathf::Cos(mathf::pi2), 0.0f, epsilon);
	EXPECT_NEAR(mathf::Cos(mathf::pi), -1.0f, epsilon);
	EXPECT_NEAR(mathf::Cos(3.0f * mathf::pi2), 0.0f, epsilon);
	EXPECT_NEAR(mathf::Cos(2.0f * mathf::pi), 1.0f, epsilon);
}
TEST(Mathf, Tan)
{
	EXPECT_NEAR(mathf::Tan(0.0f), 0.0f, epsilon);
	EXPECT_NEAR(mathf::Tan(mathf::pi4), 1.0f, epsilon);
	EXPECT_NEAR(mathf::Tan(-mathf::pi4), -1.0f, epsilon);
	EXPECT_NEAR(mathf::Tan(3.0f * mathf::pi4), -1.0f, epsilon);
	EXPECT_NEAR(mathf::Tan(-3.0f * mathf::pi4), 1.0f, epsilon);
}
TEST(Mathf, Asin)
{
	EXPECT_NEAR(mathf::Asin(0.0f), 0.0f, epsilon);
	EXPECT_NEAR(mathf::Asin(1.0f), mathf::pi2, epsilon);
	EXPECT_NEAR(mathf::Asin(-1.0f), -mathf::pi2, epsilon);
}
TEST(Mathf, Acos)
{
	EXPECT_NEAR(mathf::Acos(1.0f), 0.0f, epsilon);
	EXPECT_NEAR(mathf::Acos(0.0f), mathf::pi2, epsilon);
	EXPECT_NEAR(mathf::Acos(-1.0f), mathf::pi, epsilon);
}
TEST(Mathf, Atan)
{
	EXPECT_NEAR(mathf::Atan(0.0f), 0.0f, epsilon);
	EXPECT_NEAR(mathf::Atan(1.0f), mathf::pi4, epsilon);
	EXPECT_NEAR(mathf::Atan(-1.0f), -mathf::pi4, epsilon);
}
TEST(Mathf, Atan2)
{
	EXPECT_NEAR(mathf::Atan2(0.0f, 0.0f), 0.0f, epsilon);
	EXPECT_NEAR(mathf::Atan2(0.0f, 1.0f), 0.0f, epsilon);
	EXPECT_NEAR(mathf::Atan2(1.0f, 1.0f), mathf::pi4, epsilon);
	EXPECT_NEAR(mathf::Atan2(1.0f, 0.0f), mathf::pi2, epsilon);
	EXPECT_NEAR(mathf::Atan2(1.0f, -1.0f), 3.0f * mathf::pi4, epsilon);
	EXPECT_NEAR(mathf::Atan2(0.0f, -1.0f), mathf::pi, epsilon);
	EXPECT_NEAR(mathf::Atan2(-1.0f, -1.0f), -3.0f * mathf::pi4, epsilon);
	EXPECT_NEAR(mathf::Atan2(-1.0f, 0.0f), -mathf::pi2, epsilon);
	EXPECT_NEAR(mathf::Atan2(-1.0f, 1.0f), -mathf::pi4, epsilon);
}



#endif // __INCLUDE_GUARD_testMathf_h__