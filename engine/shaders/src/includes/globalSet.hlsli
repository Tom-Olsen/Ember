#ifndef __INCLUDE_GUARD_globalSet_hlsli__
#define __INCLUDE_GUARD_globalSet_hlsli__
#include "descriptorSetMacros.h"



SamplerComparisonState shadowSampler : register(s3099, GLOBAL_SET);
SamplerState colorSampler : register(s3098, GLOBAL_SET);
SamplerState colorSamplerClampEdge : register(s3097, GLOBAL_SET);
Texture2DArray<float> shadowMaps : register(t3199, GLOBAL_SET);



#endif // __INCLUDE_GUARD_globalSet_hlsli__