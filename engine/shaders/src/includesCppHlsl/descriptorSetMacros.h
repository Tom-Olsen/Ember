#ifndef __INCLUDE_GUARD_descriptorSetMacros_h__
#define __INCLUDE_GUARD_descriptorSetMacros_h__



// Notes:
// - higher set index = higher update frequency
// - (dynamic): these descriptor sets are generate dynamically based on the shader code via reflection. See ShaderProperties.
// -  (static): these descriptor sets are always the same and get extracted from the default material once on startup.
// - DRAW_SET (4) will be removed in the future and replaced by storage buffer + indexing (push Constant) -> way fever descriptorSet rebinds.



//  HLSL spaces:
#define GLOBAL_SET	space0  // global:    samplers, shadowMaps, global noise textures, etc	(static)
#define SCENE_SET	space1  // lighting:  dirLightCount, posLightsCount						(static)
#define FRAME_SET	space2  // camera:    view, proj, viewProj, cameraPos					(static)
#define SHADER_SET	space3  // material:  textures, buffers, instanceData, metallicity etc.	(dynamic)
#define DRAW_SET	space4  // drawCall:  localToWorld, worldToLocal						(dynamic)



// Vulkan set indices:
#define GLOBAL_SET_INDEX		0
#define SCENE_SET_INDEX			1
#define FRAME_SET_INDEX			2
#define SHADER_SET_INDEX		3
#define DRAW_SET_INDEX			4
#define DESCRIPTOR_SET_COUNT	5



// Binding conventions:
// Global set:			s3000-3099, t3100-3199, u3200-3299, b3300-3399
// Scene set:			s2000-2099, t2100-2199, u2200-2299, b2300-2399
// Frame set:			s1000-1099, t1100-1199, u1200-1299, b1300-1399
// Material/Draw set:	   s0-  99,  t100- 199,  u200- 299,  b300- 399



#endif // __INCLUDE_GUARD_descriptorSetMacros_h__