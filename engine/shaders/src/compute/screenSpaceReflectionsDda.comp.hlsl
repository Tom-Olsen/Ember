#define EMBER_SCENE_COLOR_ACCESS EMBER_SCENE_COLOR_ACCESS_OUT_OF_PLACE
#include "computeShaderCommon.hlsli"
#include "deferredRenderingConstants.h"
#include "globalSet.hlsli"



TextureCube<float4> environmentMap : register(t100, CALL_SET);



// Ray march budget and binary-search iterations after a depth crossing:
static const uint maxStepCount = 64;
static const uint refinementStepCount = 8;



// Getters:
// Converts pixel+sceneDepth to world position:
float3 GetWorldPosition(uint2 pixel, float sceneDepth)
{
    uint width;
    uint height;
    sceneDepthTexture.GetDimensions(width, height);
    float2 uv = (float2(pixel) + 0.5f) / float2(width, height);
    float4 clipPosition = float4(2.0f * uv - 1.0f, sceneDepth, 1.0f);
    float4 worldPosition = mul(camera_clipToWorldMatrix, clipPosition);
    return worldPosition.xyz / worldPosition.w;
}
// Get world position of drawn geometry, or fails:
bool TryGetWorldPosition(uint2 pixel, out float3 worldPosition)
{
    float sceneDepth = GetSceneDepth(pixel);
    if (sceneDepth >= 1.0f)
    {
        worldPosition = 0.0f;
        return false;
    }
    worldPosition = GetWorldPosition(pixel, sceneDepth);
    return true;
}
// Depth of worldPosition in view space:
float GetViewSpaceDepth(float3 worldPosition)
{
    return -mul(camera_viewMatrix, float4(worldPosition, 1.0f)).z;
}
// Direction of ray from camera to worldPosition:
float3 GetCameraRayDirection(float3 worldPosition)
{
    bool isPerspective = abs(camera_projMatrix[3][3]) < 0.5f;
    return isPerspective ? normalize(worldPosition - camera_position.xyz) : Camera_GetForward();
}
// Environment color in worldDirection:
float3 GetEnvironmentColor(float3 worldDirection)
{
    float3 cubeDirection = mul(LinAlg_RotateX3x3(-math_PI_2), worldDirection);
    return environmentMap.SampleLevel(colorSampler, cubeDirection, 0.0f).rgb;
}



