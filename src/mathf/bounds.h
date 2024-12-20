
#ifndef __INCLUDE_GUARD_bounds_h__
#define __INCLUDE_GUARD_bounds_h__
#include "float3.h"
#include <vector>
#include <string>



struct Bounds
{
public:
	// Members:
	Float3 center;
	Float3 extents;

	// Constructors:
	Bounds();
	Bounds(const Float3& center, const Float3& extents);
	Bounds(const Bounds& bounds);
	Bounds(const Float3* const points);
	Bounds(const std::vector<Float3>& points);

	// Methods:
	Float3 GetMin() const;
	Float3 GetMax() const;
	Float3 GetSize() const;
	Float3 ClosestPoint(const Float3& point) const;
	bool Contains(const Float3& point) const;
	void Encapsulate(const Float3& point);
	void Expand(float amount);
	void Expand(const Float3& amount);
	//bool IntersectRay(const Ray& ray);
	//bool Intersects(const Bounds& bounds) const;
	void SetMinMax(const Float3& min, const Float3& max);
	//float SqrDistance(const Float3& point) const;
	std::string ToString() const;
};



#endif // __INCLUDE_GUARD_bounds_h__