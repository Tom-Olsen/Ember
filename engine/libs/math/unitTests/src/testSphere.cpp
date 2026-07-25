#include "emberMath.h"
#include <gtest/gtest.h>



TEST(Sphere, ClosestPoint)
{
	Sphere sphere(Float3(0.0f, 0.0f, 1.0f), 1.0f);
	EXPECT_TRUE(sphere.ClosestPoint(Float3(2.0f, 0.0f, 1.0f)).IsEpsilonEqual(Float3(1.0f, 0.0f, 1.0f)));
	EXPECT_TRUE(sphere.ClosestPoint(Float3(0.5f, 0.0f, 1.0f)).IsEpsilonEqual(Float3(0.5f, 0.0f, 1.0f)));
}
TEST(Sphere, Contains)
{
	Sphere sphere(Float3(0.0f, 0.0f, 1.0f), 1.0f);
	EXPECT_TRUE(sphere.Contains(Float3(0.5f, 0.0f, 1.0f)));
	EXPECT_TRUE(sphere.Contains(Float3(0.0f, 0.0f, 2.0f)));
	EXPECT_FALSE(sphere.Contains(Float3(1.5f, 0.0f, 1.0f)));
}
TEST(Sphere, IntersectRay)
{
	Sphere sphere(Float3(0.0f, 0.0f, 1.0f), 1.0f);
	Ray ray(Float3(2.0f, 0.0f, 1.0f), Float3::left);
	RayHit hit = sphere.IntersectRay(ray);
	ASSERT_TRUE(hit.GetHit());
	EXPECT_TRUE(math::IsEpsilonEqual(hit.GetDistance(), 1.0f));
	EXPECT_TRUE(hit.GetPoint().IsEpsilonEqual(Float3(1.0f, 0.0f, 1.0f)));
	EXPECT_TRUE(hit.GetNormal().IsEpsilonEqual(Float3::right));
}
TEST(Sphere, IntersectRayInside)
{
	Sphere sphere(Float3(0.0f, 0.0f, 1.0f), 1.0f);
	Ray ray(Float3(0.0f, 0.0f, 1.0f), Float3::right);
	RayHit hit = sphere.IntersectRay(ray);
	ASSERT_TRUE(hit.GetHit());
	EXPECT_TRUE(hit.GetPoint().IsEpsilonEqual(Float3(1.0f, 0.0f, 1.0f)));
}
TEST(Sphere, IntersectRayTangent)
{
	Sphere sphere(Float3(0.0f, 0.0f, 1.0f), 1.0f);
	Ray ray(Float3(0.0f, 1.0f, 3.0f), Float3::down);
	RayHit hit = sphere.IntersectRay(ray);
	ASSERT_TRUE(hit.GetHit());
	EXPECT_TRUE(hit.GetPoint().IsEpsilonEqual(Float3(0.0f, 1.0f, 1.0f)));
}
TEST(Sphere, IntersectRayMiss)
{
	Sphere sphere(Float3(0.0f, 0.0f, 1.0f), 1.0f);
	Ray ray(Float3(2.0f, 0.0f, 1.0f), Float3::up);
	EXPECT_FALSE(sphere.IntersectRay(ray).GetHit());
}