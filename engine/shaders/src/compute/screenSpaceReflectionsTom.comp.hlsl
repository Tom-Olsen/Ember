#define EMBER_SCENE_COLOR_ACCESS EMBER_SCENE_COLOR_ACCESS_OUT_OF_PLACE
#include "computeShaderCommon.hlsli"
#include "deferredRenderingConstants.h"
#include "globalSet.hlsli"



// Bindings:
TextureCube<float4> environmentMap : register(t100, CALL_SET);



// Ray march parameters:
static const uint maxStepCount = 64;
static const uint refinementStepCount = 8;
static const uint pixelStepSize = 10;
// Screen ray sample status:
static const uint screenRaySampleOutsideScreen = 0;
static const uint screenRaySampleWithoutGeometry = 1;
static const uint screenRaySampleValid = 2;



// Structs:
struct WorldRay
{
	float3 origin;
	float3 direction;
};
struct ScreenRay
{
    float3 origin;     // xy = pixel position, z = NDC depth [0,1]
    float3 direction;  // xy = pixel delta, z = NDC depth delta
};
struct ScreenRaySample
{
    float3 screenPosition;	// xy = pixel position, z = NDC depth.
    float sceneViewDepth;	// scene depth in view space.
    float depthDelta;		// sample depth minus scene depth in view space.
};



// Small helpers:
float3 ScreenPositionToWorld(float3 screenPosition, float2 screenSize)
{
    float2 uv = screenPosition.xy / screenSize;
    float4 clipPosition = float4(2.0f * uv - 1.0f, screenPosition.z, 1.0f);
    float4 worldPosition = mul(camera_clipToWorldMatrix, clipPosition);
    return worldPosition.xyz / worldPosition.w;
}
float3 GetWorldPosition(uint2 pixel, float ndcDepth)
{
	uint screenWidth;
    uint screenHeight;
	sceneDepthTexture.GetDimensions(screenWidth, screenHeight);
	float2 screenSize = float2(screenWidth, screenHeight);
    float3 screenPosition = float3(float2(pixel) + 0.5f, ndcDepth);
    return ScreenPositionToWorld(screenPosition, screenSize);
}
bool TryGetGeometryWorldPosition(uint2 pixel, out float3 worldPosition)
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
float3 GetEnvironmentColor(float3 worldDirection)
{
    float3 cubeDirection = mul(mathLinAlg_RotateX3x3(-math_PI_2), worldDirection);
    return environmentMap.SampleLevel(colorSampler, cubeDirection, 0.0f).rgb;
}



// Big helpers:
ScreenRay ProjectRayToScreen(WorldRay worldRay, float2 screenSize)
{
    // Second point on the same world-space line:
    float3 rayPoint = worldRay.origin + worldRay.direction;

    // World -> Clip xy[-w, w], z[0,w]:
    float4 originClip = mul(camera_worldToClipMatrix, float4(worldRay.origin, 1.0f));
    float4 pointClip  = mul(camera_worldToClipMatrix, float4(rayPoint,  1.0f));

    // Clip -> NDC xy[-1, 1], z[0,1]:
    float3 originNdc = originClip.xyz / originClip.w;
    float3 pointNdc  = pointClip.xyz  / pointClip.w;

    // NDC -> pixels [0, width/height]:
    float3 originScreen;
    originScreen.xy = (originNdc.xy * 0.5f + 0.5f) * screenSize;
    originScreen.z  = originNdc.z;
    float3 pointScreen;
    pointScreen.xy = (pointNdc.xy * 0.5f + 0.5f) * screenSize;
    pointScreen.z  = pointNdc.z;

    ScreenRay screenRay;
    screenRay.origin = originScreen;
    screenRay.direction = pointScreen - originScreen;
    return screenRay;
}
uint EvaluateScreenRaySample(float3 screenPosition, float2 screenSize, out ScreenRaySample raySample)
{
    raySample.screenPosition = screenPosition;
    raySample.sceneViewDepth = 0.0f;
    raySample.depthDelta = 0.0f;

    if (screenPosition.x < 0.0f || screenPosition.x >= screenSize.x ||
        screenPosition.y < 0.0f || screenPosition.y >= screenSize.y ||
        screenPosition.z < 0.0f || screenPosition.z >= 1.0f)
        return screenRaySampleOutsideScreen;

    uint2 pixel = uint2(screenPosition.xy);
    float sceneNdcDepth = GetSceneNdcDepth(pixel);
    if (sceneNdcDepth >= 1.0f)
        return screenRaySampleWithoutGeometry;

    float2 pixelCenter = float2(pixel) + 0.5f;
    float3 rayWorldPosition   = ScreenPositionToWorld(float3(pixelCenter, screenPosition.z), screenSize);
    float3 sceneWorldPosition = ScreenPositionToWorld(float3(pixelCenter, sceneNdcDepth), screenSize);
    float rayViewDepth = Camera_GetDepth(rayWorldPosition);
    raySample.sceneViewDepth = Camera_GetDepth(sceneWorldPosition);
    raySample.depthDelta = rayViewDepth - raySample.sceneViewDepth;
    return screenRaySampleValid;
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
	ScreenRay screenRay = ProjectRayToScreen(worldRay, screenSize);
	float maxDir = max(abs(screenRay.direction.x), abs(screenRay.direction.y));
	if (maxDir < 1e-4f)
		return false;
	screenRay.direction /= maxDir;

    ScreenRaySample previousSample;
    bool hasPreviousSample = false;
    for (uint step = 1; step <= maxStepCount; step++)
	{
		// Create screen ray sample:
		float3 screenPosition = screenRay.origin + step * pixelStepSize * screenRay.direction;
		ScreenRaySample raySample;
		uint sampleStatus = EvaluateScreenRaySample(screenPosition, screenSize, raySample);
		if (sampleStatus == screenRaySampleOutsideScreen)
			break;
		if (sampleStatus == screenRaySampleWithoutGeometry)
			continue;

		// Reject self-reflections around the ray origin:
		uint2 pixel = uint2(raySample.screenPosition.xy);
        int2 pixelOffset = int2(pixel) - int2(sourcePixel);
        bool isOutsideSourceNeighborhood = any(abs(pixelOffset) > 1);

		// Test:
		if (isOutsideSourceNeighborhood && hasPreviousSample && previousSample.depthDelta < 0.0f && raySample.depthDelta >= 0.0f)
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
    if (!TryGetGeometryWorldPosition(pixel, worldPosition))
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