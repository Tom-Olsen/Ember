#ifndef __INCLUDE_GUARD_descriptorSet_hlsli__
#define __INCLUDE_GUARD_descriptorSet_hlsli__



// Notes:
// - higher set index = higher update frequency
// - (dynamic): these descriptor sets are generate dynamically based on the shader code via reflection. See ShaderProperties.
// -  (static): these descriptor sets are always the same and get extracted from the default material once on startup.



//  HLSL spaces:
#define ENGINE_SET    space0  // e.g engine:    samplers, shadowMaps, global noise textures, etc	(static)
#define SCENE_SET     space1  // e.g lighting:  dirLightCount, posLightsCount						(static)
#define FRAME_SET     space2  // e.g camera:    view, proj, viewProj, cameraPos						(static)
#define MATERIAL_SET  space3  // e.g material:  textures, buffers, instanceData, metallicity etc.	(dynamic)
#define DRAW_SET      space4  // e.g drawCall:  localToWorld, worldToLocal							(dynamic)



// Vulkan set indices:
#define ENGINE_SET_INDEX    0
#define SCENE_SET_INDEX     1
#define FRAME_SET_INDEX     2
#define MATERIAL_SET_INDEX  3
#define DRAW_SET_INDEX      4



// Binding conventions:
// Engine set:			s3000-3099, t3100-3199, u3200-3299, b3300-3399
// Scene set:			s2000-2099, t2100-2199, u2200-2299, b2300-2399
// Frame set:			s1000-1099, t1100-1199, u1200-1299, b1300-1399
// Material/Draw set:	   s0-  99,  t100- 199,  u200- 299,  b300- 399



#endif // __INCLUDE_GUARD_descriptorSet_hlsli__