#include "sphere.h"
#include "mathFunctions.h"
#include <sstream>



namespace emberMath
{
	// Public methods:
	// Constructors:
	Sphere::Sphere(): center(Float3::zero), radius(0.5f) {}
	Sphere::Sphere(const Float3& center, float radius)
		: center(center), radius(radius)
	{
		this->radius = math::Max(0.0f, radius);
	}
	Sphere::Sphere(const Sphere& sphere)
		: center(sphere.center), radius(sphere.radius) {}



	// Methods:
	bool Sphere::Contains(const Float3& point) const
	{
		return Float3::DistanceSq(point, center) <= radius * radius;
	}
	Float3 Sphere::ClosestPoint(const Float3& point) const
	{
		Float3 centerToPoint = point - center;
		float distSq = centerToPoint.LengthSq();
		if (distSq <= radius * radius || math::IsEpsilonZero(distSq))
			return point;
		return center + radius * centerToPoint / math::Sqrt(distSq);
	}
	RayHit Sphere::IntersectRay(const Ray& ray) const
	{
		Float3 originToCenter = ray.origin - center;
		float b = 2.0f * Float3::Dot(originToCenter, ray.direction);
		float c = originToCenter.LengthSq() - radius * radius;
		float discriminant = b * b - 4.0f * c;
		if (discriminant < 0.0f)
			return RayHit();

		float sqrtDiscriminant = math::Sqrt(discriminant);
		float dist0 = 0.5f * (-b - sqrtDiscriminant);
		float dist1 = 0.5f * (-b + sqrtDiscriminant);
		float distance = dist0 >= 0.0f ? dist0 : dist1;
		if (distance < 0.0f)
			return RayHit();

		Float3 point = ray.GetPoint(distance);
		Float3 centerToPoint = point - center;
		Float3 normal = centerToPoint.IsEpsilonZero() ? Float3::zero : centerToPoint.Normalize();
		return RayHit(distance, point, normal);
	}



	// Equality:
	bool Sphere::operator==(const Sphere& other) const
	{
		return center == other.center && radius == other.radius;
	}
	bool Sphere::operator!=(const Sphere& other) const
	{
		return !((*this) == other);
	}



	// Logging:
	std::string Sphere::ToString() const
	{
		return "Sphere(center: " + center.ToString() + ", radius: " + std::to_string(radius) + ")";
	}
	std::ostream& operator<<(std::ostream& os, const Sphere& sphere)
	{
		os << sphere.ToString();
		return os;
	}
}