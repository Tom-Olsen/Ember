#include "ray.h"
#include "mathFunctions.h"
#include <cassert>
#include <sstream>



namespace emberMath
{
	// Constructor:
	Ray::Ray(const Float3& origin, const Float3& direction)
    : origin(origin)
    {
        assert(!direction.IsEpsilonZero());
        this->direction = direction.Normalize();
    }



	// Methods:
	Float3 Ray::GetPoint(float distance) const
	{
		return origin + distance * direction;
	}
	std::optional<Float3> Ray::HitOnPlane(const Float3& planeSupport, const Float3& planeNormal) const
	{
		if (planeNormal.IsEpsilonZero())
			return std::nullopt;

		float denominator = Float3::Dot(direction, planeNormal);
		if (math::IsEpsilonZero(denominator))
			return std::nullopt;

		float distance = Float3::Dot(planeSupport - origin, planeNormal) / denominator;
		if (distance < 0.0f)
			return std::nullopt;

		return GetPoint(distance);
	}



	// Logging:
	std::string Ray::ToString() const
	{
		std::ostringstream oss;
		oss << "origin: " << origin.ToString() << ", direction: " << direction.ToString();
		return oss.str();
	}
	std::ostream& operator<<(std::ostream& os, const Ray& ray)
	{
		os << ray.ToString();
		return os;
	}
}