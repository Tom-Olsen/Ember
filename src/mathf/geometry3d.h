#ifndef __INCLUDE_GUARD_geometry3d_h__
#define __INCLUDE_GUARD_geometry3d_h__
#include <stdint.h>
#include "float3.h"



// Class/Struct independent functions:
namespace geometry3d
{
	float PointToPlaneDistance(const Float3& point, const Float3& planeNormal);
	Float3 PointToPlaneProjection(const Float3& point, const Float3& planeNormal);
	Float3 ReflectPointOnPlane(const Float3& point, const Float3& planeNormal);
	Float3 LinePlaneIntersection(const Float3& lineSupport, const Float3& lineDirection, const Float3& planeSupport, const Float3& planeNormal);
}



#endif // __INCLUDE_GUARD_geometry3d_h__