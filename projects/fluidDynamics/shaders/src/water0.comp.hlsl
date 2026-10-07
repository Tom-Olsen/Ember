#define EMBER_SCENE_COLOR_ACCESS EMBER_SCENE_COLOR_ACCESS_OUT_OF_PLACE
#include "computeShaderCommon.hlsli"



// Bindings:
cbuffer CallValues : register(b300, CALL_SET)
{
	float4x4 worldToFluidMatrix;
	float3 fluidBoundsMin;
	float3 fluidBoundsMax;
	float surfaceDensity;
	float densityRayStepLength;
	float surfaceBias;
	float fluidIndexOfRefraction;
	float3 absorption;
	float normalSampleDistance;
	float sceneRayStepLength;
	float sceneRayMaxDistance;
	float sceneSurfaceThickness;
	float environmentMipLevel;
};
Texture3D<float> densityTexture : register(t100, CALL_SET);
TextureCube<float4> environmentMap : register(t101, CALL_SET);



// Ray march settings:
static const uint maxDensityStepCount = 256;
static const uint maxDensityRefinementStepCount = 8;
static const uint maxSceneStepCount = 128;
static const uint maxSceneRefinementStepCount = 8;
static const uint maxInternalReflectionCount = 2;
static const uint sceneSampleOutsideScreen = 0;
static const uint sceneSampleWithoutGeometry = 1;
static const uint sceneSampleValid = 2;
static const uint densityCrossingAny = 0;
static const uint densityCrossingEntry = 1;
static const uint densityCrossingExit = 2;



// Structs:
struct SurfaceHit
{
	float distance;
	float3 position;
	float3 normal;
	bool isEntering;
};
struct SceneRaySample
{
	float distance;
	float depthDelta;
	float sceneViewDepth;
	float2 uv;
};



// Small helpers:
float3 GetEnvironmentColor(float3 worldDirection)
{
	float3 cubeDirection = mul(mathLinAlg_RotateX3x3(-math_PI_2), worldDirection);
	return environmentMap.SampleLevel(colorSampler, cubeDirection, max(environmentMipLevel, 0.0f)).rgb;
}
float SampleDensityFluid(float3 fluidPosition)
{
	if (any(fluidPosition < 0.0f) || any(fluidPosition > 1.0f))
		return 0.0f;
	return densityTexture.SampleLevel(colorSamplerClampEdge, fluidPosition, 0.0f).r;
}
float SampleDensityWorld(float3 worldPosition)
{
	float3 boundsPosition = mul(worldToFluidMatrix, float4(worldPosition, 1.0f)).xyz;
	float3 fluidPosition = (boundsPosition - fluidBoundsMin) / (fluidBoundsMax - fluidBoundsMin);
	return SampleDensityFluid(fluidPosition);
}
// Sample the interior side of a known bounds boundary despite coordinate rounding:
float SampleDensityWorldAtBounds(float3 worldPosition)
{
	float3 boundsPosition = mul(worldToFluidMatrix, float4(worldPosition, 1.0f)).xyz;
	float3 fluidPosition = (boundsPosition - fluidBoundsMin) / (fluidBoundsMax - fluidBoundsMin);
	return SampleDensityFluid(saturate(fluidPosition));
}
float3 GetFacingNormal(float3 incidentDirection, float3 outwardNormal)
{
	return dot(incidentDirection, outwardNormal) < 0.0f ? outwardNormal : -outwardNormal;
}
float GetFresnel(float3 incidentDirection, float3 facingNormal, float sourceIor, float destinationIor)
{
	float r0 = (sourceIor - destinationIor) / (sourceIor + destinationIor);
	r0 *= r0;
	float cosTheta = saturate(dot(-incidentDirection, facingNormal));
	return r0 + (1.0f - r0) * pow(1.0f - cosTheta, 5.0f);
}



