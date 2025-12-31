#ifndef __INCLUDE_GUARD_vertexShaderCommon_hlsli__
#define __INCLUDE_GUARD_vertexShaderCommon_hlsli__
#include "descriptorSets.hlsli"



// Including this into your shader has no negative side effects if you dont't use the resources defined here.
// The DXC hlsl compiler will optimize them out of your binary shader code.
// However, by using these pre defined resource you automatically follow the best practices in the readme.txt,
// which allows for ideal descriptorSet reusage across shader stages, which will increase performance.



// Per draw call resources:
cbuffer ModelMatrizes : register(b100, DRAW_SET)
{
    float4x4 model_localToWorldMatrix; // TRS matrix.
    float4x4 model_worldToLocalMatrix; // inverse TRS matrix.
};

// Per frame resources:
cbuffer Camera : register(b100, FRAME_SET)
{
    float4 camera_Position;             // camera position.
    float4x4 camera_viewMatrix;         // world to camera matrix.
    float4x4 camera_projMatrix;         // camera projection matrix (HDC => NDC after w division, which happens automatically).
    float4x4 camera_worldToClipMatrix;  // world to camera clip space matrix: (projection * view)
};

// Per material/shaderProperites resources:
struct InstanceData
{
    float4x4 localToWorldMatrix;    // per instance TRS matrix (relative to group transform).
    float4 color;                   // per instance color.
};
StructuredBuffer<InstanceData> instanceBuffer : register(t100, MATERIAL_SET);



// Macro to fake a constant buffer member as it depends on different sets:
#define cb_localToClipMatrix mul(cb_worldToClipMatrix, cb_localToWorldMatrix)



#endif // __INCLUDE_GUARD_vertexShaderCommon_hlsli__