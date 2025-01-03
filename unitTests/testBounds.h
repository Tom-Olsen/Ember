#ifndef __INCLUDE_GUARD_testBounds_h__
#define __INCLUDE_GUARD_testBounds_h__



TEST(Bounds, GetMin)
{
	Bounds bounds(Float3(1.0f, 2.0f, 3.0f), Float3(2.0f, 3.0f, 4.0f));
	Float3 min = bounds.GetMin();
	EXPECT_FLOAT3_EQ(min, Float3(-1.0f, -1.0f, -1.0f));
}
TEST(Bounds, GetMax)
{
	Bounds bounds(Float3(1.0f, 2.0f, 3.0f), Float3(2.0f, 3.0f, 4.0f));
	Float3 max = bounds.GetMax();
	EXPECT_FLOAT3_EQ(max, Float3(3.0f, 5.0f, 7.0f));
}
TEST(Bounds, GetSize)
{
	Bounds bounds(Float3(1.0f, 2.0f, 3.0f), Float3(2.0f, 3.0f, 4.0f));
	Float3 size = bounds.GetSize();
	EXPECT_FLOAT3_EQ(size, Float3(4.0f, 6.0f, 8.0f));
}
TEST(Bounds, ClosestPoint)
{
	Bounds bounds(Float3(1.0f, 2.0f, 3.0f), Float3(2.0f, 3.0f, 4.0f));
	Float3 point = Float3(1.0f, 2.0f, 8.0f);
	Float3 closestPoint = bounds.ClosestPoint(point);
	EXPECT_FLOAT3_EQ(closestPoint, Float3(1.0f, 2.0f, 7.0f));
}
TEST(Bounds, Contains)
{
	Bounds bounds(Float3(1.0f, 2.0f, 3.0f), Float3(2.0f, 3.0f, 4.0f));
	Float3 pointInside = Float3(1.0f, 2.0f, 7.0f - epsilon);
	Float3 pointOutside = Float3(1.0f, 2.0f, 7.0f + epsilon);
	EXPECT_TRUE(bounds.Contains(pointInside));
	EXPECT_FALSE(bounds.Contains(pointOutside));
}
TEST(Bounds, Encapsulate)
{
	Bounds bounds(Float3(0.0f, 0.0f, 0.0f), Float3(1.0f, 1.0f, 1.0f));
	Float3 point = Float3(2.0f, 2.0f, 2.0f);
	bounds.Encapsulate(point);
	bool contains = bounds.Contains(point);
	Float3 size = bounds.GetSize();
	Float3 center = bounds.center;
	EXPECT_TRUE(contains);
	EXPECT_FLOAT3_EQ(size, Float3(3.0f, 3.0f, 3.0f));
	EXPECT_FLOAT3_EQ(center, Float3(0.5f, 0.5f, 0.5f));
}
TEST(Bounds, Expand_Float)
{
	Bounds bounds(Float3(0.0f, 0.0f, 0.0f), Float3(1.0f, 1.0f, 1.0f));
	Float3 sizeOld = bounds.GetSize();
	float amount = 3.1415f;
	bounds.Expand(amount);
	Float3 size = bounds.GetSize();
	Float3 result = 2.0f * amount * Float3::one + sizeOld;
	EXPECT_FLOAT3_EQ(size, result);
}
TEST(Bounds, Expand_Float3)
{
	Bounds bounds(Float3(1.0f, 2.0f, 3.0f), Float3(1.0f, 2.0f, 3.0f));
	Float3 sizeOld = bounds.GetSize();
	Float3 amount = Float3(3.1415f, 2.7182f, 1.6180f);
	bounds.Expand(amount);
	Float3 size = bounds.GetSize();
	Float3 result = 2.0f * amount + sizeOld;
	EXPECT_FLOAT3_EQ(size, result);
}
TEST(Bounds, SetMinMax)
{
	Bounds bounds(Float3(0.0f, 0.0f, 0.0f), Float3(1.0f, 1.0f, 1.0f));
	Float3 min = Float3(0.0f, 0.0f, 0.0f);
	Float3 max = Float3(1.0f, 2.0f, 4.0f);
	bounds.SetMinMax(min, max);
	Float3 size = bounds.GetSize();
	Float3 center = bounds.center;
	EXPECT_FLOAT3_EQ(size, Float3(1.0f, 2.0f, 4.0f));
	EXPECT_FLOAT3_EQ(center, Float3(0.5f, 1.0f, 2.0f));
}



#endif // __INCLUDE_GUARD_testBounds_h__