#pragma once
#include "float3.h"
#include "ray.h"
#include "rayTriangleHit.h"
#include <string>



namespace emberMath
{
    struct Triangle
    {
	public: // Members:
		Float3 a;
		Float3 b;
		Float3 c;

	public: // Methods:
		// Constructors:
		Triangle();
		Triangle(const Float3& a, const Float3& b, const Float3& c);
		Triangle(const Triangle& triangle);

		// Getters:
		Float3 GetCenter() const;
		Float3 GetNormal() const;

		// Methods:
		bool Contains(const Float3& point) const;
		Float3 ClosestPoint(const Float3& point) const;
		RayTriangleHit IntersectRay(const Ray& ray) const;

		// Equality:
		bool operator==(const Triangle& other) const;
		bool operator!=(const Triangle& other) const;

		// Logging:
		std::string ToString() const;
		friend std::ostream& operator<<(std::ostream& os, const Triangle& triangle);

	private: // Methods:
		static Float3 ClosestPointOnSegment(const Float3& point, const Float3& a, const Float3& b);
    };
}