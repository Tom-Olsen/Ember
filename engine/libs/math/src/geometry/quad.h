#pragma once
#include "float3.h"
#include "ray.h"
#include <optional>
#include <string>



namespace emberMath
{
	struct Quad
	{
	public: // Members:
		Float3 origin;	// quad corner.
		Float3 uAxis;	// edge vector from origin along the first side (full length).
		Float3 vAxis;	// edge vector from origin along the second side (full length).

	public: // Methods:
		Quad();
		Quad(const Float3& origin, const Float3& uAxis, const Float3& vAxis);
		Quad(const Quad& quad);

		// Getters:
		Float3 GetCenter() const;
		Float3 GetNormal() const;

		// Methods:
		bool Contains(const Float3& point) const;
		Float3 ClosestPoint(const Float3& point) const; // closest point on the quad surface clamped to its bounds.
		std::optional<Float3> IntersectRay(const Ray& ray) const;

		// Equality:
		bool operator==(const Quad& other) const;
		bool operator!=(const Quad& other) const;

		// Logging:
		std::string ToString() const;
		friend std::ostream& operator<<(std::ostream& os, const Quad& quad);
	};
}
