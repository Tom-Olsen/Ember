#pragma once
#include "float3.h"
#include <optional>
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
		Ray(const Float3& origin, const Float3& direction);

		// Methods:
		Float3 GetPoint(float distance) const;
		std::optional<Float3> HitOnPlane(const Float3& planeSupport, const Float3& planeNormal) const;

		// Logging:
		std::string ToString() const;
		friend std::ostream& operator<<(std::ostream& os, const Ray& ray);
	};
}