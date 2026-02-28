#ifndef __INCLUDE_GUARD_globalSet_hlsli__
#define __INCLUDE_GUARD_globalSet_hlsli__
#include "descriptorSetMacros.h"



SamplerState colorSampler : register(s3000, GLOBAL_SET);
SamplerComparisonState shadowSampler : register(s3001, GLOBAL_SET);
Texture2DArray<float> shadowMaps : register(t3100, GLOBAL_SET);



#endif // __INCLUDE_GUARD_globalSet_hlsli__