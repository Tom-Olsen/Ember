#include "cone.h"
#include "mathFunctions.h"
#include <cassert>
#include <sstream>



namespace emberMath
{
	// Public methods:
	// Constructors:
	Cone::Cone(): apex(Float3::zero), baseCenter(Float3::up), radius(0.5f) {}
	Cone::Cone(const Float3& apex, const Float3& baseCenter, float radius)
        : apex(apex), baseCenter(baseCenter), radius(radius)
	{
		this->radius = math::Max(0.0f, radius);
	}
	Cone::Cone(const Cone& cone)
        : apex(cone.apex), baseCenter(cone.baseCenter), radius(cone.radius) {}



	// Getters:
	Float3 Cone::GetCenter() const
	{
		return 0.5f * (apex + baseCenter);
	}
	Float3 Cone::GetAxis() const
	{
		Float3 direction = baseCenter - apex;
		return direction.IsEpsilonZero() ? Float3::up : direction.Normalize();
	}
	float Cone::GetHeight() const
	{
		return Float3::Distance(apex, baseCenter);
	}



	// Methods:
	bool Cone::Contains(const Float3& point) const
	{
		Float3 axis = apex - baseCenter;
		float heightSq = axis.LengthSq();
		if (math::IsEpsilonZero(heightSq))
			return false;

		float t = Float3::Dot(point - baseCenter, axis) / heightSq;
		if (t < 0.0f || t > 1.0f)
			return false;

		Float3 axisPoint = baseCenter + t * axis;
		float allowedRadius = radius * (1.0f - t);
		return Float3::DistanceSq(point, axisPoint) <= allowedRadius * allowedRadius;
	}
	Float3 Cone::ClosestPoint(const Float3& point) const
	{
		Float3 axisDir = GetAxis();
		float height = GetHeight();
		if (math::IsEpsilonZero(height))
			return apex;

		// Decompose the point into axial (s) and radial (rho) coordinates relative to the apex:
		Float3 rel = point - apex;
		float s = Float3::Dot(rel, axisDir);
		Float3 radialVec = rel - s * axisDir;
		float rho = radialVec.Length();
		Float3 radialDir = math::IsEpsilonZero(rho) ? Float3::zero : radialVec / rho;

		// Inside cone:
		if (s >= 0.0f && s <= height && rho <= radius * (s / height))
			return point;

		// Find the closest point on the boundary in the 2D (s, rho) profile:
		Float2 profilePoint = Float2(s, rho);
		Float2 slant = ClosestPointOnSegment(profilePoint, Float2(0.0f, 0.0f), Float2(height, radius));
		Float2 cap = ClosestPointOnSegment(profilePoint, Float2(height, radius), Float2(height, 0.0f));

		Float2 best = Float2::DistanceSq(profilePoint, slant) <= Float2::DistanceSq(profilePoint, cap) ? slant : cap;
		return apex + best.x * axisDir + best.y * radialDir;
	}
	std::optional<Float3> Cone::IntersectRay(const Ray& ray) const
	{
		Float3 axisDir = GetAxis();
		float height = GetHeight();
		if (math::IsEpsilonZero(height))
			return std::nullopt;

		float radiusSq = radius * radius;
		float closestDist = math::maxValue;

		// Lateral surface:
		float cos2 = height * height / (height * height + radiusSq);
		Float3 co = ray.origin - apex;
		float dv = Float3::Dot(ray.direction, axisDir);
		float cov = Float3::Dot(co, axisDir);
		float a = dv * dv - cos2;
		float b = 2.0f * (dv * cov - Float3::Dot(ray.direction, co) * cos2);
		float c = cov * cov - Float3::Dot(co, co) * cos2;

		if (!math::IsEpsilonZero(a))
		{
			float discriminant = b * b - 4.0f * a * c;
			if (discriminant >= 0.0f)
			{
				float sqrtDiscriminant = math::Sqrt(discriminant);
				float invDenominator = 0.5f / a;
				UpdateClosestLateralHit(ray, (-b - sqrtDiscriminant) * invDenominator, axisDir, height, closestDist);
				UpdateClosestLateralHit(ray, (-b + sqrtDiscriminant) * invDenominator, axisDir, height, closestDist);
			}
		}
		else if (!math::IsEpsilonZero(b))
			UpdateClosestLateralHit(ray, -c / b, axisDir, height, closestDist);

		// Base cap:
		float denominator = Float3::Dot(ray.direction, axisDir);
		if (!math::IsEpsilonZero(denominator))
		{
			float dist = Float3::Dot(baseCenter - ray.origin, axisDir) / denominator;
			if (dist >= 0.0f && Float3::DistanceSq(ray.GetPoint(dist), baseCenter) <= radiusSq)
				closestDist = math::Min(closestDist, dist);
		}

		if (closestDist == math::maxValue)
			return std::nullopt;
		return ray.GetPoint(closestDist);
	}



	// Equality:
	bool Cone::operator==(const Cone& other) const
	{
		return apex == other.apex && baseCenter == other.baseCenter && radius == other.radius;
	}
	bool Cone::operator!=(const Cone& other) const
	{
		return !((*this) == other);
	}



	// Logging:
	std::string Cone::ToString() const
	{
		return "Cone(apex: " + apex.ToString() + ", baseCenter: " + baseCenter.ToString() + ", radius: " + std::to_string(radius) + ")";
	}
	std::ostream& operator<<(std::ostream& os, const Cone& cone)
	{
		os << cone.ToString();
		return os;
	}



	// Private methods:
	Float2 Cone::ClosestPointOnSegment(const Float2& point, const Float2& a, const Float2& b)
	{
		Float2 ab = b - a;
		float lengthSq = ab.LengthSq();
		float t = math::IsEpsilonZero(lengthSq) ? 0.0f : Float2::Dot(point - a, ab) / lengthSq;
		t = math::Clamp(t, 0.0f, 1.0f);
		return a + t * ab;
	}
	void Cone::UpdateClosestLateralHit(const Ray& ray, float dist, const Float3& axisDir, float height, float& closestDist) const
	{
		if (dist < 0.0f)
			return;
		float axialDist = Float3::Dot(ray.GetPoint(dist) - apex, axisDir);
		if (axialDist >= 0.0f && axialDist <= height)
			closestDist = math::Min(closestDist, dist);
	}
}