#include "geometry3d.h"



namespace geometry3d
{
	float PointToPlaneDistance(const Float3& point, const Float3& planeNormal)
	{
		return Float3::Dot(point, planeNormal) / Float3::Dot(planeNormal, planeNormal);
	}
	Float3 PointToPlaneProjection(const Float3& point, const Float3& planeNormal)
	{
		return point - PointToPlaneDistance(point, planeNormal) * planeNormal;
	}
	Float3 ReflectPointOnPlane(const Float3& point, const Float3& planeNormal)
	{
		return point - 2.0f * PointToPlaneDistance(point, planeNormal) * planeNormal.Normalize();
	}
	Float3 LinePlaneIntersection(const Float3& lineSupport, const Float3& lineDirection, const Float3& planeSupport, const Float3& planeNormal)
	{
		float d = Float3::Dot(planeSupport, planeNormal);
		float t = (d - Float3::Dot(lineSupport, planeNormal)) / Float3::Dot(lineDirection, planeNormal);
		return lineSupport + t * lineDirection;
	}
}