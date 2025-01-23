#ifndef __INCLUDE_GUARD_bounds2d_h__
#define __INCLUDE_GUARD_bounds2d_h__
#include "float2.h"
#include <array>
#include <vector>
#include <string>



struct Bounds2d
{
public: // Members:
	Float2 center;
	Float2 extents;

public: // Methods:
	Bounds2d();
	Bounds2d(const Float2& center, const Float2& extents);
	Bounds2d(const Bounds2d& bounds);
	Bounds2d(const Float2* const corners);
	Bounds2d(const std::vector<Float2>& points);

	// Getters:
	Float2 GetMin() const;
	Float2 GetMax() const;
	Float2 GetSize() const;
	float GetDiagonal() const;
	std::array<Float2, 4> GetCorners() const;

	// Setters:
	void SetMinMax(const Float2& min, const Float2& max);

	// Methods:
	Float2 ClosestPoint(const Float2& point) const;
	bool Contains(const Float2& point) const;
	void Encapsulate(const Float2& point);
	void Expand(float amount);
	void Expand(const Float2& amount);

	// Logging:
	std::string ToString() const;
	friend std::ostream& operator<<(std::ostream& os, const Bounds2d& bounds);
};



#endif // __INCLUDE_GUARD_bounds2d_h__