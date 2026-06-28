#include "emberMath.h"
#include <gtest/gtest.h>



TEST(geometry3d, GetOrhtogonalVector)
{
	Float3 direction = math::Random::UniformDirection3();
	std::optional<Float3> orthogonal = geometry3d::GetOrhtogonalVector(direction);
	ASSERT_TRUE(orthogonal.has_value());
	float dot = Float3::Dot(direction, orthogonal.value());
	EXPECT_FLOAT_EQ(dot, 0.0f);
	EXPECT_FALSE(geometry3d::GetOrhtogonalVector(Float3::zero).has_value());
}

TEST(geometry3d, IsInsidePlane)
{
	// Plane parameterisation:
	Float3 planeSupport = Float3(1.0f, -1.0f, 2.0f);
	Float3 planeNormal = Float3(1.0f, 2.0f, 3.0f);
	std::optional<Float3> orthogonal = geometry3d::GetOrhtogonalVector(planeNormal);
	ASSERT_TRUE(orthogonal.has_value());
	Float3 dir0 = orthogonal.value().Normalize();
	Float3 dir1 = Float3::Cross(planeNormal, dir0).Normalize();

	bool allGood = true;
	for (int i = 0; i < 100; i++)
		for (int j = 0; j < 100; j++)
		{
			Float3 point = planeSupport + dir0 * i + dir1 * j;
			std::optional<bool> isInsidePlane = geometry3d::IsInsidePlane(point, planeSupport, planeNormal);
			allGood = allGood && isInsidePlane.has_value() && isInsidePlane.value();
		}
	EXPECT_TRUE(allGood);
	EXPECT_FALSE(geometry3d::IsInsidePlane(planeSupport, planeSupport, Float3::zero).has_value());
}

TEST(geometry3d, PointToPlaneDistance)
{
	// Plane parameterisation:
	Float3 planeSupport = Float3(1.0f, -1.0f, 2.0f);
	Float3 planeNormal = Float3(1.0f, 2.0f, 3.0f);
	std::optional<Float3> orthogonal = geometry3d::GetOrhtogonalVector(planeNormal);
	ASSERT_TRUE(orthogonal.has_value());
	Float3 dir0 = orthogonal.value().Normalize();
	Float3 dir1 = Float3::Cross(planeNormal, dir0).Normalize();

	bool allGood = true;
	for (int i = 0; i < 100; i++)
		for (int j = 0; j < 100; j++)
		{
			// Generate point offset by random distance +-50 from plane:
			float distance = math::Random::Uniform(-50.0f, 50.0f);
			Float3 point = planeSupport + dir0 * i + dir1 * j;
			point += planeNormal.Normalize() * distance;

			std::optional<float> distanceToPlane = geometry3d::PointToPlaneDistance(point, planeSupport, planeNormal);
			allGood &= distanceToPlane.has_value() && math::IsEpsilonEqual(distanceToPlane.value(), distance);
		}
	EXPECT_TRUE(allGood);
	EXPECT_FALSE(geometry3d::PointToPlaneDistance(planeSupport, planeSupport, Float3::zero).has_value());
}

TEST(geometry3d, PointToPlaneProjection)
{
	// Plane parameterisation:
	Float3 planeSupport = Float3(1.0f, -1.0f, 2.0f);
	Float3 planeNormal = Float3(1.0f, 2.0f, 3.0f);
	std::optional<Float3> orthogonal = geometry3d::GetOrhtogonalVector(planeNormal);
	ASSERT_TRUE(orthogonal.has_value());
	Float3 dir0 = orthogonal.value().Normalize();
	Float3 dir1 = Float3::Cross(planeNormal, dir0).Normalize();

	bool allGood = true;
	for (int i = 0; i < 100; i++)
		for (int j = 0; j < 100; j++)
		{
			// Generate point offset by random distance +-50 from plane:
			float distance = math::Random::Uniform(-50.0f, 50.0f);
			Float3 point = planeSupport + dir0 * i + dir1 * j;
			point += planeNormal.Normalize() * distance;

			std::optional<Float3> pointProjection = geometry3d::PointToPlaneProjection(point, planeSupport, planeNormal);
			std::optional<bool> isInsidePlane = pointProjection.has_value() ? geometry3d::IsInsidePlane(pointProjection.value(), planeSupport, planeNormal) : std::nullopt;
			allGood = allGood && isInsidePlane.has_value() && isInsidePlane.value();
		}
	EXPECT_TRUE(allGood);
	EXPECT_FALSE(geometry3d::PointToPlaneProjection(planeSupport, planeSupport, Float3::zero).has_value());
}

