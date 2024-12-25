#ifndef __INCLUDE_GUARD_testBounds_h__
#define __INCLUDE_GUARD_testBounds_h__



TEST(Bounds, GetMin)
{
	Bounds bounds(Float3(1.0f, 2.0f, 3.0f), Float3(2.0f, 3.0f, 4.0f));
	Float3 min = bounds.GetMin();

    EXPECT_FLOAT_EQ(min.x, -1.0f);
	EXPECT_FLOAT_EQ(min.y, -1.0f);
	EXPECT_FLOAT_EQ(min.z, -1.0f);
}
TEST(Bounds, GetMax)
{
	Bounds bounds(Float3(1.0f, 2.0f, 3.0f), Float3(2.0f, 3.0f, 4.0f));
	Float3 max = bounds.GetMax();

	EXPECT_FLOAT_EQ(max.x, 3.0f);
	EXPECT_FLOAT_EQ(max.y, 5.0f);
	EXPECT_FLOAT_EQ(max.z, 7.0f);
}
TEST(Bounds, GetSize)
{
	Bounds bounds(Float3(1.0f, 2.0f, 3.0f), Float3(2.0f, 3.0f, 4.0f));
	Float3 size = bounds.GetSize();

	EXPECT_FLOAT_EQ(size.x, 4.0f);
	EXPECT_FLOAT_EQ(size.y, 6.0f);
	EXPECT_FLOAT_EQ(size.z, 8.0f);
}
TEST(Bounds, ClosestPoint)
{
	Bounds bounds(Float3(1.0f, 2.0f, 3.0f), Float3(2.0f, 3.0f, 4.0f));
	Float3 point = Float3(1.0f, 2.0f, 8.0f);
	Float3 closestPoint = bounds.ClosestPoint(point);

	EXPECT_FLOAT_EQ(closestPoint.x, 1.0f);
	EXPECT_FLOAT_EQ(closestPoint.y, 2.0f);
	EXPECT_FLOAT_EQ(closestPoint.z, 7.0f);
}
TEST(Bounds, Contains)
{// fails at epsilon = 1e-7
	Bounds bounds(Float3(1.0f, 2.0f, 3.0f), Float3(2.0f, 3.0f, 4.0f));
	float epsilon = 1e-6;
	Float3 pointInside = Float3(1.0f, 2.0f, 7.0f - epsilon);
	Float3 pointOutside = Float3(1.0f, 2.0f, 7.0f + epsilon);

	EXPECT_FLOAT_EQ(bounds.Contains(pointInside), true);
	EXPECT_FLOAT_EQ(bounds.Contains(pointOutside), false);	
}
TEST(Bounds, Encapsulate)
{
	Bounds bounds(Float3(0.0f, 0.0f, 0.0f), Float3(1.0f, 1.0f, 1.0f));
	Float3 point = Float3(2.0f, 2.0f, 2.0f);
	bounds.Encapsulate(point);
	bool contains = bounds.Contains(point);
	Float3 size = bounds.GetSize();
	Float3 center = bounds.center;

	EXPECT_FLOAT_EQ(contains, true);
	EXPECT_FLOAT_EQ(size.x, 3.0f);
	EXPECT_FLOAT_EQ(center.x, 0.5f);
}
TEST(Bounds, Expand)
{
	Bounds bounds(Float3(0.0f, 0.0f, 0.0f), Float3(1.0f, 1.0f, 1.0f));
	Float3 sizeOld = bounds.GetSize();
	float amount = 3.1415f;
	bounds.Expand(amount);
	Float3 size = bounds.GetSize();

	EXPECT_FLOAT_EQ(size.x, 2.0f * amount + sizeOld.x);
}
TEST(Bounds, Expand3)
{
	Bounds bounds(Float3(1.0f, 2.0f, 3.0f), Float3(1.0f, 2.0f, 3.0f));
	Float3 sizeOld = bounds.GetSize();
	Float3 amount = Float3(3.1415f, 2.7182f, 1.6180f);
	bounds.Expand(amount);
	Float3 size = bounds.GetSize();

	EXPECT_FLOAT_EQ(size.x, 2.0f * amount.x + sizeOld.x);
	EXPECT_FLOAT_EQ(size.y, 2.0f * amount.y + sizeOld.y);
	EXPECT_FLOAT_EQ(size.z, 2.0f * amount.z + sizeOld.z);
}
TEST(Bounds, SetMinMax)
{
	Bounds bounds(Float3(0.0f, 0.0f, 0.0f), Float3(1.0f, 1.0f, 1.0f));
	Float3 min = Float3(0.0f, 0.0f, 0.0f);
	Float3 max = Float3(1.0f, 2.0f, 4.0f);
	bounds.SetMinMax(min, max);
	Float3 size = bounds.GetSize();
	Float3 center = bounds.center;

	EXPECT_FLOAT_EQ(size.x, 1.0f);
	EXPECT_FLOAT_EQ(size.y, 2.0f);
	EXPECT_FLOAT_EQ(size.z, 4.0f);
	EXPECT_FLOAT_EQ(center.x, 0.5f);
	EXPECT_FLOAT_EQ(center.y, 1.0f);
	EXPECT_FLOAT_EQ(center.z, 2.0f);
}



#endif // __INCLUDE_GUARD_testBounds_h__