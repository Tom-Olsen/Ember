#ifndef __INCLUDE_GUARD_fragmentShaderCommon_hlsli__
#define __INCLUDE_GUARD_fragmentShaderCommon_hlsli__
#include "descriptorSets.hlsli"
#include "lightData.hlsli"
#include "shadowConstants.h"



// Including this into your shader has no negative side effects if you dont't use the resources defined here.
// The DXC hlsl compiler will optimize them out of your binary shader code.
// However, by using these pre defined resource you automatically follow the best practices in the readme.txt,
// which allows for ideal descriptorSet reusage across shader stages, which will increase performance.



// Per frame resources:
cbuffer Camera : register(b100, FRAME_SET)
{
    float4 camera_Position;             // camera position.
    float4x4 camera_viewMatrix;         // world to camera matrix.
    float4x4 camera_projMatrix;         // camera projection matrix (HDC => NDC after w division, which happens automatically).
    float4x4 camera_worldToClipMatrix;  // world to camera clip space matrix: (projection * view)
};

// Per scene resources:
cbuffer LightProperties : register(b100, SCENE_SET)
{
    int light_dirCount;   // number of active directional lights in the scene.
    int light_posCount;   // number of active positional lights in the scene.
    DirectionalLightData light_directionData[MAX_DIR_LIGHTS];
    PositionalLightData light_positionData[MAX_POS_LIGHTS];
};

// Engie resources:
SamplerState colorSampler : register(s100, ENGINE_SET);
SamplerComparisonState shadowSampler : register(s101, ENGINE_SET);
Texture2DArray<float> shadowMaps : register(t200, ENGINE_SET);



#endif // __INCLUDE_GUARD_fragmentShaderCommon_hlsli__