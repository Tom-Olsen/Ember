#pragma once
#include "float3.h"
#include <optional>
#include <stdint.h>



namespace emberMath
{
	// 3D geometry functions:
	namespace geometry3d
	{
		std::optional<Float3> GetOrhtogonalVector(const Float3& v);
		std::optional<bool> IsInsidePlane(const Float3& point, const Float3& planeSupport, const Float3& planeNormal);
		std::optional<float> PointToPlaneDistance(const Float3& point, const Float3& planeSupport, const Float3& planeNormal);
		std::optional<Float3> PointToPlaneProjection(const Float3& point, const Float3& planeSupport, const Float3& planeNormal);
		std::optional<Float3> ReflectPointOnPlane(const Float3& point, const Float3& planeSupport, const Float3& planeNormal);
		std::optional<Float3> LinePlaneIntersection(const Float3& lineSupport, const Float3& lineDirection, const Float3& planeSupport, const Float3& planeNormal);
	}
}