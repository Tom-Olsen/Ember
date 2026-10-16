#define EMBER_SCENE_COLOR_ACCESS EMBER_SCENE_COLOR_ACCESS_OUT_OF_PLACE
#include "computeShaderCommon.hlsli"
#include "deferredRenderingConstants.h"
#include "reflectionLighting.hlsli"



// Bindings:
TextureCube<float4> environmentMap : register(t100, CALL_SET);
[[vk::image_format("rgba16")]] RWTexture2D<float4> reflectionUvMap : register(u200, CALL_SET);



// Bilateral upsampling parameters:
static const int upsampleRadius = 1;
static const float spatialWeightScale = 0.5f;
static const float minimumDepthTolerance = 0.05f;
static const float relativeDepthTolerance = 0.02f;
static const float minimumNormalWeightExponent = 8.0f;
static const float maximumNormalWeightExponent = 48.0f;
static const float roughnessWeightScale = 8.0f;



// Helpers:
float3 ScreenPositionToWorld(float3 screenPosition, float2 screenSize)
{
	float2 uv = screenPosition.xy / screenSize;
	float4 clipPosition = float4(2.0f * uv - 1.0f, screenPosition.z, 1.0f);
	float4 worldPosition = mul(camera_clipToWorldMatrix, clipPosition);
	return worldPosition.xyz / worldPosition.w;
}
bool TryGetSurfaceData(uint2 pixel, float2 screenSize, out float3 worldPosition, out float viewDepth, out float3 worldNormal, out float roughness)
{
	float ndcDepth = Scene_GetNdcDepth(pixel);

	// No geometry:
	if (ndcDepth >= 1.0f)
	{
		worldPosition = 0.0f;
		viewDepth = 0.0f;
		worldNormal = 0.0f;
		roughness = 0.0f;
		return false;
	}

	worldPosition = ScreenPositionToWorld(float3(float2(pixel) + 0.5f, ndcDepth), screenSize);
	viewDepth = Camera_GetDepth(worldPosition);
	worldNormal = normalize(2.0f * gbufferNormalTexture.Load(int3(pixel, 0)).xyz - 1.0f);
	roughness = gbufferSurfacePropertiesTexture.Load(int3(pixel, 0))[DEFERRED_SURFACE_PROPERTIES_ROUGHNESS_CHANNEL];
	return true;
}



