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



// Bilinear sampling:
// Texture texels use representative screen pixel centers; sampleUv is normalized to the screen.
void Bilateral_GetBilinearSamplingData(float2 sampleUv, uint2 screenSize, uint2 textureSize, out int2 lowerTexturePixel, out int2 upperTexturePixel, out float2 interpolationFraction)
{
	// Use actual screen pixel centers to preserve alignment at arbitrary resolutions.
	int2 anchorTexturePixel = min(int2(sampleUv * float2(textureSize)), int2(textureSize) - 1);
	uint2 anchorScreenPixel = Bilateral_LowResolutionPixelToScreenPixel(uint2(anchorTexturePixel), screenSize, float2(textureSize));
	float2 sampleScreenPosition = sampleUv * float2(screenSize);
	lowerTexturePixel = anchorTexturePixel - int2(sampleScreenPosition.x < float(anchorScreenPixel.x) + 0.5f ? 1 : 0, sampleScreenPosition.y < float(anchorScreenPixel.y) + 0.5f ? 1 : 0);
	lowerTexturePixel = clamp(lowerTexturePixel, int2(0, 0), max(int2(textureSize) - 2, int2(0, 0)));
	upperTexturePixel = min(lowerTexturePixel + 1, int2(textureSize) - 1);
	uint2 lowerScreenPixel = Bilateral_LowResolutionPixelToScreenPixel(uint2(lowerTexturePixel), screenSize, float2(textureSize));
	uint2 upperScreenPixel = Bilateral_LowResolutionPixelToScreenPixel(uint2(upperTexturePixel), screenSize, float2(textureSize));
	interpolationFraction = saturate((sampleScreenPosition - (float2(lowerScreenPixel) + 0.5f)) / max(float2(upperScreenPixel - lowerScreenPixel), 1.0f));
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