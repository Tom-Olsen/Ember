#pragma once



TEST(geometry3d, GetOrhtogonalVector)
{
	Float3 direction = math::Random::UniformDirection3();
	Float3 orthogonal = geometry3d::GetOrhtogonalVector(direction);
	float dot = Float3::Dot(direction, orthogonal);
	EXPECT_FLOAT_EQ(dot, 0.0f);
}

TEST(geometry3d, IsInsidePlane)
{
	// Plane parameterisation:
	Float3 planeSupport = Float3(1.0f, -1.0f, 2.0f);
	Float3 planeNormal = Float3(1.0f, 2.0f, 3.0f);
	Float3 dir0 = geometry3d::GetOrhtogonalVector(planeNormal).Normalize();
	Float3 dir1 = Float3::Cross(planeNormal, dir0).Normalize();

	bool allGood = true;
	for (int i = 0; i < 100; i++)
		for (int j = 0; j < 100; j++)
		{
			Float3 point = planeSupport + dir0 * i + dir1 * j;
			allGood = allGood && (geometry3d::IsInsidePlane(point, planeSupport, planeNormal));
		}
	EXPECT_TRUE(allGood);
}

TEST(geometry3d, PointToPlaneDistance)
{
	// Plane parameterisation:
	Float3 planeSupport = Float3(1.0f, -1.0f, 2.0f);
	Float3 planeNormal = Float3(1.0f, 2.0f, 3.0f);
	Float3 dir0 = geometry3d::GetOrhtogonalVector(planeNormal).Normalize();
	Float3 dir1 = Float3::Cross(planeNormal, dir0).Normalize();

	bool allGood = true;
	for (int i = 0; i < 100; i++)
		for (int j = 0; j < 100; j++)
		{
			// Generate point offset by random distance +-50 from plane:
			float distance = math::Random::Uniform(-50.0f, 50.0f);
			Float3 point = planeSupport + dir0 * i + dir1 * j;
			point += planeNormal.Normalize() * distance;

			float distanceToPlane = geometry3d::PointToPlaneDistance(point, planeSupport, planeNormal);
			allGood = allGood && (math::Abs(distanceToPlane - distance) < math::epsilon);
		}
	EXPECT_TRUE(allGood);
}

TEST(geometry3d, PointToPlaneProjection)
{
	// Plane parameterisation:
	Float3 planeSupport = Float3(1.0f, -1.0f, 2.0f);
	Float3 planeNormal = Float3(1.0f, 2.0f, 3.0f);
	Float3 dir0 = geometry3d::GetOrhtogonalVector(planeNormal).Normalize();
	Float3 dir1 = Float3::Cross(planeNormal, dir0).Normalize();

	bool allGood = true;
	for (int i = 0; i < 100; i++)
		for (int j = 0; j < 100; j++)
		{
			// Generate point offset by random distance +-50 from plane:
			float distance = math::Random::Uniform(-50.0f, 50.0f);
			Float3 point = planeSupport + dir0 * i + dir1 * j;
			point += planeNormal.Normalize() * distance;

			Float3 pointProjection = geometry3d::PointToPlaneProjection(point, planeSupport, planeNormal);
			allGood = allGood && geometry3d::IsInsidePlane(pointProjection, planeSupport, planeNormal);
		}
	EXPECT_TRUE(allGood);
}

TEST(geometry3d, ReflectPointOnPlane)
{
	// Plane parameterisation:
	Float3 planeSupport = Float3(1.0f, -1.0f, 2.0f);
	Float3 planeNormal = Float3(1.0f, 2.0f, 3.0f);
	Float3 dir0 = geometry3d::GetOrhtogonalVector(planeNormal).Normalize();
	Float3 dir1 = Float3::Cross(planeNormal, dir0).Normalize();

	bool allGood = true;
	for (int i = 0; i < 100; i++)
		for (int j = 0; j < 100; j++)
		{
			// Generate points A and B offset by random distance +-50 from plane in opposite direction:
			float distance = math::Random::Uniform(-50.0f, 50.0f);
			Float3 pointOnPlane = planeSupport + dir0 * i + dir1 * j;
			Float3 pointA = pointOnPlane + planeNormal.Normalize() * distance;
			Float3 pointB = pointOnPlane - planeNormal.Normalize() * distance;

			Float3 reflection = geometry3d::ReflectPointOnPlane(pointA, planeSupport, planeNormal);
			allGood = allGood && reflection.IsEpsilonEqual(pointB);
		}
	EXPECT_TRUE(allGood);
}

TEST(geometry3d, LinePlaneIntersection)
{
	// Plane parameterisation:
	Float3 planeSupport = Float3(1.0f, -1.0f, 2.0f);
	Float3 planeNormal = Float3(1.0f, 2.0f, 3.0f);
	Float3 dir0 = geometry3d::GetOrhtogonalVector(planeNormal).Normalize();
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
			Float3 intersection = geometry3d::LinePlaneIntersection(lineSupport, lineDirection, planeSupport, planeNormal);
			allGood = allGood && intersection.IsEpsilonEqual(point);
		}
	EXPECT_TRUE(allGood);
}