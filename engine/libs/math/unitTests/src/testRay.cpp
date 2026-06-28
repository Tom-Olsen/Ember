#include "emberMath.h"
#include <gtest/gtest.h>
#include <optional>



TEST(Ray, GetPoint)
{
	Float3 origin(1.0f, 2.0f, 3.0f);
	Float3 direction = Float3::one;
	Ray ray(origin, direction);
	float distance = 5.0f;
	Float3 point = origin + distance * direction.Normalize();
	EXPECT_TRUE(ray.GetPoint(distance).IsEpsilonEqual(point));
}

TEST(Ray, HitOnPlane)
{
	Ray ray(Float3(1.0f, 2.0f, 3.0f), Float3::forward);
	Float3 planeSupport(4.0f, 7.0f, 6.0f);
	Float3 planeNormal = Float3::forward;

	std::optional<Float3> hit = ray.HitOnPlane(planeSupport, planeNormal);
	ASSERT_TRUE(hit.has_value());
	EXPECT_TRUE(hit.value().IsEpsilonEqual(Float3(1.0f, 7.0f, 3.0f)));
}

TEST(Ray, HitOnPlaneBehindOrigin)
{
	Ray ray(Float3(1.0f, 2.0f, 3.0f), Float3::forward);
	Float3 planeSupport(4.0f, -7.0f, 6.0f);
	Float3 planeNormal = Float3::forward;

	EXPECT_FALSE(ray.HitOnPlane(planeSupport, planeNormal).has_value());
}

TEST(Ray, HitOnPlaneParallel)
{
	Ray ray(Float3(1.0f, 2.0f, 3.0f), Float3::forward);
	Float3 planeSupport(4.0f, 7.0f, 6.0f);
	Float3 planeNormal = Float3::right;

	EXPECT_FALSE(ray.HitOnPlane(planeSupport, planeNormal).has_value());
}

TEST(Ray, HitOnPlaneInvalidPlane)
{
	Ray ray(Float3(1.0f, 2.0f, 3.0f), Float3::forward);
	Float3 planeSupport(4.0f, 7.0f, 6.0f);

	EXPECT_FALSE(ray.HitOnPlane(planeSupport, Float3::zero).has_value());
}