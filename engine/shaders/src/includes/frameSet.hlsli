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



float GetCameraNearClip()
{
    return camera_projMatrix[2][3] / camera_projMatrix[2][2];
}



#endif // __INCLUDE_GUARD_frameSet_hlsli__