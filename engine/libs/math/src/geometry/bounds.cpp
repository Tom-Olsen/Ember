#include "bounds.h"
#include "mathConstants.h"
#include "mathFunctions.h"
#include <cassert>
#include <sstream>



namespace emberMath
{
	// Public methods:
	// Constructors:
	Bounds::Bounds() : center(0.0f), extent(0.0f) {}
	Bounds::Bounds(const Float3& center, const Float3& extent) : center(center), extent(extent) {}
	Bounds::Bounds(const Bounds& bounds) : center(bounds.center), extent(bounds.extent) {}
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
		extent = 0.5f * (max - min);
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
		extent = 0.5f * (max - min);
	}



	// Getters:
	Float3 Bounds::GetMin() const
	{
		return center - extent;
	}
	Float3 Bounds::GetMax() const
	{
		return center + extent;
	}
	Float3 Bounds::GetSize() const
	{
		return 2.0f * extent;
	}
	float Bounds::GetDiagonal() const
	{
		return 2.0f * extent.Length();

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
        assert(min < max);
		center = 0.5f * (max + min);
		extent = 0.5f * (max - min);
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
		extent = 0.5f * (max - min);
	}
    void Bounds::Encapsulate(const Bounds& bounds)
    {
        Encapsulate(bounds.GetMin());
        Encapsulate(bounds.GetMax());
    }
	void Bounds::Expand(float amount)
	{
		extent += Float3(amount);
        if (amount < 0.0f)
            extent = Float3::Max(extent, Float3(0.0f));
	}
	void Bounds::Expand(const Float3& amount)
	{
		extent += Float3::Abs(amount);
        extent = Float3::Max(extent, Float3(0.0f));
	}
	RayHit Bounds::IntersectRay(const Ray& ray) const
	{
		Float3 min = GetMin();
		Float3 max = GetMax();
		float enterDist = -math::maxValue;
		float exitDist = math::maxValue;
		Float3 enterNormal = Float3::zero;
		Float3 exitNormal = Float3::zero;

		if (ray.direction.x == 0.0f)
		{
			if (ray.origin.x < min.x || ray.origin.x > max.x)
				return RayHit();
		}
		else
		{
			float invDir = 1.0f / ray.direction.x;
			float dist0 = (min.x - ray.origin.x) * invDir;
			float dist1 = (max.x - ray.origin.x) * invDir;
			Float3 normal0 = Float3::left;
			Float3 normal1 = Float3::right;
			if (dist0 > dist1)
			{
				float temp = dist0;
				dist0 = dist1;
				dist1 = temp;
				Float3 tempNormal = normal0;
				normal0 = normal1;
				normal1 = tempNormal;
			}
			if (dist0 > enterDist)
			{
				enterDist = dist0;
				enterNormal = normal0;
			}
			if (dist1 < exitDist)
			{
				exitDist = dist1;
				exitNormal = normal1;
			}
		}

		if (ray.direction.y == 0.0f)
		{
			if (ray.origin.y < min.y || ray.origin.y > max.y)
				return RayHit();
		}
		else
		{
			float invDir = 1.0f / ray.direction.y;
			float dist0 = (min.y - ray.origin.y) * invDir;
			float dist1 = (max.y - ray.origin.y) * invDir;
			Float3 normal0 = Float3::back;
			Float3 normal1 = Float3::forward;
			if (dist0 > dist1)
			{
				float temp = dist0;
				dist0 = dist1;
				dist1 = temp;
				Float3 tempNormal = normal0;
				normal0 = normal1;
				normal1 = tempNormal;
			}
			if (dist0 > enterDist)
			{
				enterDist = dist0;
				enterNormal = normal0;
			}
			if (dist1 < exitDist)
			{
				exitDist = dist1;
				exitNormal = normal1;
			}
		}

		if (ray.direction.z == 0.0f)
		{
			if (ray.origin.z < min.z || ray.origin.z > max.z)
				return RayHit();
		}
		else
		{
			float invDir = 1.0f / ray.direction.z;
			float dist0 = (min.z - ray.origin.z) * invDir;
			float dist1 = (max.z - ray.origin.z) * invDir;
			Float3 normal0 = Float3::down;
			Float3 normal1 = Float3::up;
			if (dist0 > dist1)
			{
				float temp = dist0;
				dist0 = dist1;
				dist1 = temp;
				Float3 tempNormal = normal0;
				normal0 = normal1;
				normal1 = tempNormal;
			}
			if (dist0 > enterDist)
			{
				enterDist = dist0;
				enterNormal = normal0;
			}
			if (dist1 < exitDist)
			{
				exitDist = dist1;
				exitNormal = normal1;
			}
		}

		if (enterDist > exitDist || exitDist < 0.0f)
			return RayHit();

		float distance = enterDist < 0.0f ? exitDist : enterDist;
		Float3 normal = enterDist < 0.0f ? exitNormal : enterNormal;
		return RayHit(distance, ray.GetPoint(distance), normal);
	}



	// Equality:
	bool Bounds::operator == (const Bounds & other) const
	{
		return center == other.center && extent == other.extent;
	}
	bool Bounds::operator!=(const Bounds& other) const
	{
		return !((*this) == other);
	}


    
	// Logging:
	std::string Bounds::ToString() const
	{
		return "Bounds(center: " + center.ToString() + ", extent: " + extent.ToString() + ")";
	}
	std::ostream& operator<<(std::ostream& os, const Bounds& bounds)
	{
		os << bounds.ToString();
		return os;
	}
}