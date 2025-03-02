#ifndef __INCLUDE_GUARD_geometry3d_h__
#define __INCLUDE_GUARD_geometry3d_h__
#include "float3.h"
#include <stdint.h>



namespace emberMath
{
	// 3D geometry functions:
	namespace geometry3d
	{
		Float3 GetOrhtogonalVector(const Float3& v);
		bool IsInsidePlane(const Float3& point, const Float3& planeSupport, const Float3& planeNormal);
		float PointToPlaneDistance(const Float3& point, const Float3& planeSupport, const Float3& planeNormal);
		Float3 PointToPlaneProjection(const Float3& point, const Float3& planeSupport, const Float3& planeNormal);
		Float3 ReflectPointOnPlane(const Float3& point, const Float3& planeSupport, const Float3& planeNormal);
		Float3 LinePlaneIntersection(const Float3& lineSupport, const Float3& lineDirection, const Float3& planeSupport, const Float3& planeNormal);
	}
}



#endif // __INCLUDE_GUARD_geometry3d_h__