#include "bounds2d.h"
#include "mathFunctions.h"
#include <sstream>



namespace emberMath
{
	// Constructors:
	Bounds2d::Bounds2d() : center(0.0f), extents(0.0f), depth(0.0f) {}
	Bounds2d::Bounds2d(const Float2& center, const Float2& extents, float depth) : center(center), extents(extents), depth(depth) {}
	Bounds2d::Bounds2d(const Bounds2d& bounds) : center(bounds.center), extents(bounds.extents), depth(bounds.depth) {}
	Bounds2d::Bounds2d(const Float2* const corners, float depth) : depth(depth)
	{
		Float2 min = corners[0];
		Float2 max = corners[0];
		for (int i = 1; i < 4; i++)
		{
			min = Float2::Min(min, corners[i]);
			max = Float2::Max(max, corners[i]);
		}
		center = 0.5f * (max + min);
		extents = 0.5f * (max - min);
	}
	Bounds2d::Bounds2d(const std::vector<Float2>& points, float depth) : depth(depth)
	{
		Float2 min = points[0];
		Float2 max = points[0];
		for (int i = 1; i < points.size(); i++)
		{
			min = Float2::Min(min, points[i]);
			max = Float2::Max(max, points[i]);
		}
		center = 0.5f * (max + min);
		extents = 0.5f * (max - min);
	}



	// Public methods:
	// Getters:
	Float2 Bounds2d::GetMin() const
	{
		return center - extents;
	}
	Float2 Bounds2d::GetMax() const
	{
		return center + extents;
	}
	Float2 Bounds2d::GetSize() const
	{
		return 2.0f * extents;
	}
	Float3 Bounds2d::GetCenter3D() const
	{
		return Float3(center, depth);
	}
	float Bounds2d::GetDiagonal() const
	{
		return 2.0f * extents.Length();

	}
	std::array<Float2, 4> Bounds2d::GetCorners() const
	{
		Float2 min = GetMin();
		Float2 max = GetMax();
		return std::array<Float2, 4>
		{
			Float2(min.x, min.y),
			Float2(min.x, max.y),
			Float2(max.x, min.y),
			Float2(max.x, max.y)
		};
	}

	// Setters:
	void Bounds2d::SetMinMax(const Float2& min, const Float2& max)
	{
		center = 0.5f * (max + min);
		extents = 0.5f * (max - min);
	}

	// Methods:
	Float2 Bounds2d::ClosestPoint(const Float2& point) const
	{
		Float2 min = GetMin();
		Float2 max = GetMax();
		return Float2::Clamp(point, min, max);
	}
	Float2 Bounds2d::ClosestPointOnSurface(const Float2& point) const
	{
		Float2 min = GetMin();
		Float2 max = GetMax();

		// If the point is outside the bounds, clamp it:
		if (point.x < min.x || point.x > max.x || point.y < min.y || point.y > max.y)
			return Float2::Clamp(point, min, max);

		// Compute distances to each edge:
		float distLeft = point.x - min.x;
		float distRight = max.x - point.x;
		float distBottom = point.y - min.y;
		float distTop = max.y - point.y;

		// Find the minimum distance and snap closestPoint to it:
		float minDist = distLeft;
		Float2 closestPoint = Float2(min.x, point.y);
		if (distRight < minDist)
		{
			minDist = distRight;
			closestPoint = Float2(max.x, point.y);
		}
		if (distBottom < minDist)
		{
			minDist = distBottom;
			closestPoint = Float2(point.x, min.y);
		}
		if (distTop < minDist)
		{
			minDist = distTop;
			closestPoint = Float2(point.x, max.y);
		}
		return closestPoint;
	}
	bool Bounds2d::Contains(const Float2& point) const
	{
		Float2 min = GetMin();
		Float2 max = GetMax();
		return point.x >= min.x && point.x <= max.x
			&& point.y >= min.y && point.y <= max.y;
	}
	void Bounds2d::Encapsulate(const Float2& point)
	{
		Float2 min = Float2::Min(point, GetMin());
		Float2 max = Float2::Max(point, GetMax());
		center = 0.5f * (max + min);
		extents = 0.5f * (max - min);
	}
	void Bounds2d::Expand(float amount)
	{
		extents += Float2(math::Abs(amount));
	}
	void Bounds2d::Expand(const Float2& amount)
	{
		extents += Float2::Abs(amount);
	}

	// Logging:
	std::string Bounds2d::ToString() const
	{
		return "Bounds2d(center: " + center.ToString() + ", extents: " + extents.ToString() + ")";
	}
	std::ostream& operator<<(std::ostream& os, const Bounds2d& bounds)
	{
		os << bounds.ToString();
		return os;
	}
}