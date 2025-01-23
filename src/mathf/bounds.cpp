#include "bounds.h"
#include "mathf.h"
#include <sstream>



// Constructors:
Bounds::Bounds() : center(0.0f), extents(0.0f) {}
Bounds::Bounds(const Float3& center, const Float3& extents) : center(center), extents(extents) {}
Bounds::Bounds(const Bounds& bounds) : center(bounds.center), extents(bounds.extents) {}
Bounds::Bounds(const Float3* const corners)
{
	Float3 min = corners[0];
	Float3 max = corners[0];
	for (int i = 1; i < 8; i++)
	{
		min = Float3::Min(min, corners[i]);
		max = Float3::Max(max, corners[i]);
	}
	center = 0.5f * (max + min);
	extents = 0.5f * (max - min);
}
Bounds::Bounds(const std::vector<Float3>& points)
{
	Float3 min = points[0];
	Float3 max = points[0];
	for (int i = 1; i < points.size(); i++)
	{
		min = Float3::Min(min, points[i]);
		max = Float3::Max(max, points[i]);
	}
	center = 0.5f * (max + min);
	extents = 0.5f * (max - min);
}



// Public methods:
// Getters:
Float3 Bounds::GetMin() const
{
	return center - extents;
}
Float3 Bounds::GetMax() const
{
	return center + extents;
}
Float3 Bounds::GetSize() const
{
	return 2.0f * extents;
}
float Bounds::GetDiagonal() const
{
	return 2.0f * extents.Length();

}
std::array<Float3, 8> Bounds::GetCorners() const
{
	Float3 min = GetMin();
	Float3 max = GetMax();
	return std::array<Float3, 8>
	{
		Float3(min.x, min.y, min.z),
		Float3(min.x, min.y, max.z),
		Float3(min.x, max.y, min.z),
		Float3(min.x, max.y, max.z),
		Float3(max.x, min.y, min.z),
		Float3(max.x, min.y, max.z),
		Float3(max.x, max.y, min.z),
		Float3(max.x, max.y, max.z)
	};
}

// Setters:
void Bounds::SetMinMax(const Float3& min, const Float3& max)
{
	center = 0.5f * (max + min);
	extents = 0.5f * (max - min);
}

// Methods:
Float3 Bounds::ClosestPoint(const Float3& point) const
{
	Float3 min = GetMin();
	Float3 max = GetMax();
	return Float3::Clamp(point, min, max);
}
bool Bounds::Contains(const Float3& point) const
{
	Float3 min = GetMin();
	Float3 max = GetMax();
	return point.x >= min.x && point.x <= max.x
		&& point.y >= min.y && point.y <= max.y
		&& point.z >= min.z && point.z <= max.z;
}
void Bounds::Encapsulate(const Float3& point)
{
	Float3 min = Float3::Min(point, GetMin());
	Float3 max = Float3::Max(point, GetMax());
	center = 0.5f * (max + min);
	extents = 0.5f * (max - min);
}
void Bounds::Expand(float amount)
{
	extents += Float3(mathf::Abs(amount));
}
void Bounds::Expand(const Float3& amount)
{
	extents += Float3::Abs(amount);
}

// Logging:
std::string Bounds::ToString() const
{
	return "Bounds(center: " + center.ToString() + ", extents: " + extents.ToString() + ")";
}
std::ostream& operator<<(std::ostream& os, const Bounds& bounds)
{
	os << bounds.ToString();
	return os;
}