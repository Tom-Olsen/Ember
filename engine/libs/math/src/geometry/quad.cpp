#include "quad.h"
#include "mathFunctions.h"
#include "plane.h"
#include <sstream>



namespace emberMath
{
	// Public methods:
	// Constructors:
	Quad::Quad(): origin(Float3::zero), uAxis(Float3::right), vAxis(Float3::forward) {}
	Quad::Quad(const Float3& origin, const Float3& uAxis, const Float3& vAxis)
		: origin(origin), uAxis(uAxis), vAxis(vAxis) {}
	Quad::Quad(const Quad& quad)
		: origin(quad.origin), uAxis(quad.uAxis), vAxis(quad.vAxis) {}



	// Getters:
	Float3 Quad::GetCenter() const
	{
		return origin + 0.5f * (uAxis + vAxis);
	}
	Float3 Quad::GetNormal() const
	{
		Float3 normal = Float3::Cross(uAxis, vAxis);
		return normal.IsEpsilonZero() ? Float3::up : normal.Normalize();
	}



	// Methods:
	bool Quad::Contains(const Float3& point) const
	{
		return ClosestPoint(point).IsEpsilonEqual(point);
	}
	Float3 Quad::ClosestPoint(const Float3& point) const
	{
		Float3 rel = point - origin;
		float uLengthSq = uAxis.LengthSq();
		float vLengthSq = vAxis.LengthSq();
		float u = math::IsEpsilonZero(uLengthSq) ? 0.0f : math::Clamp(Float3::Dot(rel, uAxis) / uLengthSq, 0.0f, 1.0f);
		float v = math::IsEpsilonZero(vLengthSq) ? 0.0f : math::Clamp(Float3::Dot(rel, vAxis) / vLengthSq, 0.0f, 1.0f);
		return origin + u * uAxis + v * vAxis;
	}
	RayHit Quad::IntersectRay(const Ray& ray) const
	{
		RayHit hit = Plane(origin, GetNormal()).IntersectRay(ray);
		if (!hit.GetHit())
			return RayHit();

		Float3 rel = hit.GetPoint() - origin;
		float uLengthSq = uAxis.LengthSq();
		float vLengthSq = vAxis.LengthSq();
		if (math::IsEpsilonZero(uLengthSq) || math::IsEpsilonZero(vLengthSq))
			return RayHit();

		float u = Float3::Dot(rel, uAxis) / uLengthSq;
		float v = Float3::Dot(rel, vAxis) / vLengthSq;
		if (u < 0.0f || u > 1.0f || v < 0.0f || v > 1.0f)
			return RayHit();

		return hit;
	}



	// Equality:
	bool Quad::operator==(const Quad& other) const
	{
		return origin == other.origin && uAxis == other.uAxis && vAxis == other.vAxis;
	}
	bool Quad::operator!=(const Quad& other) const
	{
		return !((*this) == other);
	}


    
	// Logging:
	std::string Quad::ToString() const
	{
		return "Quad(origin: " + origin.ToString() + ", uAxis: " + uAxis.ToString() + ", vAxis: " + vAxis.ToString() + ")";
	}
	std::ostream& operator<<(std::ostream& os, const Quad& quad)
	{
		os << quad.ToString();
		return os;
	}
}