#ifndef __INCLUDE_GUARD_bilateralUpsampling_hlsli__
#define __INCLUDE_GUARD_bilateralUpsampling_hlsli__
#include "deferredRenderingConstants.h"
#include "frameSet.hlsli"



// Resolution mapping:
// Map pixel centers between resolutions and select the containing destination pixel.
int2 Bilateral_ScreenPixelToLowResolutionPixel(uint2 sourcePixel, float2 screenSize, float2 lowResolutionSize)
{
	return int2(floor((float2(sourcePixel) + 0.5f) * lowResolutionSize / screenSize));
}
uint2 Bilateral_LowResolutionPixelToScreenPixel(uint2 lowResolutionPixel, uint2 screenSize, float2 lowResolutionSize)
{
	float2 screenPosition = (float2(lowResolutionPixel) + 0.5f) * float2(screenSize) / lowResolutionSize;
	return min(uint2(screenPosition), screenSize - 1);
}



// Surface data:
bool Bilateral_TryGetSurfaceData(uint2 pixel, float2 screenSize, out float3 worldPosition, out float viewDepth, out float3 worldNormal)
{
	float ndcDepth = sceneDepthTexture.Load(int3(pixel, 0));
	if (ndcDepth >= 1.0f)
	{
		worldPosition = 0.0f;
		viewDepth = 0.0f;
		worldNormal = 0.0f;
		return false;
	}

	worldPosition = Camera_GetWorldPosition(pixel, ndcDepth, screenSize);
	viewDepth = Camera_GetDepth(worldPosition);
	worldNormal = normalize(2.0f * gbufferNormalTexture.Load(int3(pixel, 0)).xyz - 1.0f);
	return true;
}
bool Bilateral_TryGetSurfaceData(uint2 pixel, float2 screenSize, out float3 worldPosition, out float viewDepth, out float3 worldNormal, out float roughness)
{
	roughness = 0.0f;
	if (!Bilateral_TryGetSurfaceData(pixel, screenSize, worldPosition, viewDepth, worldNormal))
		return false;
	roughness = gbufferSurfacePropertiesSampleTexture.Load(int3(pixel, 0))[DEFERRED_SURFACE_PROPERTIES_ROUGHNESS_CHANNEL];
	return true;
}



// Geometry-aware weighting:
float Bilateral_SpatialWeight(float2 pixelOffset, float spatialWeightScale)
{
	return exp(-spatialWeightScale * dot(pixelOffset, pixelOffset));
}
float Bilateral_DepthWeight(float viewDepth, float sampleViewDepth, float depthTolerance)
{
	float normalizedDepthDifference = abs(sampleViewDepth - viewDepth) / depthTolerance;
	return exp(-normalizedDepthDifference * normalizedDepthDifference);
}
float Bilateral_NormalWeight(float3 worldNormal, float3 sampleWorldNormal, float normalWeightExponent)
{
	return pow(saturate(dot(worldNormal, sampleWorldNormal)), normalWeightExponent);
}
float Bilateral_RoughnessWeight(float roughness, float sampleRoughness, float roughnessWeightScale)
{
	return exp(-roughnessWeightScale * abs(sampleRoughness - roughness));
}



#endif // __INCLUDE_GUARD_bilateralUpsampling_hlsli__