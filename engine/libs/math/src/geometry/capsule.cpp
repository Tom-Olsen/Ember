#include "capsule.h"
#include "mathFunctions.h"
#include <cassert>
#include <sstream>



namespace emberMath
{
	// Public methods:
	// Constructors:
	Capsule::Capsule(): point0(Float3::zero), point1(Float3::up), radius(0.5f) {}
	Capsule::Capsule(const Float3& point0, const Float3& point1, float radius)
        : point0(point0), point1(point1), radius(radius)
	{
		assert(radius >= 0.0f);
	}
	Capsule::Capsule(const Capsule& capsule)
        : point0(capsule.point0), point1(capsule.point1), radius(capsule.radius) {}



	// Getters:
	Float3 Capsule::GetCenter() const
	{
		return 0.5f * (point0 + point1);
	}
	Float3 Capsule::GetAxis() const
	{
		Float3 direction = point1 - point0;
		return direction.IsEpsilonZero() ? Float3::up : direction.Normalize();
	}
	float Capsule::GetHeight() const
	{
		return Float3::Distance(point0, point1);
	}


    
	// Methods:
	bool Capsule::Contains(const Float3& point) const
	{
		Float3 axis = point1 - point0;
		float heightSq = axis.LengthSq();
		if (math::IsEpsilonZero(heightSq))
			return Float3::DistanceSq(point, point0) <= radius * radius;

		float t = Float3::Dot(point - point0, axis) / heightSq;
		t = math::Clamp(t, 0.0f, 1.0f);
		Float3 segmentPoint = point0 + t * axis;
		return Float3::DistanceSq(point, segmentPoint) <= radius * radius;
	}
	Float3 Capsule::ClosestPoint(const Float3& point) const
	{
		Float3 axis = point1 - point0;
		float heightSq = axis.LengthSq();
		Float3 segmentPoint = point0;
		if (math::IsEpsilonZero(heightSq))
		{
			segmentPoint = point0;
		}
		else
		{
			float t = Float3::Dot(point - point0, axis) / heightSq;
			t = math::Clamp(t, 0.0f, 1.0f);
			segmentPoint = point0 + t * axis;
		}

		Float3 segmentToPoint = point - segmentPoint;
		float distSq = segmentToPoint.LengthSq();
		if (distSq <= radius * radius)
			return point;
		return segmentPoint + radius * segmentToPoint / math::Sqrt(distSq);
	}
	std::optional<Float3> Capsule::IntersectRay(const Ray& ray) const
	{
		float radiusSq = radius * radius;
		Float3 axis = point1 - point0;
		float height = axis.Length();
		Float3 axisDir = math::IsEpsilonZero(height) ? Float3::up : axis / height;
		float closestDist = math::maxValue;

		// Ray-sphere candidates:
		auto updateSphereHit = [&](const Float3& center, bool isPoint0)
		{
			Float3 originToCenter = ray.origin - center;
			float b = 2.0f * Float3::Dot(originToCenter, ray.direction);
			float c = originToCenter.LengthSq() - radiusSq;
			float discriminant = b * b - 4.0f * c;
			if (discriminant < 0.0f)
				return;

			float sqrtDiscriminant = math::Sqrt(discriminant);
			float dist0 = 0.5f * (-b - sqrtDiscriminant);
			float dist1 = 0.5f * (-b + sqrtDiscriminant);
			if (dist0 >= 0.0f)
			{
				float segmentDist = Float3::Dot(ray.GetPoint(dist0) - point0, axisDir);
				if (math::IsEpsilonZero(height) || (isPoint0 && segmentDist <= 0.0f) || (!isPoint0 && segmentDist >= height))
					closestDist = math::Min(closestDist, dist0);
			}
			else if (dist1 >= 0.0f)
			{
				float segmentDist = Float3::Dot(ray.GetPoint(dist1) - point0, axisDir);
				if (math::IsEpsilonZero(height) || (isPoint0 && segmentDist <= 0.0f) || (!isPoint0 && segmentDist >= height))
					closestDist = math::Min(closestDist, dist1);
			}
		};

		updateSphereHit(point0, true);
		updateSphereHit(point1, false);

		if (!math::IsEpsilonZero(height))
		{
			Float3 originToPoint0 = ray.origin - point0;
			float originAxisDist = Float3::Dot(originToPoint0, axisDir);
			float rayAxisDot = Float3::Dot(ray.direction, axisDir);
			Float3 originPerpendicular = originToPoint0 - originAxisDist * axisDir;
			Float3 rayPerpendicular = ray.direction - rayAxisDot * axisDir;

			float a = rayPerpendicular.LengthSq();
			float b = 2.0f * Float3::Dot(originPerpendicular, rayPerpendicular);
			float c = originPerpendicular.LengthSq() - radiusSq;
			float discriminant = b * b - 4.0f * a * c;
			if (!math::IsEpsilonZero(a) && discriminant >= 0.0f)
			{
				float sqrtDiscriminant = math::Sqrt(discriminant);
				float invDenominator = 0.5f / a;
				float dist0 = (-b - sqrtDiscriminant) * invDenominator;
				float dist1 = (-b + sqrtDiscriminant) * invDenominator;

				if (dist0 >= 0.0f)
				{
					float segmentDist = originAxisDist + dist0 * rayAxisDot;
					if (segmentDist >= 0.0f && segmentDist <= height)
						closestDist = math::Min(closestDist, dist0);
				}
				if (dist1 >= 0.0f)
				{
					float segmentDist = originAxisDist + dist1 * rayAxisDot;
					if (segmentDist >= 0.0f && segmentDist <= height)
						closestDist = math::Min(closestDist, dist1);
				}
			}
		}

		if (closestDist == math::maxValue)
			return std::nullopt;
		return ray.GetPoint(closestDist);
	}



	// Equality:
	bool Capsule::operator==(const Capsule& other) const
	{
		return point0 == other.point0 && point1 == other.point1 && radius == other.radius;
	}
	bool Capsule::operator!=(const Capsule& other) const
	{
		return !((*this) == other);
	}


    
	// Logging:
	std::string Capsule::ToString() const
	{
		return "Capsule(point0: " + point0.ToString() + ", point1: " + point1.ToString() + ", radius: " + std::to_string(radius) + ")";
	}
	std::ostream& operator<<(std::ostream& os, const Capsule& capsule)
	{
		os << capsule.ToString();
		return os;
	}
}