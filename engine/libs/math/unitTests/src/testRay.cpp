#include "emberMath.h"
#include "unitTestHelper.h"
#include <gtest/gtest.h>



TEST(Ray, GetPoint)
{
	Float3 origin(1.0f, 2.0f, 3.0f);
	Float3 direction = Float3::one;
	Ray ray(origin, direction);
	float distance = 5.0f;
	Float3 point = origin + distance * direction.Normalize();
	ExpectNearVec(ray.GetPoint(distance), point, epsilon);
}