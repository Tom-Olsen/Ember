#ifndef __INCLUDE_GUARD_testBounds2d_h__
#define __INCLUDE_GUARD_testBounds2d_h__



TEST(Bounds2d, GetMin)
{
	Bounds2d bounds(Float2(1.0f, 2.0f), Float2(2.0f, 3.0f));
	Float2 min = bounds.GetMin();
	EXPECT_FLOAT2_EQ(min, Float2(-1.0f, -1.0f));
}
TEST(Bounds2d, GetMax)
{
	Bounds2d bounds(Float2(1.0f, 2.0f), Float2(2.0f, 3.0f));
	Float2 max = bounds.GetMax();
	EXPECT_FLOAT2_EQ(max, Float2(3.0f, 5.0f));
}
TEST(Bounds2d, GetSize)
{
	Bounds2d bounds(Float2(1.0f, 2.0f), Float2(2.0f, 3.0f));
	Float2 size = bounds.GetSize();
	EXPECT_FLOAT2_EQ(size, Float2(4.0f, 6.0f));
}
TEST(Bounds2d, GetDiagonal)
{
	Bounds2d bounds(Float2(1.0f, 2.0f), Float2(2.0f, 3.0f));
	float diagonal = (bounds.GetMax() - bounds.GetMin()).Length();
	EXPECT_NEAR(bounds.GetDiagonal(), diagonal, epsilon);
}
TEST(Bounds2d, ClosestPoint)
{
	Bounds2d bounds(Float2(1.0f, 2.0f), Float2(2.0f, 3.0f));
	Float2 point = Float2(1.0f, 2.0f);
	Float2 closestPoint = bounds.ClosestPoint(point);
	EXPECT_FLOAT2_EQ(closestPoint, Float2(1.0f, 2.0f));
}
TEST(Bounds2d, Contains)
{
	Bounds2d bounds(Float2(1.0f, 2.0f), Float2(2.0f, 3.0f));
	Float2 pointInside = Float2(1.0f, 5.0f - epsilon);
	Float2 pointOutside = Float2(1.0f, 5.0f + epsilon);
	EXPECT_TRUE(bounds.Contains(pointInside));
	EXPECT_FALSE(bounds.Contains(pointOutside));
}
TEST(Bounds2d, Encapsulate)
{
	Bounds2d bounds(Float2(0.0f, 0.0f), Float2(1.0f, 1.0f));
	Float2 point = Float2(2.0f, 2.0f);
	bounds.Encapsulate(point);
	bool contains = bounds.Contains(point);
	Float2 size = bounds.GetSize();
	Float2 center = bounds.center;
	EXPECT_TRUE(contains);
	EXPECT_FLOAT2_EQ(size, Float2(3.0f, 3.0f));
	EXPECT_FLOAT2_EQ(center, Float2(0.5f, 0.5f));
}
TEST(Bounds2d, Expand_Float)
{
	Bounds2d bounds(Float2(0.0f, 0.0f), Float2(1.0f, 1.0f));
	Float2 sizeOld = bounds.GetSize();
	float amount = 3.1415f;
	bounds.Expand(amount);
	Float2 size = bounds.GetSize();
	Float2 result = 2.0f * amount * Float2::one + sizeOld;
	EXPECT_FLOAT2_EQ(size, result);
}
TEST(Bounds2d, Expand_Float3)
{
	Bounds2d bounds(Float2(1.0f, 2.0f), Float2(1.0f, 2.0f));
	Float2 sizeOld = bounds.GetSize();
	Float2 amount = Float2(3.1415f, 2.7182f);
	bounds.Expand(amount);
	Float2 size = bounds.GetSize();
	Float2 result = 2.0f * amount + sizeOld;
	EXPECT_FLOAT2_EQ(size, result);
}
TEST(Bounds2d, SetMinMax)
{
	Bounds2d bounds(Float2(0.0f, 0.0f), Float2(1.0f, 1.0f));
	Float2 min = Float2(0.0f, 0.0f);
	Float2 max = Float2(1.0f, 2.0f);
	bounds.SetMinMax(min, max);
	Float2 size = bounds.GetSize();
	Float2 center = bounds.center;
	EXPECT_FLOAT2_EQ(size, Float2(1.0f, 2.0f));
	EXPECT_FLOAT2_EQ(center, Float2(0.5f, 1.0f));
}



#endif // __INCLUDE_GUARD_testBounds2d_h__