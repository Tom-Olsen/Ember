#pragma once
#include "float3.h"
#include "ray.h"
#include <optional>
#include <string>



namespace emberMath
{
	struct Capsule
	{
	public: // Members:
		Float3 point0;
		Float3 point1;
		float radius;

	public: // Methods:
		Capsule();
		Capsule(const Float3& point0, const Float3& point1, float radius);
		Capsule(const Capsule& capsule);

		// Getters:
		Float3 GetCenter() const;
		Float3 GetDirection() const;
		float GetLength() const;

		// Methods:
		bool Contains(const Float3& point) const;
		Float3 ClosestPoint(const Float3& point) const; // point outside => closest point on surface. pointinside => point.
		std::optional<Float3> IntersectRay(const Ray& ray) const;

		// Equality:
		bool operator==(const Capsule& other) const;
		bool operator!=(const Capsule& other) const;

		// Logging:
		std::string ToString() const;
		friend std::ostream& operator<<(std::ostream& os, const Capsule& capsule);
	};
}