// Fluid bounds and density helpers:
float3 GetFluidBoundsNormal(float3 worldPosition)
{
	// Find the closest face in fluid bounds space:
	float3 boundsPosition = mul(worldToFluidMatrix, float4(worldPosition, 1.0f)).xyz;
	float3 minFaceDistances = abs(boundsPosition - fluidBoundsMin);
	float3 maxFaceDistances = abs(boundsPosition - fluidBoundsMax);
	float closestDistance = asfloat(0x7f800000u);
	float3 boundsNormal = 0.0f;
	for (uint axis = 0; axis < 3; axis++)
	{
		if (minFaceDistances[axis] < closestDistance)
		{
			closestDistance = minFaceDistances[axis];
			boundsNormal = 0.0f;
			boundsNormal[axis] = -1.0f;
		}
		if (maxFaceDistances[axis] < closestDistance)
		{
			closestDistance = maxFaceDistances[axis];
			boundsNormal = 0.0f;
			boundsNormal[axis] = 1.0f;
		}
	}

	// Transform the outward face normal to world space, including nonuniform scale:
	return normalize(mul(transpose((float3x3)worldToFluidMatrix), boundsNormal));
}
float3 GetDensityNormal(float3 worldPosition, float3 fallbackNormal)
{
	uint textureWidth;
	uint textureHeight;
	uint textureDepth;
	densityTexture.GetDimensions(textureWidth, textureHeight, textureDepth);

	float sampleDistance = max(normalSampleDistance, 1.0f);
	float3 texelSize = sampleDistance / float3(textureWidth, textureHeight, textureDepth);
	float3 boundsPosition = mul(worldToFluidMatrix, float4(worldPosition, 1.0f)).xyz;
	float3 fluidPosition = (boundsPosition - fluidBoundsMin) / (fluidBoundsMax - fluidBoundsMin);
	float negativeXDensity = SampleDensityFluid(fluidPosition - float3(texelSize.x, 0.0f, 0.0f));
	float positiveXDensity = SampleDensityFluid(fluidPosition + float3(texelSize.x, 0.0f, 0.0f));
	float negativeYDensity = SampleDensityFluid(fluidPosition - float3(0.0f, texelSize.y, 0.0f));
	float positiveYDensity = SampleDensityFluid(fluidPosition + float3(0.0f, texelSize.y, 0.0f));
	float negativeZDensity = SampleDensityFluid(fluidPosition - float3(0.0f, 0.0f, texelSize.z));
	float positiveZDensity = SampleDensityFluid(fluidPosition + float3(0.0f, 0.0f, texelSize.z));
	float3 gradientFluid = float3(
		positiveXDensity - negativeXDensity,
		positiveYDensity - negativeYDensity,
		positiveZDensity - negativeZDensity) / (2.0f * texelSize);

	// Density rises toward the liquid interior, so the outward normal is the
	// negative gradient. Convert the texture-space gradient to bounds space,
	// then transform by the transpose of the world-to-fluid linear transform.
	float3 gradientBounds = gradientFluid / (fluidBoundsMax - fluidBoundsMin);
	float3 gradientWorld = mul(transpose((float3x3)worldToFluidMatrix), gradientBounds);
	float gradientLengthSquared = dot(gradientWorld, gradientWorld);
	if (gradientLengthSquared < 1.0e-12f)
		return normalize(fallbackNormal);
	return -gradientWorld * rsqrt(gradientLengthSquared);
}
SurfaceHit RefineDensityHit(math_Ray worldRay, float frontDistance, float backDistance, bool frontInside)
{
	for (uint refinementIndex = 0; refinementIndex < maxDensityRefinementStepCount; refinementIndex++)
	{
		float midpointDistance = 0.5f * (frontDistance + backDistance);
		bool midpointInside = SampleDensityWorld(mathRay_GetPoint(worldRay, midpointDistance)) >= surfaceDensity;
		if (midpointInside == frontInside)
			frontDistance = midpointDistance;
		else
			backDistance = midpointDistance;
	}

	SurfaceHit hit;
	hit.distance = 0.5f * (frontDistance + backDistance);
	hit.position = mathRay_GetPoint(worldRay, hit.distance);
	hit.normal = GetDensityNormal(hit.position, -worldRay.direction);
	hit.isEntering = !frontInside;
	return hit;
}
bool TryFindFluidSurface(math_Ray worldRay, float startDistance, float endDistance, uint crossingType, bool detectInitialEntry, bool detectFinalExit, out SurfaceHit hit)
{
	// Clear hit:
	hit.distance = 0.0f;
	hit.position = 0.0f;
	hit.normal = 0.0f;
	hit.isEntering = false;

	// Reject empty search interval:
	float marchDistance = endDistance - startDistance;
	if (marchDistance <= 0.0f)
		return false;

	// Increase step length if needed to cover the interval within the step limit:
	float minimumStepLength = marchDistance / float(maxDensityStepCount);
	float stepLength = max(densityRayStepLength, minimumStepLength);
	uint stepCount = min((uint)ceil(marchDistance / stepLength), maxDensityStepCount);

	// Classify the starting sample as inside or outside the fluid:
	float previousDistance = startDistance;
	float3 previousPosition = mathRay_GetPoint(worldRay, previousDistance);
	float previousDensity = detectInitialEntry ? SampleDensityWorldAtBounds(previousPosition) : SampleDensityWorld(previousPosition);
	bool previousInside = previousDensity >= surfaceDensity;

	// Optionally treat an inside starting sample as an entry:
	if (detectInitialEntry && previousInside && crossingType != densityCrossingExit)
	{
		hit.distance = previousDistance;
		hit.position = mathRay_GetPoint(worldRay, hit.distance);
		hit.normal = GetFluidBoundsNormal(hit.position);
		hit.isEntering = true;
		return true;
	}

	// Search consecutive samples for the first accepted surface crossing:
	for (uint step = 1; step <= stepCount; step++)
	{
		// Sample the next position, clamped to the end of the interval:
		float currentDistance = step == stepCount ? endDistance : min(startDistance + float(step) * stepLength, endDistance);
		float3 currentPosition = mathRay_GetPoint(worldRay, currentDistance);

		// At a known bounds exit, sample its interior side instead of rounding outside:
		bool isBoundsExit = detectFinalExit && currentDistance == endDistance;
		float currentDensity = isBoundsExit ? SampleDensityWorldAtBounds(currentPosition) : SampleDensityWorld(currentPosition);
		bool currentInside = currentDensity >= surfaceDensity;

		// A change between inside and outside indicates a surface crossing:
		if (currentInside != previousInside)
		{
			// Check whether this entry or exit matches the requested crossing type:
			bool isEntering = currentInside;
			bool acceptsCrossing = crossingType == densityCrossingAny
				|| (crossingType == densityCrossingEntry && isEntering)
				|| (crossingType == densityCrossingExit && !isEntering);

			// Refine the crossing between these samples and return the surface hit:
			if (acceptsCrossing)
			{
				hit = RefineDensityHit(worldRay, previousDistance, currentDistance, previousInside);
				return true;
			}
		}

		// Retain this sample for comparison with the next one:
		previousDistance = currentDistance;
		previousInside = currentInside;
	}

	// Fluid reaching the bounds exits there even without a sampled density crossing:
	if (detectFinalExit && previousInside && crossingType != densityCrossingEntry)
	{
		hit.distance = endDistance;
		hit.position = mathRay_GetPoint(worldRay, hit.distance);
		hit.normal = GetFluidBoundsNormal(hit.position);
		hit.isEntering = false;
		return true;
	}

	// No accepted surface crossing was detected:
	return false;
}



