#ifndef __INCLUDE_GUARD_reflectionLighting_hlsli__
#define __INCLUDE_GUARD_reflectionLighting_hlsli__
#include "deferredRenderingConstants.h"



float3 Reflection_GetEnvironmentColor(TextureCube<float4> environmentMap, float3 worldDirection, float roughness)
{
	uint width;
	uint height;
	uint mipCount;
	environmentMap.GetDimensions(0, width, height, mipCount);
	// Ordinary image mips approximate a rough reflection; they are not a GGX prefiltered cubemap.
	float mipLevel = saturate(roughness) * float(mipCount - 1);
	float3 cubeDirection = mul(mathLinAlg_RotateX3x3(-math_PI_2), worldDirection);
	return environmentMap.SampleLevel(colorSampler, cubeDirection, mipLevel).rgb;
}
float3 Reflection_GetSpecularResponse(float3 albedo, float4 surfaceProperties, float nDotV)
{
	uint flagBitMask = uint(round(surfaceProperties[DEFERRED_SURFACE_PROPERTIES_FLAG_BIT_MASK_CHANNEL] * DEFERRED_SURFACE_FLAG_BIT_MASK_MAX));
	if ((flagBitMask & DEFERRED_SURFACE_FLAG_LIT) == 0)
		return 0.0f;

	float metallicity = surfaceProperties[DEFERRED_SURFACE_PROPERTIES_METALLICITY_CHANNEL];
	float roughness = surfaceProperties[DEFERRED_SURFACE_PROPERTIES_ROUGHNESS_CHANNEL];
	float3 reflectivity = lerp(float3(0.04f, 0.04f, 0.04f), saturate(albedo), saturate(metallicity));

	// Analytic approximation of the integrated GGX environment BRDF (Karis/Lazarov):
	// https://www.unrealengine.com/blog/physically-based-shading-on-mobile
	float4 coefficients = saturate(roughness) * float4(-1.0f, -0.0275f, -0.572f, 0.022f) + float4(1.0f, 0.0425f, 1.04f, -0.04f);
	float grazingTerm = min(coefficients.x * coefficients.x, exp2(-9.28f * saturate(nDotV))) * coefficients.x + coefficients.y;
	float2 integratedBrdf = float2(-1.04f, 1.04f) * grazingTerm + coefficients.zw;
	return saturate(reflectivity * integratedBrdf.x + integratedBrdf.y);
}



#endif // __INCLUDE_GUARD_reflectionLighting_hlsli__