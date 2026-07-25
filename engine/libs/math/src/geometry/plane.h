#pragma once
#include "float3.h"
#include "ray.h"
#include "rayHit.h"
#include <string>



namespace emberMath
{
	struct Plane
	{
	public: // Members:
		Float3 support;
		Float3 normal;

	public: // Methods:
		// Constructors:
		Plane();
		Plane(const Float3& support, const Float3& normal);
		Plane(const Plane& plane);

		// Methods:
		RayHit IntersectRay(const Ray& ray) const;

		// Equality:
		bool operator==(const Plane& other) const;
		bool operator!=(const Plane& other) const;

		// Logging:
		std::string ToString() const;
		friend std::ostream& operator<<(std::ostream& os, const Plane& plane);
	};
}