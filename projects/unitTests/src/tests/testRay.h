#pragma once



TEST(Ray, GetPoint)
{
	Float3 origin(1.0f, 2.0f, 3.0f);
	Float3 direction = Float3::one;
	Ray ray(origin, direction);
	float distance = 5.0f;
	Float3 point = origin + distance * direction.Normalize();
	EXPECT_NEAR3(ray.GetPoint(distance), point, epsilon);
}