#include "emberMath.h"
#include <array>
#include <gtest/gtest.h>



TEST(TransformedBounds, Contains)
{
	Bounds localBounds(Float3::zero, Float3::one);
	Float4x4 localToWorld = Float4x4::Translate(Float3(10.0f, 0.0f, 0.0f)) * Float4x4::RotateZ(math::pi2);
	TransformedBounds bounds(localBounds, localToWorld);
	Float3 inside = Float3(localToWorld * Float4(0.5f, 0.0f, 0.0f, 1.0f));
	Float3 outside = Float3(localToWorld * Float4(1.5f, 0.0f, 0.0f, 1.0f));
	EXPECT_TRUE(bounds.Contains(inside));
	EXPECT_FALSE(bounds.Contains(outside));
}
TEST(TransformedBounds, ClosestPoint)
{
	Bounds localBounds(Float3::zero, Float3::one);
	Float4x4 localToWorld = Float4x4::Translate(Float3(10.0f, 0.0f, 0.0f)) * Float4x4::RotateZ(math::pi2);
	TransformedBounds bounds(localBounds, localToWorld);
	Float3 point = Float3(localToWorld * Float4(2.0f, 0.0f, 0.0f, 1.0f));
	Float3 expected = Float3(localToWorld * Float4(1.0f, 0.0f, 0.0f, 1.0f));
	Float3 closestPoint = bounds.ClosestPoint(point);
	EXPECT_TRUE(closestPoint.IsEpsilonEqual(expected));
}
TEST(TransformedBounds, Encapsulate)
{
	Bounds localBounds(Float3::zero, Float3::one);
	Float4x4 localToWorld = Float4x4::Translate(Float3(10.0f, 0.0f, 0.0f)) * Float4x4::RotateZ(math::pi2);
	TransformedBounds bounds(localBounds, localToWorld);
	Float3 point = Float3(localToWorld * Float4(3.0f, 0.0f, 0.0f, 1.0f));
	bounds.Encapsulate(point);
	EXPECT_TRUE(bounds.Contains(point));
	EXPECT_TRUE(bounds.localBounds.center.IsEpsilonEqual(Float3(1.0f, 0.0f, 0.0f)));
	EXPECT_TRUE(bounds.localBounds.extent.IsEpsilonEqual(Float3(2.0f, 1.0f, 1.0f)));
}
TEST(TransformedBounds, GetCorners)
{
	Bounds localBounds(Float3::zero, Float3::one);
	Float4x4 localToWorld = Float4x4::Translate(Float3(10.0f, 0.0f, 0.0f)) * Float4x4::RotateZ(math::pi2);
	TransformedBounds bounds(localBounds, localToWorld);
	std::array<Float3, 8> corners = bounds.GetCorners();
	for (uint32_t i = 0; i < corners.size(); i++)
		EXPECT_TRUE(bounds.Contains(corners[i]));
	EXPECT_TRUE(corners[0].IsEpsilonEqual(Float3(localToWorld * Float4(localBounds.GetCorners()[0], 1.0f))));
}
TEST(TransformedBounds, GetWorldBounds)
{
	Bounds localBounds(Float3::zero, Float3(1.0f, 2.0f, 3.0f));
	Float4x4 localToWorld = Float4x4::Translate(Float3(10.0f, 0.0f, 0.0f)) * Float4x4::RotateZ(math::pi2);
	TransformedBounds bounds(localBounds, localToWorld);
	Bounds worldBounds = bounds.GetWorldBounds();
	std::array<Float3, 8> corners = bounds.GetCorners();
	for (const Float3& corner : corners)
		EXPECT_TRUE(worldBounds.Contains(corner));
	EXPECT_TRUE(worldBounds.center.IsEpsilonEqual(Float3(10.0f, 0.0f, 0.0f)));
	EXPECT_TRUE(worldBounds.extent.IsEpsilonEqual(Float3(2.0f, 1.0f, 3.0f)));
}
TEST(TransformedBounds, IntersectRay)
{
	Bounds localBounds(Float3::zero, Float3::one);
	Float4x4 localToWorld = Float4x4::Translate(Float3(10.0f, 0.0f, 0.0f)) * Float4x4::RotateZ(math::pi2);
	TransformedBounds bounds(localBounds, localToWorld);
	Ray ray(Float3(localToWorld * Float4(2.0f, 0.0f, 0.0f, 1.0f)), Float3(localToWorld * Float4(-1.0f, 0.0f, 0.0f, 0.0f)));
	std::optional<Float3> hit = bounds.IntersectRay(ray);
	ASSERT_TRUE(hit.has_value());
	Float3 expected = Float3(localToWorld * Float4(1.0f, 0.0f, 0.0f, 1.0f));
	EXPECT_TRUE(hit.value().IsEpsilonEqual(expected));
}