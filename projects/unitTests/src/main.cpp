#include <gtest/gtest.h>
#include "logger.h"
#include <iostream>

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

// math testing:
#include "emberMath.h"
#include "testBounds.h"
#include "testBounds2d.h"
#include "testEmberRandom.h"
#include "testFloat2.h"
#include "testFloat2x2.h"
#include "testFloat2x3.h"
#include "testFloat3.h"
#include "testFloat3x2.h"
#include "testFloat3x3.h"
#include "testFloat4.h"
#include "testFloat4x4.h"
#include "testGeometry3d.h"
#include "testInt2.h"
#include "testInt3.h"
#include "testMathf.h"
#include "testRay.h"
#include "testUint3.h"
#include "testGpuSort.h"



int main(int argc, char** argv)
{
	emberLogger::Logger::Init();
	math::Random::Init();
	::testing::InitGoogleTest(&argc, argv);
	::testing::FLAGS_gtest_catch_exceptions = false;
	::testing::FLAGS_gtest_output = "console";
	return RUN_ALL_TESTS();
}