// Scene depth ray march helpers:
uint EvaluateSceneRaySample(math_Ray worldRay, float distance, float2 screenSize, out SceneRaySample sample)
{
	sample.distance = distance;
	sample.depthDelta = 0.0f;
	sample.sceneViewDepth = 0.0f;
	sample.uv = 0.0f;

	float3 worldPosition = mathRay_GetPoint(worldRay, distance);
	float4 clipPosition = mul(camera_worldToClipMatrix, float4(worldPosition, 1.0f));
	if (clipPosition.w <= 0.0f)
		return sceneSampleOutsideScreen;

	float3 ndcPosition = clipPosition.xyz / clipPosition.w;
	float2 uv = 0.5f * ndcPosition.xy + 0.5f;
	if (any(uv < 0.0f) || any(uv >= 1.0f) || ndcPosition.z < 0.0f || ndcPosition.z >= 1.0f)
		return sceneSampleOutsideScreen;

	sample.uv = uv;
	uint2 pixel = min(uint2(uv * screenSize), uint2(screenSize) - 1);
	float sceneNdcDepth = Scene_GetNdcDepth(pixel);
	if (sceneNdcDepth >= 1.0f)
		return sceneSampleWithoutGeometry;

	float3 sceneWorldPosition = Camera_GetWorldPosition(pixel, sceneNdcDepth, screenSize);
	sample.sceneViewDepth = Camera_GetDepth(sceneWorldPosition);
	sample.depthDelta = Camera_GetDepth(worldPosition) - sample.sceneViewDepth;
	return sceneSampleValid;
}
bool TryRefineSceneHit(math_Ray worldRay, float2 screenSize, SceneRaySample frontSample, SceneRaySample backSample, out SceneRaySample hitSample)
{
	hitSample = backSample;
	for (uint refinementIndex = 0; refinementIndex < maxSceneRefinementStepCount; refinementIndex++)
	{
		float midpointDistance = 0.5f * (frontSample.distance + backSample.distance);
		SceneRaySample midpointSample;
		if (EvaluateSceneRaySample(worldRay, midpointDistance, screenSize, midpointSample) != sceneSampleValid)
			return false;

		if (midpointSample.depthDelta < 0.0f)
			frontSample = midpointSample;
		else
			backSample = midpointSample;
	}

	float thickness = max(sceneSurfaceThickness, 0.005f * backSample.sceneViewDepth);
	if (backSample.depthDelta < 0.0f || backSample.depthDelta > thickness)
		return false;
	hitSample = backSample;
	return true;
}
bool TryTraceScene(math_Ray worldRay, float2 screenSize, float maxDistance, out float3 sceneColor, out float hitDistance)
{
	sceneColor = 0.0f;
	hitDistance = 0.0f;
	if (maxDistance <= 0.0f)
		return false;

	float minimumStepLength = maxDistance / float(maxSceneStepCount);
	float stepLength = max(sceneRayStepLength, minimumStepLength);
	uint stepCount = min((uint)ceil(maxDistance / stepLength), maxSceneStepCount);

	SceneRaySample previousSample;
	bool hasPreviousSample = false;
	for (uint step = 0; step <= stepCount; step++)
	{
		float distance = min(float(step) * stepLength, maxDistance);
		SceneRaySample sample;
		uint status = EvaluateSceneRaySample(worldRay, distance, screenSize, sample);
		if (status == sceneSampleOutsideScreen)
			break;
		if (status == sceneSampleWithoutGeometry)
		{
			hasPreviousSample = false;
			continue;
		}

		if (hasPreviousSample && previousSample.depthDelta < 0.0f && sample.depthDelta >= 0.0f)
		{
			SceneRaySample hitSample;
			if (TryRefineSceneHit(worldRay, screenSize, previousSample, sample, hitSample))
			{
				sceneColor = Scene_SampleColor(hitSample.uv).rgb;
				hitDistance = hitSample.distance;
				return true;
			}
		}
		previousSample = sample;
		hasPreviousSample = true;
	}
	return false;
}
float3 TraceSceneOrEnvironment(float3 origin, float3 direction, float2 screenSize)
{
	math_Ray worldRay;
	worldRay.origin = origin + max(surfaceBias, 1.0e-4f) * direction;
	worldRay.direction = normalize(direction);

	float3 sceneColor;
	float hitDistance;
	float maxDistance = sceneRayMaxDistance > 0.0f ? sceneRayMaxDistance : Camera_GetFarClip();
	if (TryTraceScene(worldRay, screenSize, maxDistance, sceneColor, hitDistance))
		return sceneColor;
	return GetEnvironmentColor(worldRay.direction);
}



