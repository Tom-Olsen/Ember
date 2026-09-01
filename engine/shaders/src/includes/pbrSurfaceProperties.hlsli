#ifndef __INCLUDE_GUARD_pbrSurfaceProperties_hlsli__
#define __INCLUDE_GUARD_pbrSurfaceProperties_hlsli__
#include "descriptorSetMacros.h"



cbuffer SurfaceProperties : register(b300, SHADER_SET)
{
	// Defaults are set in vulkanDescriptorSetBinding.cpp
    float4 surface_diffuseColor;	// (1.0, 1.0, 1.0, 1.0)
    float4 surface_scaleOffset;		// .xy = 1.0 (scale), .zw = 0.0 (offset)
	int surface_isLit;				// true
    float surface_roughness;		// 0.5
    float surface_metallicity;		// 0.0
    float surface_ambientOcclusion;	// 1.0
};



#endif // __INCLUDE_GUARD_pbrSurfaceProperties_hlsli__