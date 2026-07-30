#include "plane.h"
#include "mathFunctions.h"
#include <sstream>



namespace emberMath
{
	// Public methods:
	// Constructors:
	Plane::Plane() : support(Float3::zero), normal(Float3::up) {}
	Plane::Plane(const Float3& support, const Float3& normal)
		: support(support), normal(normal) {}
	Plane::Plane(const Plane& plane)
		: support(plane.support), normal(plane.normal) {}



	// Methods:
	RayHit Plane::IntersectRay(const Ray& ray) const
	{
		if (normal.IsEpsilonZero())
			return RayHit();

		float denominator = Float3::Dot(ray.direction, normal);
		if (math::IsEpsilonZero(denominator))
			return RayHit();

		float distance = Float3::Dot(support - ray.origin, normal) / denominator;
		if (distance < 0.0f)
			return RayHit();

		return RayHit(distance, ray.GetPoint(distance), normal.Normalize());
	}



	// Equality:
	bool Plane::operator==(const Plane& other) const
	{
		return support == other.support && normal == other.normal;
	}
	bool Plane::operator!=(const Plane& other) const
	{
		return !((*this) == other);
	}



	// Logging:
	std::string Plane::ToString() const
	{
		return "Plane(support: " + support.ToString() + ", normal: " + normal.ToString() + ")";
	}
	std::ostream& operator<<(std::ostream& os, const Plane& plane)
	{
		os << plane.ToString();
		return os;
	}
}