#define EMBER_SCENE_COLOR_ACCESS EMBER_SCENE_COLOR_ACCESS_OUT_OF_PLACE
#include "computeShaderCommon.hlsli"
#include "deferredRenderingConstants.h"
#include "globalSet.hlsli"



TextureCube<float4> environmentMap : register(t100, CALL_SET);



// Ray march budget and binary-search iterations after a depth crossing:
static const uint maxStepCount = 64;
static const uint refinementStepCount = 8;
static const uint pixelStepSize = 10;



// Getters:
// Converts pixel+ndcDepth to world position:
float3 GetWorldPosition(uint2 pixel, float ndcDepth)
{
    uint width;
    uint height;
    sceneDepthTexture.GetDimensions(width, height);
    float2 uv = (float2(pixel) + 0.5f) / float2(width, height);
    float4 clipPosition = float4(2.0f * uv - 1.0f, ndcDepth, 1.0f);
    float4 worldPosition = mul(camera_clipToWorldMatrix, clipPosition);
    return worldPosition.xyz / worldPosition.w;
}
// Get world position of drawn geometry, or fails:
bool TryGetWorldPosition(uint2 pixel, out float3 worldPosition)
{
    float sceneDepth = GetSceneNdcDepth(pixel);
    if (sceneDepth >= 1.0f)
    {
        worldPosition = 0.0f;
        return false;
    }
    worldPosition = GetWorldPosition(pixel, sceneDepth);
    return true;
}
// Environment color in worldDirection:
float3 GetEnvironmentColor(float3 worldDirection)
{
    float3 cubeDirection = mul(mathLinAlg_RotateX3x3(-math_PI_2), worldDirection);
    return environmentMap.SampleLevel(colorSampler, cubeDirection, 0.0f).rgb;
}



// Screen space ray marching:
bool ScreenSpaceRayMarch(uint2 sourcePixel, WorldRay worldRay, out uint2 hitPixel, out float2 hitUv)
{
	// Screen Size:
	uint screenWidth;
    uint screenHeight;
    sceneDepthTexture.GetDimensions(screenWidth, screenHeight);
	float2 screenSize = float2(screenWidth, screenHeight);

	// Screen-space ray normalized to one pixel along its dominant axis:
	ScreenRay screenRay = ProjectRayToScreen(worldRay, screenSize, camera_worldToClipMatrix);
	float maxDir = max(abs(screenRay.direction.x), abs(screenRay.direction.y));
	if (maxDir < 1e-4f)
		return false;
	screenRay.direction /= maxDir;

    ScreenRaySample previousSample;
    bool hasPreviousSample = false;
    for (uint step = 1; step <= maxStepCount; step++)
	{
		// Screen ray sample:
		ScreenRaySample raySample;
		raySample.screenPosition = screenRay.origin + step * pixelStepSize * screenRay.direction;

		// Reject offscreen samples:
		if (raySample.screenPosition.x < 0 || screenWidth <= raySample.screenPosition.x
		 || raySample.screenPosition.y < 0 || screenHeight <= raySample.screenPosition.y
		 || raySample.screenPosition.z < 0 || 1 <= raySample.screenPosition.z)
			break;

		// Pixel and ndcDepth after ray marching:
		uint2 pixel = uint2(raySample.screenPosition.xy);
		float ndcDepth = raySample.screenPosition.z;

		// Reject pixels without geometry:
    	float sceneNdcDepth = GetSceneNdcDepth(pixel);
    	if (sceneNdcDepth >= 1.0f)
			continue;

		// Go back to world space:
		float3 rayWorldPosition   = GetWorldPosition(pixel, ndcDepth);
		float3 sceneWorldPosition = GetWorldPosition(pixel, sceneNdcDepth);

		// Compute depth delta between worldPosition and scene geometry:
		float rayViewDepth   = Camera_GetDepth(rayWorldPosition);
		raySample.sceneViewDepth = Camera_GetDepth(sceneWorldPosition);
		raySample.depthDelta = rayViewDepth - raySample.sceneViewDepth;

		// Reject self-reflections around the ray origin:
        int2 pixelOffset = int2(pixel) - int2(sourcePixel);
        bool isOutsideSourceNeighborhood = any(abs(pixelOffset) > 1);

		// Test:
		if (isOutsideSourceNeighborhood && hasPreviousSample && previousSample.depthDelta < 0.0f && sample.depthDelta >= 0.0f)
		{
			hitPixel = pixel;
			hitUv = (float2(hitPixel) + 0.5f) / screenSize;
			return true;
		}
		previousSample = raySample;
		hasPreviousSample = true;
	}

	// No hit detected:
    hitPixel = 0;
    hitUv = 0.0f;
    return false;
}



[numthreads(32, 32, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
    if (threadID.x >= pc.threadCount.x || threadID.y >= pc.threadCount.y)
        return;

    uint2 pixel = threadID.xy;
    float4 sourceColor = GetSceneColor(pixel);
    float3 worldPosition;
    if (!TryGetWorldPosition(pixel, worldPosition))
    {
        SetSceneColor(pixel, sourceColor);
        return;
    }

    // Reflect the camera ray at the current opaque surface, then offset the origin to avoid self-hits.
    float3 worldNormal = normalize(2.0f * gbufferNormalTexture.Load(int3(pixel, 0)).xyz - 1.0f);
    float3 cameraRayDirection = Camera_GetRayDirection(worldPosition);
    float surfaceDepth = Camera_GetDepth(worldPosition);
    float depthBias = max(0.02f, 0.0005f * surfaceDepth);
    float3 rayOrigin = worldPosition + depthBias * worldNormal;
    float3 reflectionDirection = normalize(reflect(cameraRayDirection, worldNormal));
	WorldRay worldRay = {rayOrigin, reflectionDirection};

	// Screen space ray marching:
    uint2 hitPixel;
    float2 hitUv;
    bool hasHit = ScreenSpaceRayMarch(pixel, worldRay, hitPixel, hitUv);

    // Reflect skybox when ray hit misses:
    float3 reflectionColor = GetEnvironmentColor(reflectionDirection);

	// Fade reflections near screen boundaries to hide cut off broken reflections:
    if (hasHit)
    {
        float edgeDistance = min(min(hitUv.x, 1.0f - hitUv.x), min(hitUv.y, 1.0f - hitUv.y));
        float edgeFade = saturate(10.0f * edgeDistance);
        reflectionColor = lerp(reflectionColor, GetSceneColor(hitPixel).rgb, edgeFade);
    }

    // Physically based reflections:
    float3 albedo = gbufferAlbedoTexture.Load(int3(pixel, 0)).rgb;
    float4 surfaceProperties = gbufferSurfacePropertiesTexture.Load(int3(pixel, 0));
    float metallicity = surfaceProperties[DEFERRED_SURFACE_PROPERTIES_METALLICITY_CHANNEL];
    float3 reflectivity = lerp(float3(0.04f, 0.04f, 0.04f), saturate(albedo), saturate(metallicity));
    float nDotV = saturate(dot(worldNormal, -cameraRayDirection));
    float3 fresnel = reflectivity + (1.0f - reflectivity) * pow(1.0f - nDotV, 5.0f);
    SetSceneColor(pixel, float4(lerp(sourceColor.rgb, reflectionColor, fresnel), sourceColor.a));
}