// Screen space ray marching:
// Project world position to screen. Fails if world position is not in screen space:
bool TryProjectWorldToScreen(float3 worldPosition, out float2 uv, out uint2 pixel, out float viewDepth)
{
	// Reject positions behind camera:
    float4 clipPosition = mul(camera_worldToClipMatrix, float4(worldPosition, 1.0f));
    if (clipPosition.w <= 0.0f)
    {
        uv = 0.0f;
        pixel = 0;
        viewDepth = 0.0f;
        return false;
    }

	// Reject positions outside camera frustum:
    float3 ndcPosition = clipPosition.xyz / clipPosition.w;
    uv = 0.5f * ndcPosition.xy + 0.5f;
    if (ndcPosition.z < 0.0f || ndcPosition.z > 1.0f || any(uv < 0.0f) || any(uv >= 1.0f))
    {
        pixel = 0;
        viewDepth = 0.0f;
        return false;
    }

	// Extract screen space pixel and view depth:
    uint width;
    uint height;
    sceneDepthTexture.GetDimensions(width, height);
    pixel = uint2(uv * float2(width, height));
    viewDepth = GetViewSpaceDepth(worldPosition);
    return true;
}
// Depth delta between worldPosition and scene geometry at the same pixel; also returns the scene depth in view space:
bool TryGetSceneDepthDelta(float3 worldPosition, out float2 uv, out uint2 pixel, out float depthDelta, out float sceneViewDepth)
{
	// Project 3d world position onto 2.5d scene geometry:
    float viewDepth;
    if (!TryProjectWorldToScreen(worldPosition, uv, pixel, viewDepth))
    {
        depthDelta = 0.0f;
        sceneViewDepth = 0.0f;
        return false;
    }

	// Reject pixels with no geometry:
    float sceneDepth = GetSceneDepth(pixel);
    if (sceneDepth >= 1.0f)
    {
        depthDelta = 0.0f;
        sceneViewDepth = 0.0f;
        return false;
    }

	// Compute depth delta between worldPosition and scene geometry:
    sceneViewDepth = GetViewSpaceDepth(GetWorldPosition(pixel, sceneDepth));
    depthDelta = viewDepth - sceneViewDepth;
    return true;
}
// Find reflection hit by marching the reflected ray through screen space.
// Refines front-to-back depth crossings and rejects crossings outside surface thickness:
bool ScreenSpaceRayMarch(uint2 sourcePixel, float3 rayOrigin, float3 rayDirection, out uint2 hitPixel, out float2 hitUv, out float hitDistance)
{
	// Limit reflection trace distance:
    float maxDistance = Camera_GetFarClip(); // heuristic; should be refined later.
    float3 previousRayPosition = rayOrigin;
    bool previousSampleWasInFront = false;

	// March reflected ray with increasing step distances:
    for (uint stepIndex = 1; stepIndex <= maxStepCount; stepIndex++)
    {
		// Quadratic spacing gives more samples close to the reflecting surface:
        float stepFraction = float(stepIndex) / float(maxStepCount);
        float travelDistance = maxDistance * stepFraction * stepFraction;
        float3 rayPosition = rayOrigin + travelDistance * rayDirection;

		// Compare ray against scene geometry:
        float2 sampleUv;
        uint2 samplePixel;
        float depthDelta;
        float sceneViewDepth;
        if (!TryGetSceneDepthDelta(rayPosition, sampleUv, samplePixel, depthDelta, sceneViewDepth))
        {
            previousRayPosition = rayPosition;
            previousSampleWasInFront = false;
            continue;	// keep marching ray forword.
        }

		// Reject self-reflections around the ray origin:
        int2 pixelOffset = int2(samplePixel) - int2(sourcePixel);
        bool isOutsideSourceNeighborhood = any(abs(pixelOffset) > 1);

		// Front-to-back depth crossing indicates a possible intersection:
        if (isOutsideSourceNeighborhood && previousSampleWasInFront && depthDelta >= 0.0f)
        {
			// Refine depth crossing by binary search:
            float3 frontPosition = previousRayPosition;
            float3 backPosition = rayPosition;
            for (uint refinementIndex = 0; refinementIndex < refinementStepCount; refinementIndex++)
            {
                float3 midpoint = 0.5f * (frontPosition + backPosition);
                float2 midpointUv;
                uint2 midpointPixel;
                float midpointDepthDelta;
                float midpointSceneViewDepth;
                if (!TryGetSceneDepthDelta(midpoint, midpointUv, midpointPixel, midpointDepthDelta, midpointSceneViewDepth) || midpointDepthDelta < 0.0f)
                    frontPosition = midpoint;
                else
                    backPosition = midpoint;
            }
			
			// Re-evaluate refined intersection:
            float3 refinedRayPosition = backPosition;
            if (!TryGetSceneDepthDelta(refinedRayPosition, sampleUv, samplePixel, depthDelta, sceneViewDepth))
            {
                hitPixel = 0;
                hitUv = 0.0f;
                hitDistance = 0.0f;
                return false;
            }

            // Reject crossings that lie too far behind the visible surface:
            float thickness = max(0.05f, 0.005f * sceneViewDepth);
            if (depthDelta <= thickness)
            {
                hitPixel = samplePixel;
                hitUv = sampleUv;
                hitDistance = length(refinedRayPosition - rayOrigin);
                return true;
            }
        }

        // Store current sample for detecting the next depth crossing:
        previousRayPosition = rayPosition;
        previousSampleWasInFront = depthDelta < 0.0f;
    }

	// No hit detected:
    hitPixel = 0;
    hitUv = 0.0f;
    hitDistance = 0.0f;
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
    float3 cameraRayDirection = GetCameraRayDirection(worldPosition);
    float3 reflectionDirection = normalize(reflect(cameraRayDirection, worldNormal));
    float surfaceViewDepth = GetViewSpaceDepth(worldPosition);
    float originBias = max(0.02f, 0.0005f * surfaceViewDepth);
    float3 rayOrigin = worldPosition + originBias * worldNormal;

	// Screen space ray marching:
    uint2 hitPixel;
    float2 hitUv;
    float hitDistance;
    bool hasHit = ScreenSpaceRayMarch(pixel, rayOrigin, reflectionDirection, hitPixel, hitUv, hitDistance);

    // Reflect skybox when ray hit misses:
    float3 reflectionColor = GetEnvironmentColor(reflectionDirection);

	// Fade reflections in near screen boundaries to hide cut off broken reflections:
    if (hasHit)
    {
        float edgeDistance = min(min(hitUv.x, 1.0f - hitUv.x), min(hitUv.y, 1.0f - hitUv.y));
        float edgeFade = saturate(10.0f * edgeDistance);
        reflectionColor = lerp(reflectionColor, GetSceneColor(hitPixel).rgb, edgeFade);
    }

    // Match the deferred metallic-roughness model:
    float3 albedo = gbufferAlbedoTexture.Load(int3(pixel, 0)).rgb;
    float4 surfaceProperties = gbufferSurfacePropertiesTexture.Load(int3(pixel, 0));
    float metallicity = surfaceProperties[DEFERRED_SURFACE_PROPERTIES_METALLICITY_CHANNEL];
    float3 reflectivity = lerp(float3(0.04f, 0.04f, 0.04f), saturate(albedo), saturate(metallicity));
    float nDotV = saturate(dot(worldNormal, -cameraRayDirection));
    float3 fresnel = reflectivity + (1.0f - reflectivity) * pow(1.0f - nDotV, 5.0f);
    SetSceneColor(pixel, float4(lerp(sourceColor.rgb, reflectionColor, fresnel), sourceColor.a));
}