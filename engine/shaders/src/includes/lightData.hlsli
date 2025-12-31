#ifndef __INCLUDE_GUARD_lightData_hlsli__
#define __INCLUDE_GUARD_lightData_hlsli__
#include "shadowConstants.h"



#define SHADOW_MAP_TEXEL_SIZE float2(1.0f / SHADOW_MAP_RESOLUTION, 1.0f / SHADOW_MAP_RESOLUTION)



// One for each shadow cascade:
struct DirectionalLightData
{
    float4x4 worldToClipMatrix; // world to light clip space matrix (projection * view).
    float3 direction;           // light direction.
    int shadowType;             // 0 = PCF shadows, 1 = pixel shadows.
    float4 colorIntensity;      // light color (xyz) and intensity (w).
};
// One for each spot light, 6 for each point light:
struct PositionalLightData
{
    float4x4 worldToClipMatrix; // world to light clip space matrix (projection * view).
    float3 position;            // light position.
    int shadowType;             // 0 = PCF shadows, 1 = pixel shadows.
    float4 colorIntensity;      // light color (xyz) and intensity (w).
    float2 blendStartEnd;       // start and end of fallOff in radial direction €[0,1].
};



#endif // __INCLUDE_GUARD_lightData_hlsli__