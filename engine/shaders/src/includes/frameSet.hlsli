#ifndef __INCLUDE_GUARD_frameSet_hlsli__
#define __INCLUDE_GUARD_frameSet_hlsli__
#include "descriptorSetMacros.h"
#include "mathRay.hlsli"



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



// Camera depth:
float Camera_GetDepth(float3 worldPosition)
{// view space depth: [nearClip,farClip] can exceed bounds if worldPosition outside camera near/far plane.
    return -mul(camera_viewMatrix, float4(worldPosition, 1.0f)).z;
}



// Camera world position:
float3 Camera_GetWorldPosition(float3 ndcPosition)
{// ndc: xy in [-1, 1], z in [0, 1].
    float4 worldPosition = mul(camera_clipToWorldMatrix, float4(ndcPosition, 1.0f));
    return worldPosition.xyz / worldPosition.w;
}

float3 Camera_GetWorldPosition(float2 uv, float ndcDepth)
{// uv in [0, 1], ndcDepth in [0, 1].
    return Camera_GetWorldPosition(float3(2.0f * uv - 1.0f, ndcDepth));
}
float3 Camera_GetWorldPosition(float2 pixelPosition, float ndcDepth, float2 screenSize)
{// continuous position in pixel units; pixel centers already include the 0.5 offset.
    return Camera_GetWorldPosition(pixelPosition / screenSize, ndcDepth);
}
float3 Camera_GetWorldPosition(uint2 pixel, float ndcDepth, float2 screenSize)
{// integer pixel index; reconstruct the position at the pixel center.
    return Camera_GetWorldPosition(float2(pixel) + 0.5f, ndcDepth, screenSize);
}



// Camera ray:
float3 Camera_GetRayDirection(float3 worldPosition)
{
    bool isPerspective = abs(camera_projMatrix[3][3]) < 0.5f;
    return isPerspective ? normalize(worldPosition - camera_position.xyz) : Camera_GetForward();
}
math_Ray Camera_GetRay(float2 uv)
{// uv in [0, 1]; the ray starts at the near plane.
    math_Ray worldRay;
    worldRay.origin = Camera_GetWorldPosition(uv, 0.0f);
    worldRay.direction = Camera_GetRayDirection(worldRay.origin);
    return worldRay;
}
math_Ray Camera_GetRay(float2 pixelPosition, float2 screenSize)
{// continuous position in pixel units; pixel centers already include the 0.5 offset.
    return Camera_GetRay(pixelPosition / screenSize);
}
math_Ray Camera_GetRay(uint2 pixel, float2 screenSize)
{// integer pixel index; construct the ray through the pixel center.
    return Camera_GetRay(float2(pixel) + 0.5f, screenSize);
}



#endif // __INCLUDE_GUARD_frameSet_hlsli__