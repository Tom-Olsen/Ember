#ifndef __INCLUDE_GUARD_screenSpaceAmbientOcclusion_hlsli__
#define __INCLUDE_GUARD_screenSpaceAmbientOcclusion_hlsli__
#include "descriptorSetMacros.h"
#include "ssaoConstants.h"



// Sampling parameters:
cbuffer SsaoSamplingParameters : register(b300, SHADER_SET)
{
	uint sampleCount;
	float sampleRadius;	// in world units.
	float sampleBias;	// in world units.
	float occlusionStrength;
};



// Cosine weighted local hemisphere, regenerated on the CPU when sampleCount changes:
cbuffer SsaoSamplingKernel : register(b301, SHADER_SET)
{
	float3 sampleDirections[SSAO_MAX_SAMPLE_COUNT];
};



// Shared estimator for full- and half-resolution AO:
float Ssao_GetOcclusion(uint2 sourcePixel, float sourceNdcDepth, float2 screenSize)
{
	// Reconstruct the source surface and orient the sampling hemisphere along its world-space normal:
	float3 worldPosition = Camera_GetWorldPosition(sourcePixel, sourceNdcDepth, screenSize);
	float3 worldNormal = normalize(2.0f * gbufferNormalTexture.Load(int3(sourcePixel, 0)).xyz - 1.0f);
	float3 tangent = normalize(mathLinAlg_GetOrhtogonalVector(worldNormal));
	float3 bitangent = cross(worldNormal, tangent);
	float3 sampleOrigin = worldPosition + sampleBias * worldNormal;

	// Keep the rotation fixed between frames to avoid temporal flicker without a history buffer:
	float rotation = 2.0f * math_PI * mathRandom_Uniform01(sourcePixel.x, sourcePixel.y, 0.0f);
	float sinRotation;
	float cosRotation;
	sincos(rotation, sinRotation, cosRotation);
	float3 rotatedTangent = cosRotation * tangent + sinRotation * bitangent;
	float3 rotatedBitangent = -sinRotation * tangent + cosRotation * bitangent;
	float occlusion = 0.0f;
	for (uint i = 0; i < sampleCount; i++)
	{
		// Transform the precomputed local direction into the rotated world-space hemisphere:
		float3 localDirection = sampleDirections[i];
		float3 direction = localDirection.x * rotatedTangent + localDirection.y * rotatedBitangent + localDirection.z * worldNormal;

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

	return occlusion / float(sampleCount);
}



// Apply strength after reconstruction, before writing the G-buffer:
float Ssao_GetAmbientOcclusion(float occlusion)
{
	return saturate(1.0f - occlusionStrength * occlusion);
}



#endif // __INCLUDE_GUARD_screenSpaceAmbientOcclusion_hlsli__