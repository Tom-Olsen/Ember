#include "bounds.h"
#include "mathf.h"



// Constructors:
Bounds::Bounds() : center(0.0f), extents(0.0f) {}
Bounds::Bounds(const Float3& center, const Float3& extents) : center(center), extents(extents) {}
Bounds::Bounds(const Bounds& bounds) : center(bounds.center), extents(bounds.extents) {}
Bounds::Bounds(const Float3* const points)
{
	Float3 min = points[0];
	Float3 max = points[0];
	for (int i = 1; i < 8; i++)
	{
		min = Float3::Min(min, points[i]);
		max = Float3::Max(max, points[i]);
	}
	center = 0.5f * (max + min);
	extents = 0.5f * (max - min);
}
Bounds::Bounds(const std::vector<Float3>& points)
{
	this->Bounds::Bounds(points.data());
}



// Public methods:
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
//bool Bounds::IntersectRay(const Ray& ray)
//{
//
//}
//bool Bounds::Intersects(const Bounds& bounds) const
//{
//
//}
void Bounds::SetMinMax(const Float3& min, const Float3& max)
{
	center = 0.5f * (max + min);
	extents = 0.5f * (max - min);
}
//float Bounds::SqrDistance(const Float3& point) const
//{
//
//}
std::string Bounds::ToString() const
{
	return "Bounds(center: " + center.ToString() + ", extents: " + extents.ToString() + ")";
}