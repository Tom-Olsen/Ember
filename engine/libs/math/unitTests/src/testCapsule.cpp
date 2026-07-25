#include "emberMath.h"
#include <gtest/gtest.h>



TEST(Capsule, ClosestPoint)
{
	Capsule capsule(Float3(0.0f, 0.0f, 0.0f), Float3(0.0f, 0.0f, 2.0f), 1.0f);
	EXPECT_TRUE(capsule.ClosestPoint(Float3(2.0f, 0.0f, 1.0f)).IsEpsilonEqual(Float3(1.0f, 0.0f, 1.0f)));
	EXPECT_TRUE(capsule.ClosestPoint(Float3(0.0f, 0.0f, 4.0f)).IsEpsilonEqual(Float3(0.0f, 0.0f, 3.0f)));
	EXPECT_TRUE(capsule.ClosestPoint(Float3(0.5f, 0.0f, 1.0f)).IsEpsilonEqual(Float3(0.5f, 0.0f, 1.0f)));
}
TEST(Capsule, Contains)
{
	Capsule capsule(Float3(0.0f, 0.0f, 0.0f), Float3(0.0f, 0.0f, 2.0f), 1.0f);
	EXPECT_TRUE(capsule.Contains(Float3(0.5f, 0.0f, 1.0f)));
	EXPECT_TRUE(capsule.Contains(Float3(0.0f, 0.0f, 2.5f)));
	EXPECT_FALSE(capsule.Contains(Float3(1.5f, 0.0f, 1.0f)));
}
TEST(Capsule, IntersectRaySide)
{
	Capsule capsule(Float3(0.0f, 0.0f, 0.0f), Float3(0.0f, 0.0f, 2.0f), 1.0f);
	Ray ray(Float3(2.0f, 0.0f, 1.0f), Float3::left);
	RayHit hit = capsule.IntersectRay(ray);
	ASSERT_TRUE(hit.GetHit());
	EXPECT_TRUE(math::IsEpsilonEqual(hit.GetDistance(), 1.0f));
	EXPECT_TRUE(hit.GetPoint().IsEpsilonEqual(Float3(1.0f, 0.0f, 1.0f)));
	EXPECT_TRUE(hit.GetNormal().IsEpsilonEqual(Float3::right));
}
TEST(Capsule, IntersectRayCap)
{
	Capsule capsule(Float3(0.0f, 0.0f, 0.0f), Float3(0.0f, 0.0f, 2.0f), 1.0f);
	Ray ray(Float3(0.0f, 0.0f, 4.0f), Float3::down);
	RayHit hit = capsule.IntersectRay(ray);
	ASSERT_TRUE(hit.GetHit());
	EXPECT_TRUE(hit.GetPoint().IsEpsilonEqual(Float3(0.0f, 0.0f, 3.0f)));
}
TEST(Capsule, IntersectRayInside)
{
	Capsule capsule(Float3(0.0f, 0.0f, 0.0f), Float3(0.0f, 0.0f, 2.0f), 1.0f);
	Ray ray(Float3(0.0f, 0.0f, 1.0f), Float3::right);
	RayHit hit = capsule.IntersectRay(ray);
	ASSERT_TRUE(hit.GetHit());
	EXPECT_TRUE(hit.GetPoint().IsEpsilonEqual(Float3(1.0f, 0.0f, 1.0f)));
}
TEST(Capsule, IntersectRayMiss)
{
	Capsule capsule(Float3(0.0f, 0.0f, 0.0f), Float3(0.0f, 0.0f, 2.0f), 1.0f);
	Ray ray(Float3(2.0f, 0.0f, 1.0f), Float3::up);
	EXPECT_FALSE(capsule.IntersectRay(ray).GetHit());
}