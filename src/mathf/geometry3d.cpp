#include "geometry3d.h"
#include "mathf.h"



namespace geometry3d
{
	Float3 GetOrhtogonalVector(const Float3& v)
	{
		Float3 result = Float3::Cross(v, Float3(1.0f, 0.0f, 0.0f));
		if (result.Length() < mathf::EPSILON)
		{
			result = Float3::Cross(v, Float3(0.0f, 1.0f, 0.0f));
			if (result.Length() < mathf::EPSILON)
				result = Float3::Cross(v, Float3(0.0f, 0.0f, 1.0f));
		}
		return result;
	}
	bool IsInsidePlane(const Float3& point, const Float3& planeSupport, const Float3& planeNormal)
	{
		float d = Float3::Dot(planeSupport, planeNormal); // plane parameter
		return mathf::Abs(Float3::Dot(point, planeNormal) - d) < mathf::EPSILON;
	}
	float PointToPlaneDistance(const Float3& point, const Float3& planeSupport, const Float3& planeNormal)
	{
		Float3 normal = planeNormal.Normalize();
		float d = Float3::Dot(planeSupport, normal); // plane parameter
		return (Float3::Dot(point, normal) - d) / Float3::Dot(normal, normal);
	}
	Float3 PointToPlaneProjection(const Float3& point, const Float3& planeSupport, const Float3& planeNormal)
	{
		return point - PointToPlaneDistance(point, planeSupport, planeNormal) * planeNormal.Normalize();
	}
	Float3 ReflectPointOnPlane(const Float3& point, const Float3& planeSupport, const Float3& planeNormal)
	{
		return point - 2.0f * PointToPlaneDistance(point, planeSupport, planeNormal) * planeNormal.Normalize();
	}
	Float3 LinePlaneIntersection(const Float3& lineSupport, const Float3& lineDirection, const Float3& planeSupport, const Float3& planeNormal)
	{
		float d = Float3::Dot(planeSupport, planeNormal);
		float t = (d - Float3::Dot(lineSupport, planeNormal)) / Float3::Dot(lineDirection, planeNormal);
		return lineSupport + t * lineDirection;
	}
}