#define EMBER_SCENE_COLOR_ACCESS EMBER_SCENE_COLOR_ACCESS_OUT_OF_PLACE
#include "computeShaderCommon.hlsli"



// The fluid density texture occupies [0, 1]^3. worldToFluidMatrix must include
// the component transform, rotated bounds transform, and bounds scale.
cbuffer CallValues : register(b300, CALL_SET)
{
	float4x4 worldToFluidMatrix;
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



// Bindings:
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
struct WorldRay
{
	float3 origin;
	float3 direction;
};
struct DensityHit
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
WorldRay GetCameraRay(uint2 pixel, float2 screenSize)
{
	float3 nearPosition = GetWorldPosition(pixel, 0.0f, screenSize);
	float3 farPosition = GetWorldPosition(pixel, 1.0f, screenSize);

	WorldRay ray;
	ray.origin = nearPosition;
	ray.direction = normalize(farPosition - nearPosition);
	return ray;
}
float GetSceneDistance(uint2 pixel, WorldRay ray, float2 screenSize)
{
	float sceneDepth = GetSceneNdcDepth(pixel);
	if (sceneDepth >= 1.0f)
		return 1.0e30f;

	float3 scenePosition = GetWorldPosition(pixel, sceneDepth, screenSize);
	return dot(scenePosition - ray.origin, ray.direction);
}
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
	float3 fluidPosition = mul(worldToFluidMatrix, float4(worldPosition, 1.0f)).xyz;
	return SampleDensityFluid(fluidPosition);
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
bool UpdateRayBoundsInterval(float origin, float direction, inout float enterDistance, inout float exitDistance)
{
	if (abs(direction) < 1.0e-7f)
		return origin >= 0.0f && origin <= 1.0f;

	float inverseDirection = 1.0f / direction;
	float distance0 = -origin * inverseDirection;
	float distance1 = (1.0f - origin) * inverseDirection;
	if (distance0 > distance1)
	{
		float temp = distance0;
		distance0 = distance1;
		distance1 = temp;
	}
	enterDistance = max(enterDistance, distance0);
	exitDistance = min(exitDistance, distance1);
	return enterDistance <= exitDistance;
}
bool RayFluidBoundsIntersection(WorldRay ray, out float enterDistance, out float exitDistance)
{
	float3 originFluid = mul(worldToFluidMatrix, float4(ray.origin, 1.0f)).xyz;
	float3 directionFluid = mul(worldToFluidMatrix, float4(ray.direction, 0.0f)).xyz;
	enterDistance = -1.0e30f;
	exitDistance = 1.0e30f;

	if (!UpdateRayBoundsInterval(originFluid.x, directionFluid.x, enterDistance, exitDistance))
		return false;
	if (!UpdateRayBoundsInterval(originFluid.y, directionFluid.y, enterDistance, exitDistance))
		return false;
	if (!UpdateRayBoundsInterval(originFluid.z, directionFluid.z, enterDistance, exitDistance))
		return false;
	return exitDistance >= max(enterDistance, 0.0f);
}
float3 GetDensityNormal(float3 worldPosition, float3 fallbackNormal)
{
	uint textureWidth;
	uint textureHeight;
	uint textureDepth;
	densityTexture.GetDimensions(textureWidth, textureHeight, textureDepth);

	float sampleDistance = max(normalSampleDistance, 1.0f);
	float3 texelSize = sampleDistance / float3(textureWidth, textureHeight, textureDepth);
	float3 fluidPosition = mul(worldToFluidMatrix, float4(worldPosition, 1.0f)).xyz;
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
	// negative gradient. A scalar-field gradient transforms by the transpose
	// of the world-to-fluid linear transform.
	float3 gradientWorld = mul(transpose((float3x3)worldToFluidMatrix), gradientFluid);
	float gradientLengthSquared = dot(gradientWorld, gradientWorld);
	if (gradientLengthSquared < 1.0e-12f)
		return normalize(fallbackNormal);
	return -gradientWorld * rsqrt(gradientLengthSquared);
}
DensityHit RefineDensityHit(WorldRay ray, float frontDistance, float backDistance, bool frontInside)
{
	for (uint refinementIndex = 0; refinementIndex < maxDensityRefinementStepCount; refinementIndex++)
	{
		float midpointDistance = 0.5f * (frontDistance + backDistance);
		bool midpointInside = SampleDensityWorld(ray.origin + midpointDistance * ray.direction) >= surfaceDensity;
		if (midpointInside == frontInside)
			frontDistance = midpointDistance;
		else
			backDistance = midpointDistance;
	}

	DensityHit hit;
	hit.distance = 0.5f * (frontDistance + backDistance);
	hit.position = ray.origin + hit.distance * ray.direction;
	hit.normal = GetDensityNormal(hit.position, -ray.direction);
	hit.isEntering = !frontInside;
	return hit;
}
bool TryMarchDensitySurface(WorldRay ray, float startDistance, float endDistance, uint crossingType, bool detectInitialEntry, out DensityHit hit)
{
	hit.distance = 0.0f;
	hit.position = 0.0f;
	hit.normal = 0.0f;
	hit.isEntering = false;

	float marchDistance = endDistance - startDistance;
	if (marchDistance <= 0.0f)
		return false;

	float minimumStepLength = marchDistance / float(maxDensityStepCount);
	float stepLength = max(densityRayStepLength, minimumStepLength);
	uint stepCount = min((uint)ceil(marchDistance / stepLength), maxDensityStepCount);
	float previousDistance = startDistance;
	bool previousInside = SampleDensityWorld(ray.origin + previousDistance * ray.direction) >= surfaceDensity;
	if (detectInitialEntry && previousInside && crossingType != densityCrossingExit)
	{
		hit.distance = previousDistance;
		hit.position = ray.origin + hit.distance * ray.direction;
		hit.normal = GetDensityNormal(hit.position, -ray.direction);
		hit.isEntering = true;
		return true;
	}

	for (uint step = 1; step <= stepCount; step++)
	{
		float currentDistance = min(startDistance + float(step) * stepLength, endDistance);
		bool currentInside = SampleDensityWorld(ray.origin + currentDistance * ray.direction) >= surfaceDensity;
		if (currentInside != previousInside)
		{
			bool isEntering = currentInside;
			bool acceptsCrossing = crossingType == densityCrossingAny
				|| (crossingType == densityCrossingEntry && isEntering)
				|| (crossingType == densityCrossingExit && !isEntering);
			if (acceptsCrossing)
			{
				hit = RefineDensityHit(ray, previousDistance, currentDistance, previousInside);
				return true;
			}
		}
		previousDistance = currentDistance;
		previousInside = currentInside;
	}
	return false;
}



// Scene depth ray march helpers:
uint EvaluateSceneRaySample(WorldRay ray, float distance, float2 screenSize, out SceneRaySample sample)
{
	sample.distance = distance;
	sample.depthDelta = 0.0f;
	sample.sceneViewDepth = 0.0f;
	sample.uv = 0.0f;

	float3 worldPosition = ray.origin + distance * ray.direction;
	float4 clipPosition = mul(camera_worldToClipMatrix, float4(worldPosition, 1.0f));
	if (clipPosition.w <= 0.0f)
		return sceneSampleOutsideScreen;

	float3 ndcPosition = clipPosition.xyz / clipPosition.w;
	float2 uv = 0.5f * ndcPosition.xy + 0.5f;
	if (any(uv < 0.0f) || any(uv >= 1.0f) || ndcPosition.z < 0.0f || ndcPosition.z >= 1.0f)
		return sceneSampleOutsideScreen;

	sample.uv = uv;
	uint2 pixel = min(uint2(uv * screenSize), uint2(screenSize) - 1);
	float sceneNdcDepth = GetSceneNdcDepth(pixel);
	if (sceneNdcDepth >= 1.0f)
		return sceneSampleWithoutGeometry;

	float3 sceneWorldPosition = GetWorldPosition(pixel, sceneNdcDepth, screenSize);
	sample.sceneViewDepth = Camera_GetDepth(sceneWorldPosition);
	sample.depthDelta = Camera_GetDepth(worldPosition) - sample.sceneViewDepth;
	return sceneSampleValid;
}
bool TryRefineSceneHit(WorldRay ray, float2 screenSize, SceneRaySample frontSample, SceneRaySample backSample, out SceneRaySample hitSample)
{
	hitSample = backSample;
	for (uint refinementIndex = 0; refinementIndex < maxSceneRefinementStepCount; refinementIndex++)
	{
		float midpointDistance = 0.5f * (frontSample.distance + backSample.distance);
		SceneRaySample midpointSample;
		if (EvaluateSceneRaySample(ray, midpointDistance, screenSize, midpointSample) != sceneSampleValid)
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
bool TryTraceScene(WorldRay ray, float2 screenSize, float maxDistance, out float3 sceneColor, out float hitDistance)
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
		uint status = EvaluateSceneRaySample(ray, distance, screenSize, sample);
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
			if (TryRefineSceneHit(ray, screenSize, previousSample, sample, hitSample))
			{
				sceneColor = SampleSceneColor(hitSample.uv).rgb;
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
	WorldRay ray;
	ray.origin = origin + max(surfaceBias, 1.0e-4f) * direction;
	ray.direction = normalize(direction);

	float3 sceneColor;
	float hitDistance;
	float maxDistance = sceneRayMaxDistance > 0.0f ? sceneRayMaxDistance : Camera_GetFarClip();
	if (TryTraceScene(ray, screenSize, maxDistance, sceneColor, hitDistance))
		return sceneColor;
	return GetEnvironmentColor(ray.direction);
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
		WorldRay internalRay;
		internalRay.origin = currentPosition + rayBias * currentDirection;
		internalRay.direction = currentDirection;

		float boundsEnterDistance;
		float boundsExitDistance;
		if (!RayFluidBoundsIntersection(internalRay, boundsEnterDistance, boundsExitDistance))
			break;

		DensityHit exitHit;
		float marchStart = max(boundsEnterDistance, 0.0f);
		if (!TryMarchDensitySurface(internalRay, marchStart, boundsExitDistance, densityCrossingExit, false, exitHit))
			break;

		// Opaque geometry can be embedded in or intersect the fluid volume. Test
		// the screen-space depth before processing the density exit interface.
		float3 sceneColor;
		float sceneHitDistance;
		if (TryTraceScene(internalRay, screenSize, exitHit.distance, sceneColor, sceneHitDistance))
		{
			float3 sceneHitPosition = internalRay.origin + sceneHitDistance * internalRay.direction;
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

	uint screenWidth;
	uint screenHeight;
	sceneDepthTexture.GetDimensions(screenWidth, screenHeight);
	float2 screenSize = float2(screenWidth, screenHeight);
	uint2 sourcePixel = threadID.xy;
	float4 sourceColor = GetSceneColor(sourcePixel);

	// Limit the primary density march to the part of the fluid bounds visible
	// before the opaque scene depth at this pixel.
	WorldRay cameraRay = GetCameraRay(sourcePixel, screenSize);
	float boundsEnterDistance;
	float boundsExitDistance;
	if (!RayFluidBoundsIntersection(cameraRay, boundsEnterDistance, boundsExitDistance))
	{
		SetSceneColor(sourcePixel, sourceColor);
		return;
	}

	float marchStart = max(boundsEnterDistance, 0.0f);
	float marchEnd = min(boundsExitDistance, GetSceneDistance(sourcePixel, cameraRay, screenSize));
	DensityHit primaryHit;
	bool rayEntersFluidBounds = boundsEnterDistance >= 0.0f;
	if (!TryMarchDensitySurface(cameraRay, marchStart, marchEnd, densityCrossingAny, rayEntersFluidBounds, primaryHit))
	{
		SetSceneColor(sourcePixel, sourceColor);
		return;
	}

	float sourceIor = primaryHit.isEntering ? 1.0f : fluidIndexOfRefraction;
	float destinationIor = primaryHit.isEntering ? fluidIndexOfRefraction : 1.0f;
	float3 facingNormal = GetFacingNormal(cameraRay.direction, primaryHit.normal);
	float3 reflectionDirection = normalize(reflect(cameraRay.direction, facingNormal));
	float3 refractionDirection = refract(cameraRay.direction, facingNormal, sourceIor / destinationIor);

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
		? GetFresnel(cameraRay.direction, facingNormal, sourceIor, destinationIor)
		: 1.0f;
	float3 fluidColor = lerp(refractionColor, reflectionColor, fresnel);

	// If the near plane starts inside the isosurface, both paths still travel
	// through the primary fluid segment before reaching the camera.
	if (!primaryHit.isEntering)
	{
		float primaryFluidDistance = max(primaryHit.distance - marchStart, 0.0f);
		fluidColor *= exp(-max(absorption, 0.0f) * primaryFluidDistance);
	}

	SetSceneColor(sourcePixel, float4(fluidColor, sourceColor.a));
}