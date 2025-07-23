#ifndef __INCLUDE_GUARD_bounds_h__
#define __INCLUDE_GUARD_bounds_h__
#include "float3.h"
#include "ray.h"
#include <array>
#include <optional>
#include <vector>
#include <string>



namespace emberMath
{
	struct Bounds
	{
	public: // Members:
		Float3 center;
		Float3 extents;

	public: // Methods:
		Bounds();
		Bounds(const Float3& center, const Float3& extents);
		Bounds(const Bounds& bounds);
		Bounds(const Float3* const corners);
		Bounds(const std::vector<Float3>& points);

		// Getters:
		Float3 GetMin() const;
		Float3 GetMax() const;
		Float3 GetSize() const;
		float GetDiagonal() const;
		std::array<Float3, 8> GetCorners() const;

		// Setters:
		void SetMinMax(const Float3& min, const Float3& max);

		// Methods:
		Float3 ClosestPoint(const Float3& point) const;
		bool Contains(const Float3& point) const;
		void Encapsulate(const Float3& point);
		void Expand(float amount);
		void Expand(const Float3& amount);
		std::optional<Float3> IntersectRay(const Ray& ray) const;	// Math::TODO: needs unit test

		// Equallity:
		bool operator==(const Bounds& other);	// Math::TODO: needs unit test	
		bool operator!=(const Bounds& other);	// Math::TODO: needs unit test

		// Logging:
		std::string ToString() const;
		friend std::ostream& operator<<(std::ostream& os, const Bounds& bounds);
	};
}



#endif // __INCLUDE_GUARD_bounds_h__