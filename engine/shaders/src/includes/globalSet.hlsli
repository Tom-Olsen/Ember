#ifndef __INCLUDE_GUARD_globalSet_hlsli__
#define __INCLUDE_GUARD_globalSet_hlsli__
#include "descriptorSetMacros.h"



// Depth comparison (reference <= sampled depth), linear filtering and linear mip interpolation.
// Clamps U/V/W to a zero-depth border; anisotropic filtering is disabled.
SamplerComparisonState shadowSampler : register(s3000, GLOBAL_SET);

// Linear filtering, linear mip interpolation and anisotropic filtering.
// Repeats the texture for U/V/W coordinates outside [0, 1].
SamplerState colorSampler : register(s3001, GLOBAL_SET);

// Same filtering as colorSampler; clamps U/V/W to the nearest edge texels.
SamplerState colorSamplerClampEdge : register(s3002, GLOBAL_SET);

// Same filtering as colorSampler; uses a floating-point zero border (RGBA = 0).
// Filtering blends edge texels with the border, so samples just outside [0, 1] can be nonzero.
SamplerState colorSamplerClampBorder : register(s3003, GLOBAL_SET);

// Linear filtering and linear mip interpolation, with anisotropic filtering disabled.
// Uses a floating-point zero border; at mip 0, edge density fades to zero half a texel outside the texture.
SamplerState colorSamplerClampBorderNoAnisotropy : register(s3004, GLOBAL_SET);

Texture2DArray<float> shadowMaps : register(t3100, GLOBAL_SET);



#endif // __INCLUDE_GUARD_globalSet_hlsli__