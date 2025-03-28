#ifndef __INCLUDE_GUARD_ray_h__
#define __INCLUDE_GUARD_ray_h__
#include "float3.h"
#include <string>



namespace emberMath
{
	// Ray from origin in direction.
	struct Ray
	{
	public:
		// Members:
		Float3 origin;
		Float3 direction;

		// Constructor:
		Ray(Float3 origin, Float3 direction);

		// Methods:
		Float3 GetPoint(float distance) const;

		// Logging:
		std::string ToString() const;
		friend std::ostream& operator<<(std::ostream& os, const Ray& ray);
	};
}



#endif // __INCLUDE_GUARD_ray_h__