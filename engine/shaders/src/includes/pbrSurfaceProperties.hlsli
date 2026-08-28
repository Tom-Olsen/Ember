#ifndef __INCLUDE_GUARD_pbrSurfaceProperties_hlsli__
#define __INCLUDE_GUARD_pbrSurfaceProperties_hlsli__
#include "descriptorSetMacros.h"



cbuffer SurfaceProperties : register(b300, CALL_SET)
{
    float4 diffuseColor;       // (1.0, 1.0, 1.0, 1.0)
    float4 scaleOffset;        // .xy = scale, .zw = offset
    float roughness;           // 0.5
    float metallicity;         // 0 = dielectric, 1 = metal
    float ambientOcclusion;    // 1 = no occlusion
};



#endif // __INCLUDE_GUARD_pbrSurfaceProperties_hlsli__