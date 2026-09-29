#define EMBER_SCENE_COLOR_ACCESS EMBER_SCENE_COLOR_ACCESS_READ
#include "computeShaderCommon.hlsli"
#include "screenSpaceReflectionUtility.hlsli"



// Bindings:
TextureCube<float4> environmentMap : register(t100, CALL_SET);
[[vk::image_format("rgba16f")]] RWTexture2D<float4> reflectionMap : register(u200, CALL_SET);



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

	// Reflection map size:
	uint reflectionWidth;
	uint reflectionHeight;
	reflectionMap.GetDimensions(reflectionWidth, reflectionHeight);
	float2 reflectionSize = float2(reflectionWidth, reflectionHeight);

	// Screen size:
	uint screenWidth;
	uint screenHeight;
	sceneDepthTexture.GetDimensions(screenWidth, screenHeight);
	float2 screenSize = float2(screenWidth, screenHeight);

	// Reflection texel mapped to full resolution source pixel:
	uint2 maxScenePixel = uint2(screenWidth - 1, screenHeight - 1);
	float2 sourcePosition = (float2(threadID.xy) + 0.5f) * screenSize / reflectionSize;
	uint2 sourcePixel = min(uint2(sourcePosition), maxScenePixel);

	// Sky rays:
	float3 worldPosition;
	if (TryGetGeometryWorldPosition(sourcePixel, screenSize, worldPosition) == false)
	{
		reflectionMap[threadID.xy] = 0.0f;	// miss.
		return;
	}

	// Reflect the camera ray at the current opaque surface, then offset the origin to avoid self-hits:
	float3 worldNormal = normalize(2.0f * gbufferNormalTexture.Load(int3(sourcePixel, 0)).xyz - 1.0f);
	float3 cameraRayDirection = Camera_GetRayDirection(worldPosition);
	float surfaceDepth = Camera_GetDepth(worldPosition);
	float depthBias = max(0.02f, 0.0005f * surfaceDepth);
	float3 rayOrigin = worldPosition + depthBias * worldNormal;
	float3 reflectionDirection = normalize(reflect(cameraRayDirection, worldNormal));
	WorldRay worldRay = {rayOrigin, reflectionDirection};

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
		reflectionColor = lerp(reflectionColor, GetSceneColor(hitPixel).rgb, edgeFade);
	}

	// Store reflection color and has hit (alpha=1) for composit pass:
	reflectionMap[threadID.xy] = float4(reflectionColor, 1.0f);
}