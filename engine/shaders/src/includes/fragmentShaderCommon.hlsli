#ifndef __INCLUDE_GUARD_fragmentShaderCommon_hlsli__
#define __INCLUDE_GUARD_fragmentShaderCommon_hlsli__
#include "globalSet.hlsli"
#include "frameSet.hlsli"
#include "sceneSet.hlsli"



// Including this into your shader has no negative side effects if you dont't use the resources defined here.
// The DXC hlsl compiler will optimize them out of your binary shader code.
// However, by using these pre defined resource you automatically follow the best practices in the readme.txt,
// which allows for ideal descriptorSet reusage across shader stages, which will increase performance.



#endif // __INCLUDE_GUARD_fragmentShaderCommon_hlsli__