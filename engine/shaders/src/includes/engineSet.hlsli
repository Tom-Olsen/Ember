#ifndef __INCLUDE_GUARD_engineSet_hlsli__
#define __INCLUDE_GUARD_engineSet_hlsli__
#include "descriptorSetMacros.h"



SamplerState colorSampler : register(s3000, ENGINE_SET);
SamplerComparisonState shadowSampler : register(s3001, ENGINE_SET);
Texture2DArray<float> shadowMaps : register(t3100, ENGINE_SET);



#endif // __INCLUDE_GUARD_engineSet_hlsli__