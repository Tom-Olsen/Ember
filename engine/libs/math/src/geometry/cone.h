#pragma once
#include "float2.h"
#include "float3.h"
#include "ray.h"
#include <optional>
#include <string>



namespace emberMath
{
	struct Cone
	{
	public: // Members:
		Float3 apex;
		Float3 baseCenter;
		float radius;

	public: // Methods:
		Cone();
		Cone(const Float3& apex, const Float3& baseCenter, float radius);
		Cone(const Cone& cone);

		// Getters:
		Float3 GetCenter() const;
		Float3 GetAxis() const;	// unit direction from apex towards baseCenter.
		float GetHeight() const;

		// Methods:
		bool Contains(const Float3& point) const;
		Float3 ClosestPoint(const Float3& point) const; // point outside => closest point on surface. point inside => point.
		std::optional<Float3> IntersectRay(const Ray& ray) const;

		// Equality:
		bool operator==(const Cone& other) const;
		bool operator!=(const Cone& other) const;

		// Logging:
		std::string ToString() const;
		friend std::ostream& operator<<(std::ostream& os, const Cone& cone);

	private: // Methods:
        static Float2 ClosestPointOnSegment(const Float2& point, const Float2& a, const Float2& b);
        void UpdateClosestLateralHit(const Ray& ray, float dist, const Float3& axisDir, float height, float& closestDist) const;
	};
}