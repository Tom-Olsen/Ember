#include "emberMath.h"
#include <array>
#include <gtest/gtest.h>



TEST(RotatedBounds, Contains)
{
	Bounds localBounds(Float3(10.0f, 0.0f, 0.0f), Float3::one);
	RotatedBounds bounds(localBounds, Float3x3::RotateZ(Radians(math::pi2)));
	Float3 inside = Float3(10.0f, 0.5f, 0.0f);
	Float3 outside = Float3(10.0f, 1.5f, 0.0f);
	EXPECT_TRUE(bounds.Contains(inside));
	EXPECT_FALSE(bounds.Contains(outside));
}
TEST(RotatedBounds, ClosestPoint)
{
	Bounds localBounds(Float3(10.0f, 0.0f, 0.0f), Float3::one);
	RotatedBounds bounds(localBounds, Float3x3::RotateZ(Radians(math::pi2)));
	Float3 point = Float3(10.0f, 2.0f, 0.0f);
	Float3 expected = Float3(10.0f, 1.0f, 0.0f);
	Float3 closestPoint = bounds.ClosestPoint(point);
	EXPECT_TRUE(closestPoint.IsEpsilonEqual(expected));
}
TEST(RotatedBounds, GetCorners)
{
	Bounds localBounds(Float3(10.0f, 0.0f, 0.0f), Float3::one);
	RotatedBounds bounds(localBounds, Float3x3::RotateZ(Radians(math::pi2)));
	std::array<Float3, 8> corners = bounds.GetCorners();
	for (uint32_t i = 0; i < corners.size(); i++)
		EXPECT_TRUE(bounds.Contains(corners[i]));
	EXPECT_TRUE(corners[0].IsEpsilonEqual(Float3(11.0f, -1.0f, -1.0f)));
}
TEST(RotatedBounds, GetWorldBounds)
{
	Bounds localBounds(Float3(10.0f, 0.0f, 0.0f), Float3(1.0f, 2.0f, 3.0f));
	RotatedBounds bounds(localBounds, Float3x3::RotateZ(Radians(math::pi2)));
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
	RotatedBounds bounds(localBounds, Float3x3::RotateZ(Radians(math::pi2)));
	Ray ray(Float3(10.0f, 2.0f, 0.0f), Float3(0.0f, -1.0f, 0.0f));
	RayHit hit = bounds.IntersectRay(ray);
	ASSERT_TRUE(hit.GetHit());
	EXPECT_TRUE(math::IsEpsilonEqual(hit.GetDistance(), 1.0f));
	EXPECT_TRUE(hit.GetPoint().IsEpsilonEqual(Float3(10.0f, 1.0f, 0.0f)));
	EXPECT_TRUE(hit.GetNormal().IsEpsilonEqual(Float3::forward));
}
TEST(RotatedBounds, EulerRotation)
{
	Bounds localBounds(Float3(10.0f, 20.0f, 30.0f), Float3(1.0f, 2.0f, 3.0f));
	RotatedBounds bounds(localBounds);
	EulerDegrees degrees(25.0f, -40.0f, 65.0f);
	Float3x3 rotationMatrix = Float3x3::Rotate(degrees.ToRadians());
	bounds.SetRotationEuler(degrees);
	EXPECT_TRUE(bounds.GetRotation3x3().IsEpsilonEqual(rotationMatrix));
	EXPECT_TRUE(bounds.GetRotationEulerDegrees().IsEpsilonEqual(degrees));
	EXPECT_TRUE(bounds.GetRotationEulerRadians().IsEpsilonEqual(degrees.ToRadians()));
	EXPECT_EQ(bounds.localBounds, localBounds);

	EulerRadians radians(-0.3f, 0.8f, -1.1f);
	bounds.SetRotationEuler(radians);
	EXPECT_TRUE(bounds.GetRotationEulerRadians().IsEpsilonEqual(radians));
	EXPECT_TRUE(bounds.GetRotationEulerDegrees().IsEpsilonEqual(radians.ToDegrees()));
	EXPECT_EQ(bounds.localBounds, localBounds);

	bounds.SetRotationMatrix(rotationMatrix);
	EXPECT_TRUE(bounds.GetRotationEulerDegrees().IsEpsilonEqual(degrees));
}