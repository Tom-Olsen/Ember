#pragma once
#include "float3.h"
#include "ray.h"
#include <optional>
#include <string>



namespace emberMath
{
	struct Sphere
	{
	public: // Members:
		Float3 center;
		float radius;

	public: // Methods:
        // Constructors:
		Sphere();
		Sphere(const Float3& center, float radius);
		Sphere(const Sphere& sphere);

		// Methods:
		bool Contains(const Float3& point) const;
		Float3 ClosestPoint(const Float3& point) const; // point outside => closest point on surface. point inside => point.
		std::optional<Float3> IntersectRay(const Ray& ray) const;

		// Equality:
		bool operator==(const Sphere& other) const;
		bool operator!=(const Sphere& other) const;

		// Logging:
		std::string ToString() const;
		friend std::ostream& operator<<(std::ostream& os, const Sphere& sphere);
	};
}