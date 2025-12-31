#ifndef __INCLUDE_GUARD_descriptorSet_hlsli__
#define __INCLUDE_GUARD_descriptorSet_hlsli__



// In hlsl set = space.
#define DRAW_SET      space0    // e.g drawCall:  localToWorld, worldToLocal
#define FRAME_SET     space1    // e.g camera:    view, proj, viewProj, cameraPos
#define MATERIAL_SET  space2    // e.g material:  textures, buffers, instanceData, shaderParams (metallicity etc.)
#define SCENE_SET     space3    // e.g lighting:  dirLightCount, posLightsCount
#define ENGINE_SET    space4    // e.g engine:    samplers, shadowMaps, global noise textures, etc



#endif // __INCLUDE_GUARD_descriptorSet_hlsli__