[numthreads(32, 32, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
	if (threadID.x >= pc.threadCount.x || threadID.y >= pc.threadCount.y)
		return;

	// Screen size:
	uint screenWidth;
	uint screenHeight;
	sceneDepthTexture.GetDimensions(screenWidth, screenHeight);
	uint2 screenSizeUint = uint2(screenWidth, screenHeight);
	float2 screenSize = float2(screenSizeUint);

	// Source pixel/color:
	uint2 sourcePixel = threadID.xy;
	float4 sourceColor = Scene_GetColor(sourcePixel);

	// Get surface data:
	float3 worldPosition;
	float viewDepth;
	float3 worldNormal;
	float roughness;
	if (TryGetSurfaceData(sourcePixel, screenSize, worldPosition, viewDepth, worldNormal, roughness) == false)
	{
		Scene_SetColor(sourcePixel, sourceColor);
		return;
	}

	// Reflection map size:
	uint reflectionWidth;
	uint reflectionHeight;
	reflectionUvMap.GetDimensions(reflectionWidth, reflectionHeight);
	float2 reflectionSize = float2(reflectionWidth, reflectionHeight);

	// Map reflection position to nearest reflection pixel:
	float2 reflectionPosition = (float2(sourcePixel) + 0.5f) * reflectionSize / screenSize - 0.5f;
	int2 nearestReflectionPixel = int2(floor(reflectionPosition + 0.5f));

	// Loop over upsample neighbourhood:
	float2 weightedHitUv = 0.0f;
	float totalHitWeight = 0.0f;
	float totalWeight = 0.0f;
	float normalWeightExponent = lerp(maximumNormalWeightExponent, minimumNormalWeightExponent, saturate(roughness));
	float depthTolerance = max(minimumDepthTolerance, relativeDepthTolerance * viewDepth);
	for (int y = -upsampleRadius; y <= upsampleRadius; y++)
	{
		for (int x = -upsampleRadius; x <= upsampleRadius; x++)
		{
			// Skip out of image pixels:
			int2 reflectionPixel = nearestReflectionPixel + int2(x, y);
			if (reflectionPixel.x < 0 || reflectionPixel.x >= int(reflectionWidth) ||
				reflectionPixel.y < 0 || reflectionPixel.y >= int(reflectionHeight))
				continue;

			// Sky rays:
			float4 reflectionUvSample = reflectionUvMap[reflectionPixel];
			if (reflectionUvSample.a <= 0.0f)
				continue;

			// Recover the full-resolution source pixel represented by this reflection texel.
			float2 sampleScreenPosition = (float2(reflectionPixel) + 0.5f) * screenSize / reflectionSize;
			uint2 sampleScreenPixel = min(uint2(sampleScreenPosition), screenSizeUint - 1);
			float3 sampleWorldPosition;
			float sampleViewDepth;
			float3 sampleWorldNormal;
			float sampleRoughness;
			if (TryGetSurfaceData(sampleScreenPixel, screenSize, sampleWorldPosition, sampleViewDepth, sampleWorldNormal, sampleRoughness) == false)
				continue;

			// Weighting:
			float2 pixelOffset = float2(reflectionPixel) - reflectionPosition;
			float spatialWeight = exp(-spatialWeightScale * dot(pixelOffset, pixelOffset));
			float normalizedDepthDifference = abs(sampleViewDepth - viewDepth) / depthTolerance;
			float depthWeight = exp(-normalizedDepthDifference * normalizedDepthDifference);
			float normalWeight = pow(saturate(dot(worldNormal, sampleWorldNormal)), normalWeightExponent);
			float roughnessWeight = exp(-roughnessWeightScale * abs(sampleRoughness - roughness));
			float weight = spatialWeight * depthWeight * normalWeight * roughnessWeight * saturate(reflectionUvSample.a);

			// Accumulate:
			totalWeight += weight;
			float hitWeight = weight * saturate(reflectionUvSample.b);
			weightedHitUv += hitWeight * reflectionUvSample.rg;
			totalHitWeight += hitWeight;
		}
	}

	// No reflections:
	if (totalWeight <= 1.0e-5f)
	{
		Scene_SetColor(sourcePixel, sourceColor);
		return;
	}

	// Fade reflections near screen boundaries to hide cut off broken reflections:
	float3 cameraRayDirection = Camera_GetRayDirection(worldPosition);
	float3 reflectionDirection = normalize(reflect(cameraRayDirection, worldNormal));
	float3 reflectionColor = Reflection_GetEnvironmentColor(environmentMap, reflectionDirection, roughness);
	if (totalHitWeight > 1.0e-5f)
	{
		float2 hitUv = weightedHitUv / totalHitWeight;
		float edgeDistance = min(min(hitUv.x, 1.0f - hitUv.x), min(hitUv.y, 1.0f - hitUv.y));
		float edgeFade = saturate(10.0f * edgeDistance);
		float hitConfidence = saturate(totalHitWeight / totalWeight);
		reflectionColor = lerp(reflectionColor, Scene_SampleColor(hitUv).rgb, edgeFade * hitConfidence);
	}

	// Apply the material BRDF at full resolution and add indirect specular lighting:
	float3 albedo = gbufferAlbedoTexture.Load(int3(sourcePixel, 0)).rgb;
	float4 surfaceProperties = gbufferSurfacePropertiesTexture.Load(int3(sourcePixel, 0));
	float nDotV = saturate(dot(worldNormal, -cameraRayDirection));
	float3 specularResponse = Reflection_GetSpecularResponse(albedo, surfaceProperties, nDotV);
	float reconstructionConfidence = saturate(totalWeight);
	Scene_SetColor(sourcePixel, float4(sourceColor.rgb + reconstructionConfidence * reflectionColor * specularResponse, sourceColor.a));
}