#ifndef __INCLUDE_GUARD_globalSet_hlsli__
#define __INCLUDE_GUARD_globalSet_hlsli__
#include "descriptorSetMacros.h"



SamplerComparisonState shadowSampler : register(s3000, GLOBAL_SET);
SamplerState colorSampler : register(s3001, GLOBAL_SET);
SamplerState colorSamplerClampEdge : register(s3002, GLOBAL_SET);
Texture2DArray<float> shadowMaps : register(t3100, GLOBAL_SET);



#endif // __INCLUDE_GUARD_globalSet_hlsli__