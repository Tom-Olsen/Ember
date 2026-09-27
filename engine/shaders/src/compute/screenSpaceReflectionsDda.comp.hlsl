#define EMBER_SCENE_COLOR_ACCESS EMBER_SCENE_COLOR_ACCESS_OUT_OF_PLACE
#include "computeShaderCommon.hlsli"
#include "deferredRenderingConstants.h"
#include "globalSet.hlsli"



TextureCube<float4> environmentMap : register(t100, CALL_SET);



// Screen-space trace quality and bounded crossing searches:
static const uint maxStepCount = 64;
static const uint refinementStepCount = 8;
static const uint silhouetteSearchStepCount = 4;
static const float preferredPixelStride = 1.0f;
static const float minimumClipW = 0.0001f;
static const float minimumSurfaceThickness = 0.05f;
static const float relativeSurfaceThickness = 0.005f;



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



// Screen-space DDA:
// Intersect one homogeneous frustum half-space, whose inside satisfies value >= 0.
bool ClipRayToPlane(float startValue, float directionValue, inout float entryDistance, inout float exitDistance)
{
    if (directionValue == 0.0f)
        return startValue >= 0.0f;

    float planeDistance = -startValue / directionValue;
    if (directionValue > 0.0f)
        entryDistance = max(entryDistance, planeDistance);
    else
        exitDistance = min(exitDistance, planeDistance);
    return entryDistance < exitDistance;
}
// Clip the world ray to the actual camera volume, including the near/far planes and positive clip W.
bool TryGetVisibleRaySegment(float3 rayOrigin, float3 rayDirection, out float entryDistance, out float exitDistance)
{
    float4 startClip = mul(camera_worldToClipMatrix, float4(rayOrigin, 1.0f));
    float4 directionClip = mul(camera_worldToClipMatrix, float4(rayDirection, 0.0f));
    entryDistance = 0.0f;
    exitDistance = 1.0e30f;

    return ClipRayToPlane(startClip.w - minimumClipW, directionClip.w, entryDistance, exitDistance)
        && ClipRayToPlane(startClip.x + startClip.w, directionClip.x + directionClip.w, entryDistance, exitDistance)
        && ClipRayToPlane(startClip.w - startClip.x, directionClip.w - directionClip.x, entryDistance, exitDistance)
        && ClipRayToPlane(startClip.y + startClip.w, directionClip.y + directionClip.w, entryDistance, exitDistance)
        && ClipRayToPlane(startClip.w - startClip.y, directionClip.w - directionClip.y, entryDistance, exitDistance)
        && ClipRayToPlane(startClip.z, directionClip.z, entryDistance, exitDistance)
        && ClipRayToPlane(startClip.w - startClip.z, directionClip.w - directionClip.z, entryDistance, exitDistance)
        && exitDistance > entryDistance;
}
// Sample an already clipped screen-space segment. Sky is distinct from leaving the clipped segment.
bool TrySampleRay(float segmentFraction, float2 startPixel, float2 endPixel, float3 startQ, float3 endQ,
                  float startK, float endK, float startDistanceK, float endDistanceK, uint2 textureSize,
                  out uint2 samplePixel, out float2 sampleUv, out float rayViewDepth, out float rayDistance,
                  out float depthDelta, out float sceneViewDepth)
{
    float2 screenPosition = lerp(startPixel, endPixel, segmentFraction);
    screenPosition = clamp(screenPosition, 0.0f, float2(textureSize) - 0.5f);
    samplePixel = uint2(screenPosition);
    sampleUv = screenPosition / float2(textureSize);

    float k = lerp(startK, endK, segmentFraction);
    float3 q = lerp(startQ, endQ, segmentFraction);
    rayViewDepth = -q.z / k;
    rayDistance = lerp(startDistanceK, endDistanceK, segmentFraction) / k;
    depthDelta = 0.0f;
    sceneViewDepth = 0.0f;

    float sceneDepth = GetSceneNdcDepth(samplePixel);
    if (sceneDepth >= 1.0f)
        return false;

    sceneViewDepth = Camera_GetDepth(GetWorldPosition(samplePixel, sceneDepth));
    depthDelta = rayViewDepth - sceneViewDepth;
    return true;
}
// Binary refinement requires a valid front geometry sample and a valid back geometry sample.
bool TryRefineCrossing(float frontFraction, float backFraction, float2 startPixel, float2 endPixel,
                       float3 startQ, float3 endQ, float startK, float endK,
                       float startDistanceK, float endDistanceK, uint2 textureSize, uint2 sourcePixel,
                       out uint2 hitPixel, out float2 hitUv, out float hitDistance)
{
    hitPixel = 0;
    hitUv = 0.0f;
    hitDistance = 0.0f;

    for (uint refinementIndex = 0; refinementIndex < refinementStepCount; refinementIndex++)
    {
        float midpoint = 0.5f * (frontFraction + backFraction);
        uint2 samplePixel;
        float2 sampleUv;
        float rayViewDepth;
        float rayDistance;
        float depthDelta;
        float sceneViewDepth;
        if (!TrySampleRay(midpoint, startPixel, endPixel, startQ, endQ, startK, endK,
                          startDistanceK, endDistanceK, textureSize, samplePixel, sampleUv,
                          rayViewDepth, rayDistance, depthDelta, sceneViewDepth))
            return false;

        if (depthDelta < 0.0f)
            frontFraction = midpoint;
        else
            backFraction = midpoint;
    }

    uint2 samplePixel;
    float2 sampleUv;
    float rayViewDepth;
    float rayDistance;
    float depthDelta;
    float sceneViewDepth;
    if (!TrySampleRay(backFraction, startPixel, endPixel, startQ, endQ, startK, endK,
                      startDistanceK, endDistanceK, textureSize, samplePixel, sampleUv,
                      rayViewDepth, rayDistance, depthDelta, sceneViewDepth))
        return false;

    int2 pixelOffset = int2(samplePixel) - int2(sourcePixel);
    float thickness = max(minimumSurfaceThickness, relativeSurfaceThickness * sceneViewDepth);
    if (depthDelta < 0.0f || depthDelta > thickness || !any(abs(pixelOffset) > 1))
        return false;

    hitPixel = samplePixel;
    hitUv = sampleUv;
    hitDistance = rayDistance;
    return true;
}
// Find a front-to-back bracket skipped just before a geometry-to-sky transition.
bool TrySearchSilhouette(float frontFraction, float skyFraction, float2 startPixel, float2 endPixel,
                         float3 startQ, float3 endQ, float startK, float endK,
                         float startDistanceK, float endDistanceK, uint2 textureSize, uint2 sourcePixel,
                         out uint2 hitPixel, out float2 hitUv, out float hitDistance)
{
    hitPixel = 0;
    hitUv = 0.0f;
    hitDistance = 0.0f;
    for (uint searchIndex = 1; searchIndex < silhouetteSearchStepCount; searchIndex++)
    {
        float sampleFraction = lerp(frontFraction, skyFraction, float(searchIndex) / float(silhouetteSearchStepCount));
        uint2 samplePixel;
        float2 sampleUv;
        float rayViewDepth;
        float rayDistance;
        float depthDelta;
        float sceneViewDepth;
        if (!TrySampleRay(sampleFraction, startPixel, endPixel, startQ, endQ, startK, endK,
                          startDistanceK, endDistanceK, textureSize, samplePixel, sampleUv,
                          rayViewDepth, rayDistance, depthDelta, sceneViewDepth))
            return false; // Sky cannot form either side of a crossing bracket.

        int2 pixelOffset = int2(samplePixel) - int2(sourcePixel);
        if (depthDelta >= 0.0f && any(abs(pixelOffset) > 1))
        {
            float frontRayViewDepth = -lerp(startQ.z, endQ.z, frontFraction)
                                    / lerp(startK, endK, frontFraction);
            if (sceneViewDepth >= min(frontRayViewDepth, rayViewDepth) &&
                sceneViewDepth <= max(frontRayViewDepth, rayViewDepth) &&
                TryRefineCrossing(frontFraction, sampleFraction, startPixel, endPixel, startQ, endQ,
                                  startK, endK, startDistanceK, endDistanceK, textureSize, sourcePixel,
                                  hitPixel, hitUv, hitDistance))
                return true;
        }
        if (depthDelta < 0.0f)
            frontFraction = sampleFraction;
    }
    return false;
}
// Find reflection hit by walking the dominant projected pixel axis with a bounded stride.
bool ScreenSpaceRayMarch(uint2 sourcePixel, float3 rayOrigin, float3 rayDirection, out uint2 hitPixel, out float2 hitUv, out float hitDistance)
{
    hitPixel = 0;
    hitUv = 0.0f;
    hitDistance = 0.0f;

    float entryDistance;
    float exitDistance;
    if (!TryGetVisibleRaySegment(rayOrigin, rayDirection, entryDistance, exitDistance))
        return false;

    float3 startWorld = rayOrigin + entryDistance * rayDirection;
    float3 endWorld = rayOrigin + exitDistance * rayDirection;
    float4 startClip = mul(camera_worldToClipMatrix, float4(startWorld, 1.0f));
    float4 endClip = mul(camera_worldToClipMatrix, float4(endWorld, 1.0f));
    if (startClip.w < 0.5f * minimumClipW || endClip.w < 0.5f * minimumClipW)
        return false;

    uint width;
    uint height;
    sceneDepthTexture.GetDimensions(width, height);
    uint2 textureSize = uint2(width, height);
    float2 startPixel = (0.5f * startClip.xy / startClip.w + 0.5f) * float2(textureSize);
    float2 endPixel = (0.5f * endClip.xy / endClip.w + 0.5f) * float2(textureSize);
    float2 pixelDelta = endPixel - startPixel;
    bool permuteAxes = abs(pixelDelta.y) > abs(pixelDelta.x);
    float2 ddaStart = permuteAxes ? startPixel.yx : startPixel;
    float2 ddaEnd = permuteAxes ? endPixel.yx : endPixel;
    float projectedRayLength = abs(ddaEnd.x - ddaStart.x);
    if (projectedRayLength < 0.0001f)
        return false;

    // Q = viewPosition / clip.w and k = 1 / clip.w. Q / k restores view position
    // after linear interpolation in screen space; the same weights restore ray distance.
    float startK = 1.0f / startClip.w;
    float endK = 1.0f / endClip.w;
    float3 startQ = mul(camera_viewMatrix, float4(startWorld, 1.0f)).xyz * startK;
    float3 endQ = mul(camera_viewMatrix, float4(endWorld, 1.0f)).xyz * endK;
    float startDistanceK = entryDistance * startK;
    float endDistanceK = exitDistance * endK;

    // Long projected rays increase their stride so the primary-axis walk uses at most 64 steps.
    float pixelStride = max(preferredPixelStride, projectedRayLength / float(maxStepCount));
    uint stepCount = min(maxStepCount, (uint)ceil(projectedRayLength / pixelStride));
    float primaryDirection = ddaEnd.x >= ddaStart.x ? 1.0f : -1.0f;
    float previousFraction = 0.0f;
    bool previousSampleWasInFront = false;

    for (uint stepIndex = 1; stepIndex <= stepCount; stepIndex++)
    {
        float primaryPosition = stepIndex == stepCount ? ddaEnd.x :
                                ddaStart.x + primaryDirection * float(stepIndex) * pixelStride;
        float sampleFraction = saturate((primaryPosition - ddaStart.x) / (ddaEnd.x - ddaStart.x));
        uint2 samplePixel;
        float2 sampleUv;
        float rayViewDepth;
        float rayDistance;
        float depthDelta;
        float sceneViewDepth;
        bool hasGeometry = TrySampleRay(sampleFraction, startPixel, endPixel, startQ, endQ,
                                        startK, endK, startDistanceK, endDistanceK, textureSize,
                                        samplePixel, sampleUv, rayViewDepth, rayDistance, depthDelta, sceneViewDepth);
        if (!hasGeometry)
        {
            if (previousSampleWasInFront &&
                TrySearchSilhouette(previousFraction, sampleFraction, startPixel, endPixel, startQ, endQ,
                                    startK, endK, startDistanceK, endDistanceK, textureSize, sourcePixel,
                                    hitPixel, hitUv, hitDistance))
                return true;
            previousFraction = sampleFraction;
            previousSampleWasInFront = false;
            continue;
        }

        int2 pixelOffset = int2(samplePixel) - int2(sourcePixel);
        bool isOutsideSourceNeighborhood = any(abs(pixelOffset) > 1);
        float previousRayViewDepth = -lerp(startQ.z, endQ.z, previousFraction)
                                   / lerp(startK, endK, previousFraction);
        float rayMinimumViewDepth = min(previousRayViewDepth, rayViewDepth);
        float rayMaximumViewDepth = max(previousRayViewDepth, rayViewDepth);
        float thickness = max(minimumSurfaceThickness, relativeSurfaceThickness * sceneViewDepth);
        bool intersectsSceneDepth = rayMaximumViewDepth >= sceneViewDepth &&
                                    rayMinimumViewDepth <= sceneViewDepth + thickness;
        if (intersectsSceneDepth && isOutsideSourceNeighborhood)
        {
            // Preserve sub-pixel refinement for a strict front-to-back crossing. The interval
            // overlap itself remains a valid DDA hit if quantized depth samples prevent refinement.
            if (previousSampleWasInFront && depthDelta >= 0.0f &&
                TryRefineCrossing(previousFraction, sampleFraction, startPixel, endPixel, startQ, endQ,
                                  startK, endK, startDistanceK, endDistanceK, textureSize, sourcePixel,
                                  hitPixel, hitUv, hitDistance))
                return true;

            hitPixel = samplePixel;
            hitUv = sampleUv;
            hitDistance = rayDistance;
            return true;
        }

        previousFraction = sampleFraction;
        previousSampleWasInFront = depthDelta < 0.0f && isOutsideSourceNeighborhood;
    }
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
    float3 reflectionDirection = normalize(reflect(cameraRayDirection, worldNormal));
    float surfaceViewDepth = Camera_GetDepth(worldPosition);
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