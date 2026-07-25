#include "emberMath.h"
#include <gtest/gtest.h>



TEST(Triangle, GetCenter)
{
	Triangle triangle(Float3::zero, Float3::right, Float3::forward);
	EXPECT_TRUE(triangle.GetCenter().IsEpsilonEqual(Float3(1.0f / 3.0f, 1.0f / 3.0f, 0.0f)));
}
TEST(Triangle, GetNormal)
{
	Triangle triangle(Float3::zero, Float3::right, Float3::forward);
	EXPECT_TRUE(triangle.GetNormal().IsEpsilonEqual(Float3::up));
}
TEST(Triangle, Contains)
{
	Triangle triangle(Float3::zero, Float3::right, Float3::forward);
	EXPECT_TRUE(triangle.Contains(Float3(0.25f, 0.25f, 0.0f)));
	EXPECT_FALSE(triangle.Contains(Float3(0.75f, 0.75f, 0.0f)));
	EXPECT_FALSE(triangle.Contains(Float3(0.25f, 0.25f, 0.1f)));
}
TEST(Triangle, ClosestPoint)
{
	Triangle triangle(Float3::zero, Float3::right, Float3::forward);
	EXPECT_TRUE(triangle.ClosestPoint(Float3(0.25f, 0.25f, 1.0f)).IsEpsilonEqual(Float3(0.25f, 0.25f, 0.0f)));
	EXPECT_TRUE(triangle.ClosestPoint(Float3(1.0f, 1.0f, 0.0f)).IsEpsilonEqual(Float3(0.5f, 0.5f, 0.0f)));
}
TEST(Triangle, ClosestPointDegenerate)
{
	Triangle triangle(Float3::zero, Float3::right, 2.0f * Float3::right);
	EXPECT_TRUE(triangle.ClosestPoint(Float3(1.5f, 1.0f, 0.0f)).IsEpsilonEqual(Float3(1.5f, 0.0f, 0.0f)));
}
TEST(Triangle, IntersectRay)
{
	Triangle triangle(Float3::zero, Float3::right, Float3::forward);
	Ray ray(Float3(0.25f, 0.25f, 2.0f), Float3::down);

	RayTriangleHit hit = triangle.IntersectRay(ray);
	ASSERT_TRUE(hit.GetHit());
	EXPECT_TRUE(math::IsEpsilonEqual(hit.GetDistance(), 2.0f));
	EXPECT_TRUE(hit.GetPoint().IsEpsilonEqual(Float3(0.25f, 0.25f, 0.0f)));
	EXPECT_TRUE(hit.GetNormal().IsEpsilonEqual(Float3::up));
	EXPECT_TRUE(hit.GetBarycentricCoordinates().IsEpsilonEqual(Float3(0.5f, 0.25f, 0.25f)));
}
TEST(Triangle, IntersectRayBackFace)
{
	Triangle triangle(Float3::zero, Float3::right, Float3::forward);
	Ray ray(Float3(0.25f, 0.25f, -2.0f), Float3::up);
	RayTriangleHit hit = triangle.IntersectRay(ray);
	ASSERT_TRUE(hit.GetHit());
	EXPECT_TRUE(hit.GetNormal().IsEpsilonEqual(Float3::up));
}
TEST(Triangle, IntersectRayMiss)
{
	Triangle triangle(Float3::zero, Float3::right, Float3::forward);
	Ray ray(Float3(0.75f, 0.75f, 2.0f), Float3::down);
	EXPECT_FALSE(triangle.IntersectRay(ray).GetHit());
}
TEST(Triangle, IntersectRayBehindOrigin)
{
	Triangle triangle(Float3::zero, Float3::right, Float3::forward);
	Ray ray(Float3(0.25f, 0.25f, 2.0f), Float3::up);
	EXPECT_FALSE(triangle.IntersectRay(ray).GetHit());
}
TEST(Triangle, IntersectRayDegenerate)
{
	Triangle triangle(Float3::zero, Float3::right, 2.0f * Float3::right);
	Ray ray(Float3(0.5f, 0.0f, 2.0f), Float3::down);
	EXPECT_FALSE(triangle.IntersectRay(ray).GetHit());
}