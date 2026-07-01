#include "emberMath.h"
#include <gtest/gtest.h>



TEST(Quad, ClosestPoint)
{
	// Unit quad in the XY plane, corner at origin.
	Quad quad(Float3(0.0f, 0.0f, 0.0f), Float3(1.0f, 0.0f, 0.0f), Float3(0.0f, 1.0f, 0.0f));
	EXPECT_TRUE(quad.ClosestPoint(Float3(0.5f, 0.5f, 3.0f)).IsEpsilonEqual(Float3(0.5f, 0.5f, 0.0f)));
	EXPECT_TRUE(quad.ClosestPoint(Float3(2.0f, 0.5f, 0.0f)).IsEpsilonEqual(Float3(1.0f, 0.5f, 0.0f)));
	EXPECT_TRUE(quad.ClosestPoint(Float3(-1.0f, -1.0f, 0.0f)).IsEpsilonEqual(Float3(0.0f, 0.0f, 0.0f)));
}
TEST(Quad, Contains)
{
	Quad quad(Float3(0.0f, 0.0f, 0.0f), Float3(1.0f, 0.0f, 0.0f), Float3(0.0f, 1.0f, 0.0f));
	EXPECT_TRUE(quad.Contains(Float3(0.5f, 0.5f, 0.0f)));
	EXPECT_FALSE(quad.Contains(Float3(0.5f, 0.5f, 0.1f)));	// Off the plane.
	EXPECT_FALSE(quad.Contains(Float3(1.5f, 0.5f, 0.0f)));	// Outside u range.
}
TEST(Quad, IntersectRayInside)
{
	Quad quad(Float3(0.0f, 0.0f, 0.0f), Float3(1.0f, 0.0f, 0.0f), Float3(0.0f, 1.0f, 0.0f));
	Ray ray(Float3(0.5f, 0.5f, 2.0f), Float3::down);
	std::optional<Float3> hit = quad.IntersectRay(ray);
	ASSERT_TRUE(hit.has_value());
	EXPECT_TRUE(hit.value().IsEpsilonEqual(Float3(0.5f, 0.5f, 0.0f)));
}
TEST(Quad, IntersectRayMiss)
{
	Quad quad(Float3(0.0f, 0.0f, 0.0f), Float3(1.0f, 0.0f, 0.0f), Float3(0.0f, 1.0f, 0.0f));
	Ray ray(Float3(1.5f, 0.5f, 2.0f), Float3::down);	// Passes outside the u range.
	EXPECT_FALSE(quad.IntersectRay(ray).has_value());
}
TEST(Quad, IntersectRayBehind)
{
	Quad quad(Float3(0.0f, 0.0f, 0.0f), Float3(1.0f, 0.0f, 0.0f), Float3(0.0f, 1.0f, 0.0f));
	Ray ray(Float3(0.5f, 0.5f, 2.0f), Float3::up);	// Points away from the quad.
	EXPECT_FALSE(quad.IntersectRay(ray).has_value());
}
TEST(Quad, IntersectRayOriented)
{
	// Quad in the YZ plane (normal +X), corner at origin, drawn like a translate-handle plane.
	Quad quad(Float3(0.0f, 0.0f, 0.0f), Float3(0.0f, 1.0f, 0.0f), Float3(0.0f, 0.0f, 1.0f));
	Ray ray(Float3(3.0f, 0.5f, 0.5f), Float3::left);
	std::optional<Float3> hit = quad.IntersectRay(ray);
	ASSERT_TRUE(hit.has_value());
	EXPECT_TRUE(hit.value().IsEpsilonEqual(Float3(0.0f, 0.5f, 0.5f)));
}
