#ifndef __INCLUDE_GUARD_screenSpaceReflectionUtility_hlsli__
#define __INCLUDE_GUARD_screenSpaceReflectionUtility_hlsli__



// Ray march parameters:
// Keep maxStepCount / 2^maxRefinementStepCount == refinementPixelPrecision so a coarse bracket
// no larger than maxStepCount pixels can reach the target precision within the refinement budget.
static const uint maxStepCount = 64;
static const uint maxRefinementStepCount = 8;
static const float minimumPixelStepSize = 1.0f;
static const float refinementPixelPrecision = 0.25f;
static const float minimumSurfaceThickness = 0.05f;
static const float relativeSurfaceThickness = 0.005f;
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
	float3 origin;		// xy = pixel position, z = NDC depth [0,1].
	float3 direction;	// xy = pixel delta, z = NDC depth delta.
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
float3 GetWorldPosition(uint2 pixel, float ndcDepth, float2 screenSize)
{
	float3 screenPosition = float3(float2(pixel) + 0.5f, ndcDepth);
	return ScreenPositionToWorld(screenPosition, screenSize);
}
bool TryGetGeometryWorldPosition(uint2 pixel, float2 screenSize, out float3 worldPosition)
{
	float sceneDepth = GetSceneNdcDepth(pixel);
	if (sceneDepth >= 1.0f)
	{
		worldPosition = 0.0f;
		return false;
	}
	worldPosition = GetWorldPosition(pixel, sceneDepth, screenSize);
	return true;
}
float GetPixelStepSize(ScreenRay screenRay, float2 screenSize)
{
	float2 directionMagnitude = abs(screenRay.direction.xy);
	float2 distanceToEdge;
	distanceToEdge.x = screenRay.direction.x < 0.0f ? screenRay.origin.x : screenSize.x - screenRay.origin.x;
	distanceToEdge.y = screenRay.direction.y < 0.0f ? screenRay.origin.y : screenSize.y - screenRay.origin.y;
	distanceToEdge = max(distanceToEdge, 0.0f);

	float2 rayDistanceToEdge = 1.0e30f;
	if (directionMagnitude.x > 1.0e-4f)
		rayDistanceToEdge.x = distanceToEdge.x / directionMagnitude.x;
	if (directionMagnitude.y > 1.0e-4f)
		rayDistanceToEdge.y = distanceToEdge.y / directionMagnitude.y;

	float rayDistance = min(rayDistanceToEdge.x, rayDistanceToEdge.y);
	return max(minimumPixelStepSize, ceil(rayDistance / float(maxStepCount)));
}



// Big helpers:
ScreenRay ProjectRayToScreen(WorldRay worldRay, float2 screenSize)
{
	// Second point on the same world-space line:
	float3 rayPoint = worldRay.origin + worldRay.direction;

	// World -> Clip xy[-w, w], z[0,w]:
	float4 originClip = mul(camera_worldToClipMatrix, float4(worldRay.origin, 1.0f));
	float4 pointClip = mul(camera_worldToClipMatrix, float4(rayPoint, 1.0f));

	// Clip -> NDC xy[-1, 1], z[0,1]:
	float3 originNdc = originClip.xyz / originClip.w;
	float3 pointNdc = pointClip.xyz / pointClip.w;

	// NDC -> pixels [0, width/height]:
	float3 originScreen;
	originScreen.xy = (originNdc.xy * 0.5f + 0.5f) * screenSize;
	originScreen.z = originNdc.z;
	float3 pointScreen;
	pointScreen.xy = (pointNdc.xy * 0.5f + 0.5f) * screenSize;
	pointScreen.z = pointNdc.z;

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
	float3 rayWorldPosition = ScreenPositionToWorld(float3(pixelCenter, screenPosition.z), screenSize);
	float3 sceneWorldPosition = ScreenPositionToWorld(float3(pixelCenter, sceneNdcDepth), screenSize);
	float rayViewDepth = Camera_GetDepth(rayWorldPosition);
	raySample.sceneViewDepth = Camera_GetDepth(sceneWorldPosition);
	raySample.depthDelta = rayViewDepth - raySample.sceneViewDepth;
	return screenRaySampleValid;
}
bool TryRefineScreenRayHit(float2 screenSize, ScreenRaySample frontSample, ScreenRaySample backSample, out ScreenRaySample hitSample)
{
	hitSample.screenPosition = 0.0f;
	hitSample.sceneViewDepth = 0.0f;
	hitSample.depthDelta = 0.0f;

	// Refine front-to-back depth crossings:
	for (uint refinementIndex = 0; refinementIndex < maxRefinementStepCount; refinementIndex++)
	{
		// Exit early if refinementPixelPrecision is reached:
		float2 bracketSize = abs(backSample.screenPosition.xy - frontSample.screenPosition.xy);
		if (max(bracketSize.x, bracketSize.y) <= refinementPixelPrecision)
			break;

		float3 midpointPosition = 0.5f * (frontSample.screenPosition + backSample.screenPosition);
		ScreenRaySample midpointSample;
		uint sampleStatus = EvaluateScreenRaySample(midpointPosition, screenSize, midpointSample);
		if (sampleStatus != screenRaySampleValid)
			return false;

		if (midpointSample.depthDelta < 0.0f)
			frontSample = midpointSample;
		else
			backSample = midpointSample;
	}

	float surfaceThickness = max(minimumSurfaceThickness, relativeSurfaceThickness * backSample.sceneViewDepth);
	if (backSample.depthDelta < 0.0f || backSample.depthDelta > surfaceThickness)
		return false;

	hitSample = backSample;
	return true;
}



// Screen space ray marching:
bool ScreenSpaceRayMarch(uint2 sourcePixel, WorldRay worldRay, float2 screenSize, out uint2 hitPixel, out float2 hitUv)
{
	// Outputs:
	hitPixel = 0;
	hitUv = 0.0f;

	// Screen-space ray normalized to one pixel along its dominant axis:
	ScreenRay screenRay = ProjectRayToScreen(worldRay, screenSize);
	float maxDir = max(abs(screenRay.direction.x), abs(screenRay.direction.y));
	if (maxDir < 1e-4f)
		return false;
	screenRay.direction /= maxDir;
	float pixelStepSize = GetPixelStepSize(screenRay, screenSize);

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
		{
			hasPreviousSample = false;
			continue;
		}

		// Reject self-reflections around the ray origin:
		uint2 pixel = uint2(raySample.screenPosition.xy);
		int2 pixelOffset = int2(pixel) - int2(sourcePixel);
		bool isOutsideSourceNeighborhood = any(abs(pixelOffset) > 1);

		// Refine front-to-back depth crossings:
		if (isOutsideSourceNeighborhood && hasPreviousSample && previousSample.depthDelta < 0.0f && raySample.depthDelta >= 0.0f)
		{
			ScreenRaySample hitSample;
			if (TryRefineScreenRayHit(screenSize, previousSample, raySample, hitSample))
			{
				hitPixel = uint2(hitSample.screenPosition.xy);
				int2 hitPixelOffset = int2(hitPixel) - int2(sourcePixel);
				if (any(abs(hitPixelOffset) > 1)) // prevents self reflection.
				{
					hitUv = (float2(hitPixel) + 0.5f) / screenSize;
					return true;
				}
			}
		}
		previousSample = raySample;
		hasPreviousSample = true;
	}

	// No hit detected:
	return false;
}


#endif // __INCLUDE_GUARD_screenSpaceReflectionUtility_hlsli__