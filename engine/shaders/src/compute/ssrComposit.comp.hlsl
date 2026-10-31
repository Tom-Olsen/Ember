#define EMBER_SCENE_COLOR_ACCESS EMBER_SCENE_COLOR_ACCESS_IN_PLACE
#include "computeShaderCommon.hlsli"
#include "bilateralUpsampling.hlsli"
#include "deferredRenderingConstants.h"
#include "reflectionLighting.hlsli"



// Bindings:
[[vk::image_format("rgba16f")]] RWTexture2D<float4> reflectionMap : register(u200, CALL_SET);



// Bilateral upsampling parameters:
static const int upsampleRadius = 1;
static const float spatialWeightScale = 0.5f;
static const float minimumDepthTolerance = 0.05f;
static const float relativeDepthTolerance = 0.02f;
static const float minimumNormalWeightExponent = 8.0f;
static const float maximumNormalWeightExponent = 48.0f;
static const float roughnessWeightScale = 8.0f;



[numthreads(32, 32, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    if (threadID.x >= pc.threadCount.x || threadID.y >= pc.threadCount.y)
        return;

	// Screen size:
	uint screenWidth;
    uint screenHeight;
    sceneDepthTexture.GetDimensions(screenWidth, screenHeight);
	float2 screenSize = float2(screenWidth, screenHeight);

	// Source pixel/color:
	uint2 sourcePixel = threadID.xy;
	float4 sourceColor = Scene_GetColor(sourcePixel);

	// Get surface data:
	float3 worldPosition;
	float viewDepth;
	float3 worldNormal;
	float roughness;
	if (Bilateral_TryGetSurfaceData(sourcePixel, screenSize, worldPosition, viewDepth, worldNormal, roughness) == false)
		return;

	// Reflection map size:
	uint reflectionWidth;
	uint reflectionHeight;
	reflectionMap.GetDimensions(reflectionWidth, reflectionHeight);
	float2 reflectionSize = float2(reflectionWidth, reflectionHeight);

	// Map the screen pixel center to the reflection texture:
	float2 screenPosition = float2(sourcePixel) + 0.5f;
	float2 reflectionPosition = screenPosition * reflectionSize / screenSize;
	int2 nearestReflectionPixel = Bilateral_ScreenPixelToLowResolutionPixel(sourcePixel, screenSize, reflectionSize);

	// Loop over upsample neighbourhood:
	float3 weightedReflection = 0.0f;
	float totalWeight = 0.0f;
	float normalWeightExponent = lerp(maximumNormalWeightExponent, minimumNormalWeightExponent, saturate(roughness));
	float depthTolerance = max(minimumDepthTolerance, relativeDepthTolerance * viewDepth);
	for (int y = -upsampleRadius; y <= upsampleRadius; y++)
	{
		for (int x = -upsampleRadius; x <= upsampleRadius; x++)
		{
			// Skip out of image pixels:
			int2 samplePixel = nearestReflectionPixel + int2(x, y);
			if (samplePixel.x < 0 || samplePixel.x >= int(reflectionWidth) ||
				samplePixel.y < 0 || samplePixel.y >= int(reflectionHeight))
				continue;
			uint2 reflectionPixel = uint2(samplePixel);

			// Sky rays:
			float4 reflectionSample = reflectionMap[reflectionPixel];
			if (reflectionSample.a <= 0.0f)
				continue;

			// Recover the full-resolution screen pixel for this reflection texel:
			uint2 sampleScreenPixel = Bilateral_LowResolutionPixelToScreenPixel(reflectionPixel, uint2(screenWidth, screenHeight), reflectionSize);

			// Get surface data:
			float3 sampleWorldPosition;
			float sampleViewDepth;
			float3 sampleWorldNormal;
			float sampleRoughness;
			if (Bilateral_TryGetSurfaceData(sampleScreenPixel, screenSize, sampleWorldPosition, sampleViewDepth, sampleWorldNormal, sampleRoughness) == false)
				continue;

			// Weighting:
			float2 pixelOffset = float2(reflectionPixel) + 0.5f - reflectionPosition;
			float spatialWeight = Bilateral_SpatialWeight(pixelOffset, spatialWeightScale);
			float depthWeight = Bilateral_DepthWeight(viewDepth, sampleViewDepth, depthTolerance);
			float normalWeight = Bilateral_NormalWeight(worldNormal, sampleWorldNormal, normalWeightExponent);
			float roughnessWeight = Bilateral_RoughnessWeight(roughness, sampleRoughness, roughnessWeightScale);
			float weight = spatialWeight * depthWeight * normalWeight * roughnessWeight * saturate(reflectionSample.a);

			// Accumulate:
			weightedReflection += weight * reflectionSample.rgb;
			totalWeight += weight;
		}
	}

	// No reflections:
	if (totalWeight <= 1.0e-5f)
		return;

	// Reflection color:
	float3 reflectionColor = weightedReflection / totalWeight;
	float reconstructionConfidence = saturate(totalWeight);

	// Apply the material BRDF at full resolution and add indirect specular lighting:
	float3 albedo = gbufferAlbedoTexture.Load(int3(sourcePixel, 0)).rgb;
	float4 surfaceProperties = gbufferSurfacePropertiesSampleTexture.Load(int3(sourcePixel, 0));
	float3 cameraRayDirection = Camera_GetRayDirection(worldPosition);
	float nDotV = saturate(dot(worldNormal, -cameraRayDirection));
	float3 specularResponse = Reflection_GetSpecularResponse(albedo, surfaceProperties, nDotV);
	Scene_SetColor(sourcePixel, float4(sourceColor.rgb + reconstructionConfidence * reflectionColor * specularResponse, sourceColor.a));
}