// Refraction helpers:
float3 TraceFromInsideFluid(float3 startPosition, float3 startDirection, float2 screenSize)
{
	float3 currentPosition = startPosition;
	float3 currentDirection = normalize(startDirection);
	float travelledInsideFluid = 0.0f;
	float rayBias = max(surfaceBias, 1.0e-4f);

	for (uint reflectionIndex = 0; reflectionIndex <= maxInternalReflectionCount; reflectionIndex++)
	{
		math_Ray internalWorldRay;
		internalWorldRay.origin = currentPosition + rayBias * currentDirection;
		internalWorldRay.direction = currentDirection;

		float boundsEnterDistance;
		float boundsExitDistance;
		if (!mathRay_TryIntersectRotatedBounds(internalWorldRay, worldToFluidMatrix, fluidBoundsMin, fluidBoundsMax, boundsEnterDistance, boundsExitDistance))
			break;

		SurfaceHit exitHit;
		float marchStart = max(boundsEnterDistance, 0.0f);
		if (!TryFindFluidSurface(internalWorldRay, marchStart, boundsExitDistance, densityCrossingExit, false, true, exitHit))
			break;

		// Opaque geometry can be embedded in or intersect the fluid volume. Test
		// the screen-space depth before processing the density exit interface.
		float3 sceneColor;
		float sceneHitDistance;
		if (TryTraceScene(internalWorldRay, screenSize, exitHit.distance, sceneColor, sceneHitDistance))
		{
			float3 sceneHitPosition = mathRay_GetPoint(internalWorldRay, sceneHitDistance);
			float totalDistance = travelledInsideFluid + distance(currentPosition, sceneHitPosition);
			float3 transmittance = exp(-max(absorption, 0.0f) * totalDistance);
			return transmittance * sceneColor;
		}

		travelledInsideFluid += distance(currentPosition, exitHit.position);
		float3 facingNormal = GetFacingNormal(currentDirection, exitHit.normal);
		float3 exitDirection = refract(currentDirection, facingNormal, fluidIndexOfRefraction);
		if (dot(exitDirection, exitDirection) > 1.0e-8f)
		{
			float3 transmittance = exp(-max(absorption, 0.0f) * travelledInsideFluid);
			return transmittance * TraceSceneOrEnvironment(exitHit.position, normalize(exitDirection), screenSize);
		}

		// Stop if another total internal reflection would exceed the limit:
		if (reflectionIndex == maxInternalReflectionCount)
			break;

		// Total internal reflection. Stay inside and search for the next exit.
		currentPosition = exitHit.position;
		currentDirection = normalize(reflect(currentDirection, facingNormal));
	}

	float3 transmittance = exp(-max(absorption, 0.0f) * travelledInsideFluid);
	return transmittance * GetEnvironmentColor(currentDirection);
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

	// Camera world ray (direction is normalized):
	math_Ray cameraWorldRay = Camera_GetRay(sourcePixel, screenSize);

	// Skip rays that do not hit the fluid bounds:
	float boundsEnterDistance;
	float boundsExitDistance;
	if (!mathRay_TryIntersectRotatedBounds(cameraWorldRay, worldToFluidMatrix, fluidBoundsMin, fluidBoundsMax, boundsEnterDistance, boundsExitDistance))
	{
		Scene_SetColor(sourcePixel, sourceColor);
		return;
	}

	// March denisty surface:
	float marchStart = max(boundsEnterDistance, 0.0f);
	float marchEnd = min(boundsExitDistance, Scene_GetDistance(sourcePixel, cameraWorldRay, screenSize, true));
	SurfaceHit primaryHit;
	bool rayEntersFluidBounds = boundsEnterDistance >= 0.0f;
	bool rayExitsFluidBounds = marchEnd == boundsExitDistance;
	if (!TryFindFluidSurface(cameraWorldRay, marchStart, marchEnd, densityCrossingAny, rayEntersFluidBounds, rayExitsFluidBounds, primaryHit))
	{
		Scene_SetColor(sourcePixel, sourceColor);
		return;
	}

	float sourceIor = primaryHit.isEntering ? 1.0f : fluidIndexOfRefraction;
	float destinationIor = primaryHit.isEntering ? fluidIndexOfRefraction : 1.0f;
	float3 facingNormal = GetFacingNormal(cameraWorldRay.direction, primaryHit.normal);
	float3 reflectionDirection = normalize(reflect(cameraWorldRay.direction, facingNormal));
	float3 refractionDirection = refract(cameraWorldRay.direction, facingNormal, sourceIor / destinationIor);

	float3 reflectionColor;
	if (primaryHit.isEntering)
		reflectionColor = TraceSceneOrEnvironment(primaryHit.position, reflectionDirection, screenSize);
	else
		reflectionColor = TraceFromInsideFluid(primaryHit.position, reflectionDirection, screenSize);

	float3 refractionColor = 0.0f;
	bool hasRefraction = dot(refractionDirection, refractionDirection) > 1.0e-8f;
	if (hasRefraction)
	{
		refractionDirection = normalize(refractionDirection);
		if (primaryHit.isEntering)
			refractionColor = TraceFromInsideFluid(primaryHit.position, refractionDirection, screenSize);
		else
			refractionColor = TraceSceneOrEnvironment(primaryHit.position, refractionDirection, screenSize);
	}

	float fresnel = hasRefraction
		? GetFresnel(cameraWorldRay.direction, facingNormal, sourceIor, destinationIor)
		: 1.0f;
	float3 fluidColor = lerp(refractionColor, reflectionColor, fresnel);

	// If the near plane starts inside the isosurface, both paths still travel
	// through the primary fluid segment before reaching the camera.
	if (!primaryHit.isEntering)
	{
		float primaryFluidDistance = max(primaryHit.distance - marchStart, 0.0f);
		fluidColor *= exp(-max(absorption, 0.0f) * primaryFluidDistance);
	}

	Scene_SetColor(sourcePixel, float4(fluidColor, sourceColor.a));
}