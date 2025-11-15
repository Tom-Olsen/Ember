#include <gtest/gtest.h>

// Floating point precision:
constexpr float epsilon = 1e-6f;

// Custom test macros:
// Float2/3/4 equality:
#define EXPECT_FLOAT2_EQ(a, b) \
	EXPECT_FLOAT_EQ(a.x, b.x); \
	EXPECT_FLOAT_EQ(a.y, b.y);
#define EXPECT_FLOAT3_EQ(a, b) \
	EXPECT_FLOAT_EQ(a.x, b.x); \
	EXPECT_FLOAT_EQ(a.y, b.y); \
	EXPECT_FLOAT_EQ(a.z, b.z);
#define EXPECT_FLOAT4_EQ(a, b) \
	EXPECT_FLOAT_EQ(a.x, b.x); \
	EXPECT_FLOAT_EQ(a.y, b.y); \
	EXPECT_FLOAT_EQ(a.z, b.z); \
	EXPECT_FLOAT_EQ(a.w, b.w);

// Float2/3/4 near equality:
#define EXPECT_NEAR2(a, b, e) \
	EXPECT_NEAR(a.x, b.x, e); \
	EXPECT_NEAR(a.y, b.y, e);
#define EXPECT_NEAR3(a, b, e) \
	EXPECT_NEAR(a.x, b.x, e); \
	EXPECT_NEAR(a.y, b.y, e); \
	EXPECT_NEAR(a.z, b.z, e);
#define EXPECT_NEAR4(a, b, e) \
	EXPECT_NEAR(a.x, b.x, e); \
	EXPECT_NEAR(a.y, b.y, e); \
	EXPECT_NEAR(a.z, b.z, e); \
	EXPECT_NEAR(a.w, b.w, e);

#include "sphFluid2dGpuTest.h"



int main(int argc, char** argv)
{
	// VS debugging (windows only=MSVC compiler):
	#ifdef _MSC_VER
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	#endif

	::testing::InitGoogleTest(&argc, argv);
	::testing::FLAGS_gtest_catch_exceptions = false;
	::testing::FLAGS_gtest_output = "console";
	return RUN_ALL_TESTS();
}