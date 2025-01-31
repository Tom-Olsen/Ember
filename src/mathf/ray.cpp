#include "ray.h"
#include "float3.h"
#include <sstream>



// Constructor:
Ray::Ray(Float3 origin, Float3 direction) : origin(origin), direction(direction.Normalize()) {}



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