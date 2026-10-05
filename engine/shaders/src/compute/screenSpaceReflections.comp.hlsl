#define EMBER_SCENE_COLOR_ACCESS EMBER_SCENE_COLOR_ACCESS_OUT_OF_PLACE
#include "computeShaderCommon.hlsli"
#include "deferredRenderingConstants.h"
#include "screenSpaceReflectionUtility.hlsli"



// Bindings:
TextureCube<float4> environmentMap : register(t100, CALL_SET);



// Helpers:
float3 GetEnvironmentColor(float3 worldDirection)
{
	float3 cubeDirection = mul(mathLinAlg_RotateX3x3(-math_PI_2), worldDirection);
	return environmentMap.SampleLevel(colorSampler, cubeDirection, 0.0f).rgb;
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
	float2 screenSize = float2(screenWidth, screenHeight);

	// Source pixel/color:
	uint2 sourcePixel = threadID.xy;
	float4 sourceColor = Scene_GetColor(sourcePixel);

	// Sky rays:
	float3 worldPosition;
	if (TryGetGeometryWorldPosition(sourcePixel, screenSize, worldPosition) == false)
	{
		Scene_SetColor(sourcePixel, sourceColor);
		return;
	}

	// Reflect the camera ray at the current opaque surface, then offset the origin to avoid self-hits:
	float3 worldNormal = normalize(2.0f * gbufferNormalTexture.Load(int3(sourcePixel, 0)).xyz - 1.0f);
	float3 cameraRayDirection = Camera_GetRayDirection(worldPosition);
	float surfaceDepth = Camera_GetDepth(worldPosition);
	float depthBias = max(0.02f, 0.0005f * surfaceDepth);
	float3 rayOrigin = worldPosition + depthBias * worldNormal;
	float3 reflectionDirection = normalize(reflect(cameraRayDirection, worldNormal));
	math_Ray worldRay = {rayOrigin, reflectionDirection};

	// Screen space ray marching:
	uint2 hitPixel;
	float2 hitUv;
	bool hasHit = ScreenSpaceRayMarch(sourcePixel, worldRay, screenSize, hitPixel, hitUv);

	// Reflect skybox when ray hit misses:
	float3 reflectionColor = GetEnvironmentColor(reflectionDirection);

	// Fade reflections near screen boundaries to hide cut off broken reflections:
	if (hasHit)
	{
		float edgeDistance = min(min(hitUv.x, 1.0f - hitUv.x), min(hitUv.y, 1.0f - hitUv.y));
		float edgeFade = saturate(10.0f * edgeDistance);
		reflectionColor = lerp(reflectionColor, Scene_GetColor(hitPixel).rgb, edgeFade);
	}

	// Physically based reflections:
	float3 albedo = gbufferAlbedoTexture.Load(int3(sourcePixel, 0)).rgb;
	float4 surfaceProperties = gbufferSurfacePropertiesTexture.Load(int3(sourcePixel, 0));
	float metallicity = surfaceProperties[DEFERRED_SURFACE_PROPERTIES_METALLICITY_CHANNEL];
	float3 reflectivity = lerp(float3(0.04f, 0.04f, 0.04f), saturate(albedo), saturate(metallicity));
	float nDotV = saturate(dot(worldNormal, -cameraRayDirection));
	float3 fresnel = reflectivity + (1.0f - reflectivity) * pow(1.0f - nDotV, 5.0f);
	Scene_SetColor(sourcePixel, float4(lerp(sourceColor.rgb, reflectionColor, fresnel), sourceColor.a));
}