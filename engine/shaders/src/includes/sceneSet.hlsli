#ifndef __INCLUDE_GUARD_sceneSet_hlsli__
#define __INCLUDE_GUARD_sceneSet_hlsli__
#include "descriptorSetMacros.h"
#include "lightData.hlsli"
#include "shadowConstants.h"



cbuffer LightProperties : register(b2300, SCENE_SET)
{
    int light_dirCount;   // number of active directional lights in the scene.
    int light_posCount;   // number of active positional lights in the scene.
    DirectionalLightData light_directionData[MAX_DIR_LIGHTS];
    PositionalLightData light_positionData[MAX_POS_LIGHTS];
};



#endif // __INCLUDE_GUARD_sceneSet_hlsli__