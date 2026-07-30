#include "triangle.h"
#include "mathFunctions.h"
#include <sstream>



namespace emberMath
{
	// Public methods:
	// Constructors:
	Triangle::Triangle(): a(Float3::zero), b(Float3::right), c(Float3::up) {}
	Triangle::Triangle(const Float3& a, const Float3& b, const Float3& c)
		: a(a), b(b), c(c) {}
	Triangle::Triangle(const Triangle& triangle)
		: a(triangle.a), b(triangle.b), c(triangle.c) {}



	// Getters:
	Float3 Triangle::GetCenter() const
	{
		return (a + b + c) / 3.0f;
	}
	Float3 Triangle::GetNormal() const
	{
		Float3 normal = Float3::Cross(b - a, c - a);
		return normal.IsEpsilonZero() ? Float3::up : normal.Normalize();
	}



	// Methods:
	bool Triangle::Contains(const Float3& point) const
	{
		return ClosestPoint(point).IsEpsilonEqual(point);
	}
	Float3 Triangle::ClosestPoint(const Float3& point) const
	{
		Float3 ab = b - a;
		Float3 ac = c - a;
		Float3 normal = Float3::Cross(ab, ac);
		if (normal.IsEpsilonZero())
		{
			Float3 closestAb = ClosestPointOnSegment(point, a, b);
			Float3 closestAc = ClosestPointOnSegment(point, a, c);
			Float3 closestBc = ClosestPointOnSegment(point, b, c);
			float distanceAb = Float3::DistanceSq(point, closestAb);
			float distanceAc = Float3::DistanceSq(point, closestAc);
			float distanceBc = Float3::DistanceSq(point, closestBc);
			if (distanceAb <= distanceAc && distanceAb <= distanceBc)
				return closestAb;
			return distanceAc <= distanceBc ? closestAc : closestBc;
		}

		Float3 ap = point - a;
		float d1 = Float3::Dot(ab, ap);
		float d2 = Float3::Dot(ac, ap);
		if (d1 <= 0.0f && d2 <= 0.0f)
			return a;

		Float3 bp = point - b;
		float d3 = Float3::Dot(ab, bp);
		float d4 = Float3::Dot(ac, bp);
		if (d3 >= 0.0f && d4 <= d3)
			return b;

		float vc = d1 * d4 - d3 * d2;
		if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f)
			return a + (d1 / (d1 - d3)) * ab;

		Float3 cp = point - c;
		float d5 = Float3::Dot(ab, cp);
		float d6 = Float3::Dot(ac, cp);
		if (d6 >= 0.0f && d5 <= d6)
			return c;

		float vb = d5 * d2 - d1 * d6;
		if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f)
			return a + (d2 / (d2 - d6)) * ac;

		float va = d3 * d6 - d5 * d4;
		if (va <= 0.0f && d4 - d3 >= 0.0f && d5 - d6 >= 0.0f)
		{
			Float3 bc = c - b;
			return b + ((d4 - d3) / ((d4 - d3) + (d5 - d6))) * bc;
		}

		float inverseDenominator = 1.0f / (va + vb + vc);
		float v = vb * inverseDenominator;
		float w = vc * inverseDenominator;
		return a + v * ab + w * ac;
	}
	RayTriangleHit Triangle::IntersectRay(const Ray& ray) const
	{
		Float3 edge1 = b - a;
		Float3 edge2 = c - a;
		Float3 p = Float3::Cross(ray.direction, edge2);
		float determinant = Float3::Dot(edge1, p);
		if (math::IsEpsilonZero(determinant))
			return RayTriangleHit();

		float inverseDeterminant = 1.0f / determinant;
		Float3 originOffset = ray.origin - a;
		float u = Float3::Dot(originOffset, p) * inverseDeterminant;
		if (u < 0.0f || u > 1.0f)
			return RayTriangleHit();

		Float3 q = Float3::Cross(originOffset, edge1);
		float v = Float3::Dot(ray.direction, q) * inverseDeterminant;
		if (v < 0.0f || u + v > 1.0f)
			return RayTriangleHit();

		float distance = Float3::Dot(edge2, q) * inverseDeterminant;
		if (distance < 0.0f)
			return RayTriangleHit();

		Float3 point = ray.GetPoint(distance);
		Float3 barycentricCoordinates(1.0f - u - v, u, v);
		return RayTriangleHit(distance, point, GetNormal(), barycentricCoordinates);
	}



	// Equality:
	bool Triangle::operator==(const Triangle& other) const
	{
		return a == other.a && b == other.b && c == other.c;
	}
	bool Triangle::operator!=(const Triangle& other) const
	{
		return !((*this) == other);
	}



	// Logging:
	std::string Triangle::ToString() const
	{
		return "Triangle(a: " + a.ToString() + ", b: " + b.ToString() + ", c: " + c.ToString() + ")";
	}
	std::ostream& operator<<(std::ostream& os, const Triangle& triangle)
	{
		os << triangle.ToString();
		return os;
	}



	// Private methods:
	Float3 Triangle::ClosestPointOnSegment(const Float3& point, const Float3& a, const Float3& b)
	{
		Float3 ab = b - a;
		float lengthSq = ab.LengthSq();
		float t = math::IsEpsilonZero(lengthSq) ? 0.0f : Float3::Dot(point - a, ab) / lengthSq;
		return a + math::Clamp(t, 0.0f, 1.0f) * ab;
	}
}