#include "emberMath.h"
#include <gtest/gtest.h>



TEST(Cone, ClosestPoint)
{
	// Cone with apex at the origin, base circle of radius 1 centered at (0, 0, 2).
	Cone cone(Float3(0.0f, 0.0f, 0.0f), Float3(0.0f, 0.0f, 2.0f), 1.0f);
	EXPECT_TRUE(cone.ClosestPoint(Float3(2.0f, 0.0f, 2.0f)).IsEpsilonEqual(Float3(1.0f, 0.0f, 2.0f)));	// Beyond the base rim.
	EXPECT_TRUE(cone.ClosestPoint(Float3(0.0f, 0.0f, -1.0f)).IsEpsilonEqual(Float3(0.0f, 0.0f, 0.0f)));	// Below the apex.
	EXPECT_TRUE(cone.ClosestPoint(Float3(0.4f, 0.0f, 1.0f)).IsEpsilonEqual(Float3(0.4f, 0.0f, 1.0f)));	// Inside the cone.
}
TEST(Cone, Contains)
{
	Cone cone(Float3(0.0f, 0.0f, 0.0f), Float3(0.0f, 0.0f, 2.0f), 1.0f);
	EXPECT_TRUE(cone.Contains(Float3(0.4f, 0.0f, 1.0f)));	// Radius at z=1 is 0.5.
	EXPECT_FALSE(cone.Contains(Float3(0.6f, 0.0f, 1.0f)));	// Outside the tapered radius.
	EXPECT_FALSE(cone.Contains(Float3(0.0f, 0.0f, 3.0f)));	// Past the base.
}
TEST(Cone, IntersectRaySide)
{
	Cone cone(Float3(0.0f, 0.0f, 0.0f), Float3(0.0f, 0.0f, 2.0f), 1.0f);
	Ray ray(Float3(2.0f, 0.0f, 1.0f), Float3::left);	// Surface radius at z=1 is 0.5.
	std::optional<Float3> hit = cone.IntersectRay(ray);
	ASSERT_TRUE(hit.has_value());
	EXPECT_TRUE(hit.value().IsEpsilonEqual(Float3(0.5f, 0.0f, 1.0f)));
}
TEST(Cone, IntersectRayBase)
{
	Cone cone(Float3(0.0f, 0.0f, 0.0f), Float3(0.0f, 0.0f, 2.0f), 1.0f);
	Ray ray(Float3(0.0f, 0.0f, 4.0f), Float3::down);
	std::optional<Float3> hit = cone.IntersectRay(ray);
	ASSERT_TRUE(hit.has_value());
	EXPECT_TRUE(hit.value().IsEpsilonEqual(Float3(0.0f, 0.0f, 2.0f)));
}
TEST(Cone, IntersectRayApex)
{
	Cone cone(Float3(0.0f, 0.0f, 0.0f), Float3(0.0f, 0.0f, 2.0f), 1.0f);
	Ray ray(Float3(0.0f, 0.0f, -2.0f), Float3::up);
	std::optional<Float3> hit = cone.IntersectRay(ray);
	ASSERT_TRUE(hit.has_value());
	EXPECT_TRUE(hit.value().IsEpsilonEqual(Float3(0.0f, 0.0f, 0.0f)));
}
TEST(Cone, IntersectRayMiss)
{
	Cone cone(Float3(0.0f, 0.0f, 0.0f), Float3(0.0f, 0.0f, 2.0f), 1.0f);
	Ray ray(Float3(2.0f, 0.0f, 1.0f), Float3::up);	// Runs parallel to the axis, outside the surface.
	EXPECT_FALSE(cone.IntersectRay(ray).has_value());
}
