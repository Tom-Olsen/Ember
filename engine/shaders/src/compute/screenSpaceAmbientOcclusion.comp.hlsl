#include "computeShaderCommon.hlsli"
#include "deferredRenderingConstants.h"



// Sampling parameters:
static const uint sampleCount = 32;
static const float sampleRadius = 1.0f;	// in world units.
static const float sampleBias = 0.01f;	// in world units.
static const float occlusionStrength = 5.0f;



[numthreads(8, 8, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
	if (threadID.x >= pc.threadCount.x || threadID.y >= pc.threadCount.y)
		return;

	// Screen size:
	uint screenWidth;
	uint screenHeight;
	sceneDepthTexture.GetDimensions(screenWidth, screenHeight);
	if (threadID.x >= screenWidth || threadID.y >= screenHeight)
		return;
	float2 screenSize = float2(screenWidth, screenHeight);
	uint2 sourcePixel = threadID.xy;

	// Scene depth and skip sky:
	float sourceNdcDepth = Scene_GetNdcDepth(sourcePixel);
	if (sourceNdcDepth >= 1.0f)
		return;

	// Reconstruct the source surface and orient the sampling hemisphere along its world-space normal:
	float3 worldPosition = Camera_GetWorldPosition(sourcePixel, sourceNdcDepth, screenSize);
	float3 worldNormal = normalize(2.0f * gbufferNormalTexture.Load(int3(sourcePixel, 0)).xyz - 1.0f);
	float3 tangent = normalize(mathLinAlg_GetOrhtogonalVector(worldNormal));
	float3 bitangent = cross(worldNormal, tangent);
	float3 sampleOrigin = worldPosition + sampleBias * worldNormal;

	// Keep the rotation fixed between frames to avoid temporal flicker without a history buffer:
	float rotation = 2.0f * math_PI * mathRandom_Uniform01(sourcePixel.x, sourcePixel.y, 0.0f);
	float occlusion = 0.0f;
	for (uint i = 0; i < sampleCount; i++)
	{
		// Fibonacci spiral angles:
		float directionFraction = (float(i) + 0.5f) / float(sampleCount);
		float angle = rotation + math_GOLDEN_ANGLE * float(i);
		float sinAngle;
		float cosAngle;
		sincos(angle, sinAngle, cosAngle);

		// Construct cosine weighted half hemisphere orthogonal to surface normal:
		float diskRadius = sqrt(directionFraction);
		float3 direction = diskRadius * (cosAngle * tangent + sinAngle * bitangent) + sqrt(1.0f - directionFraction) * worldNormal;

		// Sample distance independently from direction, keeping the quadratic bias toward the source:
		float distanceFraction = mathRandom_Uniform01(sourcePixel.x, sourcePixel.y, i, 1u, 0.0f);
		float distanceScale = lerp(0.1f, 1.0f, distanceFraction * distanceFraction);
		float3 samplePosition = sampleOrigin + sampleRadius * distanceScale * direction;

		// Project the sample into the depth buffer. Miss = continue:
		float4 sampleClip = Camera_PositionWorldToClip(samplePosition);
		if (sampleClip.w <= 0.0f)
			continue;
		float3 sampleNdc = sampleClip.xyz / sampleClip.w;
		float2 sampleUv = sampleNdc.xy * 0.5f + 0.5f;
		float3 sampleCoord = float3(sampleUv, sampleNdc.z);
		if (any(sampleCoord < 0.0f) || any(sampleCoord >= 1.0f))
			continue;
		uint2 samplePixel = uint2(sampleUv * screenSize);
		if (all(samplePixel == sourcePixel))
			continue;
		float sceneNdcDepth = Scene_GetNdcDepth(samplePixel);
		if (sceneNdcDepth >= 1.0f)
			continue;

		// Compare linear view depths instead of nonlinear NDC depths:
		float3 scenePosition = Camera_GetWorldPosition(samplePixel, sceneNdcDepth, screenSize);
		float sampleViewDepth = Camera_GetDepth(samplePosition);
		float sceneViewDepth = Camera_GetDepth(scenePosition);
		if (sampleViewDepth <= sceneViewDepth + sampleBias)
			continue;

		// Range attenuation: small offset -> big ambient occlusion, big offset -> small ambient occlusion.
		float3 surfaceOffset = scenePosition - worldPosition;
		float rangeWeight = saturate(1.0f - dot(surfaceOffset, surfaceOffset) / (sampleRadius * sampleRadius));
		occlusion += rangeWeight * rangeWeight;
	}

	// Write ambient occlusion:
	float ambientOcclusion = saturate(1.0f - occlusionStrength * occlusion / float(sampleCount));
	float4 surfaceProperties = gbufferSurfacePropertiesStorageTexture[sourcePixel];
	surfaceProperties[DEFERRED_SURFACE_PROPERTIES_AMBIENT_OCCLUSION_CHANNEL] = ambientOcclusion;
	gbufferSurfacePropertiesStorageTexture[sourcePixel] = surfaceProperties;
}