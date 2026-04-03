#include "ray.h"
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