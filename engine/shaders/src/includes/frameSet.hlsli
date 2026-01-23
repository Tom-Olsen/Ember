#ifndef __INCLUDE_GUARD_frameSet_hlsli__
#define __INCLUDE_GUARD_frameSet_hlsli__
#include "descriptorSetMacros.h"



cbuffer Camera : register(b1300, FRAME_SET)
{
    float4 camera_Position;             // camera position.
    float4x4 camera_viewMatrix;         // world to camera matrix.
    float4x4 camera_projMatrix;         // camera projection matrix (HDC => NDC after w division, which happens automatically).
    float4x4 camera_worldToClipMatrix;  // world to camera clip space matrix: (projection * view)
};



#endif // __INCLUDE_GUARD_frameSet_hlsli__