TEST(geometry3d, ReflectPointOnPlane)
{
	// Plane parameterisation:
	Float3 planeSupport = Float3(1.0f, -1.0f, 2.0f);
	Float3 planeNormal = Float3(1.0f, 2.0f, 3.0f);
	std::optional<Float3> orthogonal = geometry3d::GetOrhtogonalVector(planeNormal);
	ASSERT_TRUE(orthogonal.has_value());
	Float3 dir0 = orthogonal.value().Normalize();
	Float3 dir1 = Float3::Cross(planeNormal, dir0).Normalize();

	bool allGood = true;
	for (int i = 0; i < 100; i++)
		for (int j = 0; j < 100; j++)
		{
			// Generate points A and B offset by distance +-50 from plane in opposite direction:
			float distance = -49.5f + 0.01f * (i * 100 + j);
			Float3 pointOnPlane = planeSupport + dir0 * i + dir1 * j;
			Float3 pointA = pointOnPlane + planeNormal.Normalize() * distance;
			Float3 pointB = pointOnPlane - planeNormal.Normalize() * distance;

			std::optional<Float3> reflection = geometry3d::ReflectPointOnPlane(pointA, planeSupport, planeNormal);
			allGood = allGood && reflection.has_value() && reflection.value().IsEpsilonEqual(pointB);
		}
	EXPECT_TRUE(allGood);
	EXPECT_FALSE(geometry3d::ReflectPointOnPlane(planeSupport, planeSupport, Float3::zero).has_value());
}

TEST(geometry3d, LinePlaneIntersection)
{
	// Plane parameterisation:
	Float3 planeSupport = Float3(1.0f, -1.0f, 2.0f);
	Float3 planeNormal = Float3(1.0f, 2.0f, 3.0f);
	std::optional<Float3> orthogonal = geometry3d::GetOrhtogonalVector(planeNormal);
	ASSERT_TRUE(orthogonal.has_value());
	Float3 dir0 = orthogonal.value().Normalize();
	Float3 dir1 = Float3::Cross(planeNormal, dir0).Normalize();

	bool allGood = true;
	for (int i = 0; i < 100; i++)
		for (int j = 0; j < 100; j++)
		{
			Float3 point = planeSupport + dir0 * i + dir1 * j;

			// Random line direction:
			Float3 lineDirection = planeNormal;
			lineDirection += dir0 * math::Random::Uniform(-10.0f, 10.0f);
			lineDirection += dir1 * math::Random::Uniform(-10.0f, 10.0f);
			lineDirection.Normalize();
			
			// Line support at random distance:
			float distance = math::Random::Uniform(-20.0f, 20.0f);
			Float3 lineSupport = point + distance * lineDirection;

			// Intersection of line and plane should be the original point:
			std::optional<Float3> intersection = geometry3d::LinePlaneIntersection(lineSupport, lineDirection, planeSupport, planeNormal);
			allGood = allGood && intersection.has_value() && intersection.value().IsEpsilonEqual(point);
		}
	EXPECT_TRUE(allGood);
	EXPECT_FALSE(geometry3d::LinePlaneIntersection(planeSupport, Float3::zero, planeSupport, planeNormal).has_value());
	EXPECT_FALSE(geometry3d::LinePlaneIntersection(planeSupport, dir0, planeSupport, Float3::zero).has_value());
	EXPECT_FALSE(geometry3d::LinePlaneIntersection(planeSupport, dir0, planeSupport, planeNormal).has_value());
}