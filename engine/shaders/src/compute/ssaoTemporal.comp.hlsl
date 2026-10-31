#include "computeShaderCommon.hlsli"
#include "bilateralUpsampling.hlsli"



// Bindings:
[[vk::image_format("r16f")]] RWTexture2D<float> rawAmbientOcclusionMap : register(u200, CALL_SET);		// current frame's raw ambient occlusion (read).
[[vk::image_format("r16f")]] RWTexture2D<float> previousAmbientOcclusionMap : register(u201, CALL_SET);	// old accumulated ambient occlusion at history pixels (read).
[[vk::image_format("r16f")]] RWTexture2D<float> currentAmbientOcclusionMap : register(u202, CALL_SET);	// current frame's accumulated ambient occlusion (write).
[[vk::image_format("rgba32f")]] RWTexture2D<float4> previousNormalDepth : register(u203, CALL_SET);		// old normals and depths at history pixels (read).
[[vk::image_format("rgba32f")]] RWTexture2D<float4> currentNormalDepth : register(u204, CALL_SET);		// current frame's normals and depths at AO pixels (write).
// Temporal reconstruction parameters:
cbuffer SsaoTemporalParameters : register(b300, CALL_SET)
{
	bool historyValid;
	float historyWeight;			// in [0,1].
	float minimumDepthTolerance;	// in world units.
	float relativeDepthTolerance;	// > 0.
	float minimumNormalSimilarity;	// in [0,1].
};



