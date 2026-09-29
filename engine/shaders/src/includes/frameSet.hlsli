#ifndef __INCLUDE_GUARD_frameSet_hlsli__
#define __INCLUDE_GUARD_frameSet_hlsli__
#include "descriptorSetMacros.h"



// Render targets (for compute shaders only):
#ifdef EMBER_ENABLE_FRAME_SET_RENDER_TARGETS
Texture2D<float> sceneDepthTexture : register(t1100, FRAME_SET);
Texture2D<float4> gbufferNormalTexture : register(t1101, FRAME_SET);
Texture2D<float4> gbufferAlbedoTexture : register(t1102, FRAME_SET);
Texture2D<float4> gbufferSurfacePropertiesTexture : register(t1103, FRAME_SET);
Texture2D<float4> sceneColorSampleTexture0 : register(t1104, FRAME_SET);
Texture2D<float4> sceneColorSampleTexture1 : register(t1105, FRAME_SET);
[[vk::image_format("rgba16f")]] RWTexture2D<float4> sceneColorTexture0 : register(u1200, FRAME_SET);
[[vk::image_format("rgba16f")]] RWTexture2D<float4> sceneColorTexture1 : register(u1201, FRAME_SET);
#endif



// Camera:
cbuffer CameraProperties : register(b1300, FRAME_SET)
{
    float4 camera_position;             // camera position.
    float4x4 camera_viewMatrix;         // world to camera matrix.
    float4x4 camera_projMatrix;         // camera projection matrix (HDC => NDC after w division, which happens automatically).
    float4x4 camera_worldToClipMatrix;  // world to camera clip space matrix: (projection * view)
    float4x4 camera_clipToWorldMatrix;  // camera clip space to world matrix: inverse(projection * view)
};



// Clip planes:
float Camera_GetNearClip()
{
    return camera_projMatrix[2][3] / camera_projMatrix[2][2];
}
float Camera_GetFarClip()
{
    return (camera_projMatrix[2][3] - camera_projMatrix[3][3]) / (camera_projMatrix[2][2] - camera_projMatrix[3][2]);
}



// Camera directions:
float3 Camera_GetRight()
{
    return normalize(camera_viewMatrix[0].xyz);
}
float3 Camera_GetForward()
{
    return -normalize(camera_viewMatrix[2].xyz);
}
float3 Camera_GetUp()
{
    return normalize(camera_viewMatrix[1].xyz);
}



// Camera worldPosition data:
float3 Camera_GetRayDirection(float3 worldPosition)
{
    bool isPerspective = abs(camera_projMatrix[3][3]) < 0.5f;
    return isPerspective ? normalize(worldPosition - camera_position.xyz) : Camera_GetForward();
}
float Camera_GetDepth(float3 worldPosition)
{ // view space depth: [nearClip,farClip] can exceed bounds if worldPosition outside camera near/far plane.
    return -mul(camera_viewMatrix, float4(worldPosition, 1.0f)).z;
}


#endif // __INCLUDE_GUARD_frameSet_hlsli__