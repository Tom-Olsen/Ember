#ifndef __INCLUDE_GUARD_emberMath_h__
#define __INCLUDE_GUARD_emberMath_h__
#include <stdint.h>



// Include all math headers:
// Basic types:
#include "int2.h"
#include "int3.h"
#include "uint3.h"
#include "float2.h"
#include "float3.h"
#include "float4.h"
#include "float2x2.h"
#include "float2x3.h"
#include "float3x2.h"
#include "float3x3.h"
#include "float4x4.h"

// Geometry:
#include "bounds.h"
#include "bounds2d.h"
#include "geometry3d.h"
#include "ray.h"

// Random:
#include "mathConstants.h"
#include "mathFunctions.h"
#include "emberRandom.h"



// Force emberMath namespace everywhere:
using namespace emberMath;



#endif // __INCLUDE_GUARD_emberMath_h__