[numthreads(8, 8, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
	if (threadID.x >= pc.threadCount.x || threadID.y >= pc.threadCount.y)
		return;

	// Ambient occlusion map size:
	uint aoWidth;
	uint aoHeight;
	rawAmbientOcclusionMap.GetDimensions(aoWidth, aoHeight);
	if (threadID.x >= aoWidth || threadID.y >= aoHeight)
		return;
	uint2 aoSize = uint2(aoWidth, aoHeight);
	uint2 aoPixel = threadID.xy;

	// Screen size:
	uint screenWidth;
	uint screenHeight;
	sceneDepthTexture.GetDimensions(screenWidth, screenHeight);
	uint2 screenSize = uint2(screenWidth, screenHeight);
	uint2 sourcePixel = Bilateral_LowResolutionPixelToScreenPixel(aoPixel, screenSize, float2(aoSize));

	// Get surface data:
	float occlusion = rawAmbientOcclusionMap[aoPixel];
	float3 worldPosition;
	float viewDepth;
	float3 worldNormal;
	if (!Bilateral_TryGetSurfaceData(sourcePixel, float2(screenSize), worldPosition, viewDepth, worldNormal))
	{
		currentAmbientOcclusionMap[aoPixel] = 0.0f;
		currentNormalDepth[aoPixel] = float4(0.0f, 0.0f, 0.0f, 1.0f);
		return;
	}

	// Write current AO+normal+depth, so next frame can use it for previousAmbientOcclusionMap and previousNormalDepth:
	currentAmbientOcclusionMap[aoPixel] = occlusion;
	currentNormalDepth[aoPixel] = float4(worldNormal, Scene_GetNdcDepth(sourcePixel));

	// No valid history exists, so we can stop here:
	if (!historyValid)
		return;

	// Reproject into the previous frame:
	float2 sourceUv = (float2(sourcePixel) + 0.5f) / float2(screenSize);
	float2 previousUv = sourceUv - gbufferVelocityTexture.Load(int3(sourcePixel, 0));
	if (!all(isfinite(previousUv)) || any(previousUv < 0.0f) || any(previousUv >= 1.0f))
		return;

	// Get bilinear interpolation data for previousUv,
	// as we need to check all 4 possible pixels normal and depth to see if they belong to the same surface:
	int2 lowerPixel;
	int2 upperPixel;
	float2 fraction;
	Bilateral_GetBilinearSamplingData(previousUv, screenSize, aoSize, lowerPixel, upperPixel, fraction);

	// Bilateral interpolation between the four surrounding pixels.
	// If any of the four candidates does not belong to the same surface, it gets rejected:
	float depthTolerance = max(minimumDepthTolerance, relativeDepthTolerance * viewDepth);
	float weightedAmbientOcclusionAccumulation = 0.0f;
	float totalWeight = 0.0f;
	for (int y = 0; y < 2; y++)
	{
	    for (int x = 0; x < 2; x++)
	    {
			// Select one of the four neighboring history pixels:
	        int2 historyPixel = int2(x == 0 ? lowerPixel.x : upperPixel.x, y == 0 ? lowerPixel.y : upperPixel.y);

			// Reject sky pixels and incompatible surface normals:
	        float4 historyNormalDepth = previousNormalDepth[historyPixel];
	        if (historyNormalDepth.w >= 1.0f || dot(worldNormal, historyNormalDepth.xyz) < minimumNormalSimilarity)
	            continue;

	        // Reconstruct world position using the previous camera:
	        uint2 historyScreenPixel = Bilateral_LowResolutionPixelToScreenPixel(uint2(historyPixel), screenSize, float2(aoSize));
	        float2 historyUv = (float2(historyScreenPixel) + 0.5f) / float2(screenSize);
	        float4 previousWorldPosition = mul(camera_clipToWorldMatrixOld, float4(2.0f * historyUv - 1.0f, historyNormalDepth.w, 1.0f));

			// Reject invalid positions and incompatible depths:
	        if (abs(previousWorldPosition.w) < 1.0e-6f)
	            continue;
	        float previousViewDepth = Camera_GetDepth(previousWorldPosition.xyz / previousWorldPosition.w);
	        if (abs(previousViewDepth - viewDepth) > depthTolerance)
	            continue;

	        float weight = (x == 0 ? 1.0f - fraction.x : fraction.x) * (y == 0 ? 1.0f - fraction.y : fraction.y);
	        weightedAmbientOcclusionAccumulation += weight * previousAmbientOcclusionMap[historyPixel];
	        totalWeight += weight;
	    }
	}

	// None of the candidate pixels hit:
	if (totalWeight <= 1.0e-5f)
		return;

	// Find the current AO range among nearby pixels on the same surface.
	// Clamp previous AO to this range before blending, so outdated occlusion fades faster.
	float minimumOcclusion = occlusion;
	float maximumOcclusion = occlusion;
	for (int y = -1; y <= 1; y++)
	{
		for (int x = -1; x <= 1; x++)
		{
			int2 neighborPixel = int2(aoPixel) + int2(x, y);
			if (any(neighborPixel < 0) || any(neighborPixel >= int2(aoSize)))
				continue;

			// Get surface data:
			uint2 neighborScreenPixel = Bilateral_LowResolutionPixelToScreenPixel(uint2(neighborPixel), screenSize, float2(aoSize));
			float3 neighborWorldPosition;
			float neighborViewDepth;
			float3 neighborWorldNormal;
			if (!Bilateral_TryGetSurfaceData(neighborScreenPixel, float2(screenSize), neighborWorldPosition, neighborViewDepth, neighborWorldNormal))
				continue;

			// Reject neighbors with incompatible depth or surface normal.
			if (abs(neighborViewDepth - viewDepth) > depthTolerance || dot(worldNormal, neighborWorldNormal) < minimumNormalSimilarity)
				continue;

			float neighborOcclusion = rawAmbientOcclusionMap[neighborPixel];
			minimumOcclusion = min(minimumOcclusion, neighborOcclusion);
			maximumOcclusion = max(maximumOcclusion, neighborOcclusion);
		}
	}

	// Update accumulated ambient occlusion:
	float clampedHistoryOcclusion = clamp(weightedAmbientOcclusionAccumulation / totalWeight, minimumOcclusion, maximumOcclusion);
	currentAmbientOcclusionMap[aoPixel] = lerp(occlusion, clampedHistoryOcclusion, historyWeight * saturate(totalWeight));
}