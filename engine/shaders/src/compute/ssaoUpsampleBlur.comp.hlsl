#include "computeShaderCommon.hlsli"
#include "bilateralUpsampling.hlsli"
#include "deferredRenderingConstants.h"



// Bindings:
[[vk::image_format("r16f")]] RWTexture2D<float> ambientOcclusionMap : register(u200, CALL_SET);



// Bilateral blur and reconstruction parameters:
cbuffer SsaoReconstructionParameters : register(b300, CALL_SET)
{
	int blurRadius;                 // in AO texels.
	float spatialWeightScale;
	float minimumDepthTolerance;    // in world units.
	float relativeDepthTolerance;
	float normalWeightExponent;
	float occlusionStrength;
};



[numthreads(8, 8, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
	// Note:
	// The gaussian blur loop computes blurred pixel at sourcePixel from aoMap texels (upsampling).

	if (threadID.x >= pc.threadCount.x || threadID.y >= pc.threadCount.y)
		return;

	// Screen size:
	uint screenWidth;
	uint screenHeight;
	sceneDepthTexture.GetDimensions(screenWidth, screenHeight);
	if (threadID.x >= screenWidth || threadID.y >= screenHeight)
		return;
	uint2 screenSizeUint = uint2(screenWidth, screenHeight);
	float2 screenSize = float2(screenSizeUint);
	uint2 sourcePixel = threadID.xy;

	// Get surface data:
	float3 worldPosition;
	float viewDepth;
	float3 worldNormal;
	if (!Bilateral_TryGetSurfaceData(sourcePixel, screenSize, worldPosition, viewDepth, worldNormal))
		return;

	// Ambient occlusion map size:
	uint aoWidth;
	uint aoHeight;
	ambientOcclusionMap.GetDimensions(aoWidth, aoHeight);
	float2 aoSize = float2(aoWidth, aoHeight);

	// Prepare bilateral filtering:
	float2 screenPosition = float2(sourcePixel) + 0.5f;
	float2 aoPosition = screenPosition * aoSize / screenSize;
	int2 nearestAoPixel = Bilateral_ScreenPixelToLowResolutionPixel(sourcePixel, screenSize, aoSize);
	float depthTolerance = max(minimumDepthTolerance, relativeDepthTolerance * viewDepth);

	// Depth/Normal aware gaussian blur:
	float weightedOcclusion = 0.0f;
	float totalWeight = 0.0f;
	for (int y = -blurRadius; y <= blurRadius; y++)
	{
		for (int x = -blurRadius; x <= blurRadius; x++)
		{
			// Skip out of image pixels:
			int2 samplePixel = nearestAoPixel + int2(x, y);
			if (samplePixel.x < 0 || samplePixel.x >= int(aoWidth) || samplePixel.y < 0 || samplePixel.y >= int(aoHeight))
				continue;
			uint2 aoPixel = uint2(samplePixel);
			uint2 sampleScreenPixel = Bilateral_LowResolutionPixelToScreenPixel(aoPixel, screenSizeUint, aoSize);

			// Get sample surface data:
			float3 sampleWorldPosition;
			float sampleViewDepth;
			float3 sampleWorldNormal;
			if (!Bilateral_TryGetSurfaceData(sampleScreenPixel, screenSize, sampleWorldPosition, sampleViewDepth, sampleWorldNormal))
				continue;

			// Weighting:
			float2 pixelOffset = float2(aoPixel) + 0.5f - aoPosition;
			float spatialWeight = Bilateral_SpatialWeight(pixelOffset, spatialWeightScale);
			float depthWeight = Bilateral_DepthWeight(viewDepth, sampleViewDepth, depthTolerance);
			float normalWeight = Bilateral_NormalWeight(worldNormal, sampleWorldNormal, normalWeightExponent);
			float weight = spatialWeight * depthWeight * normalWeight;

			// Accumulate:
			weightedOcclusion += weight * ambientOcclusionMap[aoPixel];
			totalWeight += weight;
		}
	}

	// No compatible surface samples means no screen-space occlusion at this pixel:
	float occlusion = totalWeight > 1.0e-5f ? weightedOcclusion / totalWeight : 0.0f;
	float4 surfaceProperties = gbufferSurfacePropertiesStorageTexture[sourcePixel];
	surfaceProperties[DEFERRED_SURFACE_PROPERTIES_AMBIENT_OCCLUSION_CHANNEL] = saturate(1.0f - occlusionStrength * occlusion);
	gbufferSurfacePropertiesStorageTexture[sourcePixel] = surfaceProperties;
}