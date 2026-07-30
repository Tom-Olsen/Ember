#include "emberMath.h"
#include <gtest/gtest.h>



TEST(Plane, IntersectRay)
{
	Ray ray(Float3(1.0f, 2.0f, 3.0f), Float3::forward);
	Plane plane(Float3(4.0f, 7.0f, 6.0f), Float3::forward);

	RayHit hit = plane.IntersectRay(ray);
	ASSERT_TRUE(hit.GetHit());
	EXPECT_TRUE(math::IsEpsilonEqual(hit.GetDistance(), 5.0f));
	EXPECT_TRUE(hit.GetPoint().IsEpsilonEqual(Float3(1.0f, 7.0f, 3.0f)));
	EXPECT_TRUE(hit.GetNormal().IsEpsilonEqual(Float3::forward));
}

TEST(Plane, IntersectRayBehindOrigin)
{
	Ray ray(Float3(1.0f, 2.0f, 3.0f), Float3::forward);
	Plane plane(Float3(4.0f, -7.0f, 6.0f), Float3::forward);

	EXPECT_FALSE(plane.IntersectRay(ray).GetHit());
}

TEST(Plane, IntersectRayParallel)
{
	Ray ray(Float3(1.0f, 2.0f, 3.0f), Float3::forward);
	Plane plane(Float3(4.0f, 7.0f, 6.0f), Float3::right);

	EXPECT_FALSE(plane.IntersectRay(ray).GetHit());
}

TEST(Plane, IntersectRayInvalidPlane)
{
	Ray ray(Float3(1.0f, 2.0f, 3.0f), Float3::forward);
	Plane plane(Float3(4.0f, 7.0f, 6.0f), Float3::zero);

	EXPECT_FALSE(plane.IntersectRay(ray).GetHit());
}