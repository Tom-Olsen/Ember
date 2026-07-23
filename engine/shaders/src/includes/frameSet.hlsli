#ifndef __INCLUDE_GUARD_frameSet_hlsli__
#define __INCLUDE_GUARD_frameSet_hlsli__
#include "descriptorSetMacros.h"



cbuffer Camera : register(b1399, FRAME_SET)
{
    float4 camera_position;             // camera position.
    float4x4 camera_viewMatrix;         // world to camera matrix.
    float4x4 camera_projMatrix;         // camera projection matrix (HDC => NDC after w division, which happens automatically).
    float4x4 camera_worldToClipMatrix;  // world to camera clip space matrix: (projection * view)
};



// Clip planes:
float GetCameraNearClip()
{
    return camera_projMatrix[2][3] / camera_projMatrix[2][2];
}
float GetCameraFarClip()
{
    return (camera_projMatrix[2][3] - camera_projMatrix[3][3]) / (camera_projMatrix[2][2] - camera_projMatrix[3][2]);
}

// Camera directions:
float3 GetCameraRight()
{
    return normalize(camera_viewMatrix[0].xyz);
}
float3 GetCameraForward()
{
    return -normalize(camera_viewMatrix[2].xyz);
}
float3 GetCameraUp()
{
    return normalize(camera_viewMatrix[1].xyz);
}



#endif // __INCLUDE_GUARD_frameSet_hlsli__