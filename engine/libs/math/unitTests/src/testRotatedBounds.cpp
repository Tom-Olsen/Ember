#include "emberMath.h"
#include <array>
#include <gtest/gtest.h>



TEST(RotatedBounds, Contains)
{
	Bounds localBounds(Float3(10.0f, 0.0f, 0.0f), Float3::one);
	RotatedBounds bounds(localBounds, Float3x3::RotateZ(math::pi2));
	Float3 inside = Float3(10.0f, 0.5f, 0.0f);
	Float3 outside = Float3(10.0f, 1.5f, 0.0f);
	EXPECT_TRUE(bounds.Contains(inside));
	EXPECT_FALSE(bounds.Contains(outside));
}
TEST(RotatedBounds, ClosestPoint)
{
	Bounds localBounds(Float3(10.0f, 0.0f, 0.0f), Float3::one);
	RotatedBounds bounds(localBounds, Float3x3::RotateZ(math::pi2));
	Float3 point = Float3(10.0f, 2.0f, 0.0f);
	Float3 expected = Float3(10.0f, 1.0f, 0.0f);
	Float3 closestPoint = bounds.ClosestPoint(point);
	EXPECT_TRUE(closestPoint.IsEpsilonEqual(expected));
}
TEST(RotatedBounds, GetCorners)
{
	Bounds localBounds(Float3(10.0f, 0.0f, 0.0f), Float3::one);
	RotatedBounds bounds(localBounds, Float3x3::RotateZ(math::pi2));
	std::array<Float3, 8> corners = bounds.GetCorners();
	for (uint32_t i = 0; i < corners.size(); i++)
		EXPECT_TRUE(bounds.Contains(corners[i]));
	EXPECT_TRUE(corners[0].IsEpsilonEqual(Float3(11.0f, -1.0f, -1.0f)));
}
TEST(RotatedBounds, GetWorldBounds)
{
	Bounds localBounds(Float3(10.0f, 0.0f, 0.0f), Float3(1.0f, 2.0f, 3.0f));
	RotatedBounds bounds(localBounds, Float3x3::RotateZ(math::pi2));
	Bounds worldBounds = bounds.GetWorldBounds();
	std::array<Float3, 8> corners = bounds.GetCorners();
	for (const Float3& corner : corners)
		EXPECT_TRUE(worldBounds.Contains(corner));
	EXPECT_TRUE(worldBounds.center.IsEpsilonEqual(Float3(10.0f, 0.0f, 0.0f)));
	EXPECT_TRUE(worldBounds.extent.IsEpsilonEqual(Float3(2.0f, 1.0f, 3.0f)));
}
TEST(RotatedBounds, IntersectRay)
{
	Bounds localBounds(Float3(10.0f, 0.0f, 0.0f), Float3::one);
	RotatedBounds bounds(localBounds, Float3x3::RotateZ(math::pi2));
	Ray ray(Float3(10.0f, 2.0f, 0.0f), Float3(0.0f, -1.0f, 0.0f));
	std::optional<Float3> hit = bounds.IntersectRay(ray);
	ASSERT_TRUE(hit.has_value());
	EXPECT_TRUE(hit.value().IsEpsilonEqual(Float3(10.0f, 1.0f, 0.0f)));
}