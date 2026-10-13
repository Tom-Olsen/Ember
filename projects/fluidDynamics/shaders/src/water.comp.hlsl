#define EMBER_SCENE_COLOR_ACCESS EMBER_SCENE_COLOR_ACCESS_OUT_OF_PLACE
#include "computeShaderCommon.hlsli"



// Bindings:
cbuffer CallValues : register(b300, CALL_SET)
{
	float4x4 worldToFluidMatrix;	// given by fluid simulation.
	float3 fluidBoundsMin;			// given by fluid simulation.
	float3 fluidBoundsMax;			// given by fluid simulation.
	float surfaceDensity;			// user input. cpu setter enforces >= 1e-4f.
	float indexOfRefraction;		// user input. cpu setter enforces >= 1e-4f.
	float3 absorption;				// user input.
	//float normalSampleDistance;		// user input.
	uint stepCount;					// user input. cpu setter enforces >= 1.
	uint refinementStepCount;		// user input. cpu setter enforces >= 0.
	float stepLength;				// user input. cpu setter enforces >= 0.01f.
	uint surfaceInteractions;		// user input. cpu setter clamps to [1,4].
};
Texture3D<float> densityTexture : register(t100, CALL_SET);
TextureCube<float4> environmentMap : register(t101, CALL_SET);



// Structs:
struct FluidRay
{
	math_Ray ray;
	uint stepCount;		// starts at 0 and gets icremented with every iteration. Gets inherited by children.
	bool insideFluid;
};
struct SurfaceHit
{
	float3 position;
	float3 normal;
	float distance;
	uint hitState;
};
struct SurfaceOptics
{
    float3 reflectionDirection;
    float3 transmissionDirection;
    float weight;	// 0 = transmission, 1 = reflection.
};



// Hit state:
static const uint HitState_undefined = 0;
static const uint HitState_enteringFluid = 1;
static const uint HitState_leavingFluid = 2;
static const uint HitState_missingFluid = 3;
static const uint HitState_hitFluid = 4;
static const uint HitState_hitGeometry = 5;



// Constants:
static const float minStepLength = 0.1f;
static const float normalSampleDistance = 1.0f;



// Small helpers:
float3 GetEnvironmentColor(float3 direction_World)
{
	float3 cubeDirection = mul(mathLinAlg_RotateX3x3(-math_PI_2), direction_World);
	return environmentMap.SampleLevel(colorSampler, cubeDirection, 0.0f).rgb;
}
float SampleDensity_World(float3 position_World)
{
	float3 position_Fluid = mul(worldToFluidMatrix, float4(position_World, 1.0f)).xyz;
	float3 position_Uv = (position_Fluid - fluidBoundsMin) / (fluidBoundsMax - fluidBoundsMin);
	return densityTexture.SampleLevel(colorSamplerClampBorderNoAnisotropy, position_Uv, 0.0f);
}
float SampleDensity_Fluid(float3 fluidPosition)
{
	return densityTexture.SampleLevel(colorSamplerClampBorderNoAnisotropy, fluidPosition, 0.0f);
}
bool InsideFluid_World(float3 position_World)
{
	return SampleDensity_World(position_World) > surfaceDensity;
}
float3 GetFluidBoundsPadding()
{
	// Padding = half a density texture texel.
	uint textureWidth, textureHeight, textureDepth;
	densityTexture.GetDimensions(textureWidth, textureHeight, textureDepth);
	return 0.5f * (fluidBoundsMax - fluidBoundsMin) / float3(textureWidth, textureHeight, textureDepth);
}
bool IsValidClipPosition(float4 clipPosition)
{
	return 0.0f < clipPosition.w && 0.0f <= clipPosition.z && clipPosition.z < clipPosition.w;
}
bool IsValidSceneDepth(float sceneDepth)
{
	return 0.0f <= sceneDepth && sceneDepth < 1.0f;
}



// Optics:
float3 TransmissionDirection(float3 rayDirection, float3 fluidSurfaceNormal, bool entering)
{
	float3 orientedNormal = entering ? fluidSurfaceNormal : -fluidSurfaceNormal;
	float eta = entering ? 1.0f / indexOfRefraction : indexOfRefraction;
	return refract(rayDirection, orientedNormal, eta);
}
float Fresnel(float3 rayDirection, float3 fluidSurfaceNormal, bool entering)
{
    float n1 = entering ? 1.0f : indexOfRefraction;
    float n2 = entering ? indexOfRefraction : 1.0f;
    float3 orientedNormal = entering ? fluidSurfaceNormal : -fluidSurfaceNormal;
    float cosThetaI = saturate(-dot(rayDirection, orientedNormal));
    float eta = n1 / n2;

    // Snell's law:
    float sinThetaTSquared = eta * eta * (1.0f - cosThetaI * cosThetaI);

    // Total internal reflection:
    if (sinThetaTSquared >= 1.0f)
        return 1.0f;

    // Fresnel equations:
    float cosThetaT = sqrt(1.0f - sinThetaTSquared);
    float rs = (n1 * cosThetaI - n2 * cosThetaT) / (n1 * cosThetaI + n2 * cosThetaT);
    float rp = (n1 * cosThetaT - n2 * cosThetaI) / (n1 * cosThetaT + n2 * cosThetaI);
    return 0.5f * (rs * rs + rp * rp);
}
SurfaceOptics ComputeSurfaceOptics(FluidRay fluidRay, SurfaceHit hit)
{
	SurfaceOptics optics;
	bool entering = hit.hitState == HitState_enteringFluid;
	optics.reflectionDirection = reflect(fluidRay.ray.direction, hit.normal);
	optics.transmissionDirection = TransmissionDirection(fluidRay.ray.direction, hit.normal, entering);
	optics.weight = Fresnel(fluidRay.ray.direction, hit.normal, entering);
	return optics;
}



// Big helpers:
float3 ClampSurfaceNormalToBounds(float3 densityNormal, float3 position_Fluid, float3 textureSize)
{
	// Find the boundary whose outward normal best matches the density normal:
	uint boundaryAxis = 0;
	float boundaryPosition = 0.0f;
	float3 boundsNormal = math_zero3;
	float boundsAlignment = 0.0f;
	[unroll]
	for (uint axis = 0; axis < 3; axis++)
	{
		// Positive boundary normal in world space:
		float3 normal_Bounds = math_zero3;
		normal_Bounds[axis] = 1.0f;
		float3 normal_World = normalize(mul(transpose((float3x3)worldToFluidMatrix), normal_Bounds));

		// Keep the best aligned boundary, independent of its distance:
		float alignment = dot(densityNormal, normal_World);
		if (abs(alignment) > boundsAlignment)
		{
			boundaryAxis = axis;
			boundaryPosition = alignment >= 0.0f ? 1.0f : 0.0f;
			boundsNormal = alignment >= 0.0f ? normal_World : -normal_World;
			boundsAlignment = abs(alignment);
		}
	}

	// Distance to the selected boundary in density texels:
	float boundaryDistance = abs(position_Fluid[boundaryAxis] - boundaryPosition) * textureSize[boundaryAxis];

	// Correction strength fades with distance and normal misalignment:
	float distanceWeight = 1.0f - smoothstep(1.0f, 2.0f, boundaryDistance);
	float alignmentWeight = smoothstep(0.70710678f/*cos(45)*/, 0.86602540f/*cos(30)*/, boundsAlignment);
	float boundsWeight = distanceWeight * alignmentWeight;

	// Blend toward the selected boundary normal:
	return normalize(lerp(densityNormal, boundsNormal, boundsWeight));
}
float3 GetDensityNormal(float3 position_World, float3 fallbackNormal)
{
	// Texel size:
	uint textureWidth, textureHeight, textureDepth;
	densityTexture.GetDimensions(textureWidth, textureHeight, textureDepth);
	float3 textureSize = float3(textureWidth, textureHeight, textureDepth);
	float3 texelSize = normalSampleDistance / textureSize;

	// position_World -> position_Fluid:
	float3 position_Bounds = mul(worldToFluidMatrix, float4(position_World, 1.0f)).xyz;
	float3 position_Fluid = (position_Bounds - fluidBoundsMin) / (fluidBoundsMax - fluidBoundsMin);

	// Fluid gradient:
	float densityNegativeX = SampleDensity_Fluid(position_Fluid - float3(texelSize.x, 0.0f, 0.0f));
	float densityPositiveX = SampleDensity_Fluid(position_Fluid + float3(texelSize.x, 0.0f, 0.0f));
	float densityNegativeY = SampleDensity_Fluid(position_Fluid - float3(0.0f, texelSize.y, 0.0f));
	float densityPositiveY = SampleDensity_Fluid(position_Fluid + float3(0.0f, texelSize.y, 0.0f));
	float densityNegativeZ = SampleDensity_Fluid(position_Fluid - float3(0.0f, 0.0f, texelSize.z));
	float densityPositiveZ = SampleDensity_Fluid(position_Fluid + float3(0.0f, 0.0f, texelSize.z));
	float3 gradient_Fluid = float3(
		densityPositiveX - densityNegativeX,
		densityPositiveY - densityNegativeY,
		densityPositiveZ - densityNegativeZ) / (2.0f * texelSize);

	// gradient_Fluid -> gradient_World:
	float3 gradient_Bounds = gradient_Fluid / (fluidBoundsMax - fluidBoundsMin);
	float3 gradient_World = mul(transpose((float3x3)worldToFluidMatrix), gradient_Bounds);

	// Singular gradient:
	float gradientLengthSquared = dot(gradient_World, gradient_World);
	if (gradientLengthSquared < 1.0e-12f)
		return normalize(fallbackNormal);

	// Flip and normalize gradient:
	float3 densityNormal = -gradient_World * rsqrt(gradientLengthSquared);
	return ClampSurfaceNormalToBounds(densityNormal, position_Fluid, textureSize);
}
float4 RefineFluidHit(float3 pos0, float3 pos1, bool insideFluid)
{
	float subStepLength = stepLength;
	float distance = stepLength; // distance from pos0 to final posMid.
	for (uint i = 0; i < refinementStepCount; i++)
	{
		float3 posMid = 0.5f * (pos0 + pos1);
		bool midInsideFluid = SampleDensity_World(posMid) > surfaceDensity;
		subStepLength = 0.5f * subStepLength;
		if (insideFluid != midInsideFluid)
		{
			pos1 = posMid;
			distance -= subStepLength;
		}
		else
			pos0 = posMid;
	}
	return float4(0.5f * (pos0 + pos1), distance - 0.5f * subStepLength);
}
float4 RefineGeometryHit(float4 clipPos0, float4 clipPos1, float2 screenSize)
{
	static const float minSurfaceThickness = 0.05f;
	static const float relativeSurfaceThickness = 0.005f;
	const float4 invalidHit = float4(0.0f, 0.0f, 0.0f, 1.0e30f);
	float subStepLength = stepLength;
	float distance = stepLength; // distance from pos0 to final endpoint.
	for (uint i = 0; i < refinementStepCount; i++)
	{
		float4 clipPosMid = 0.5f * (clipPos0 + clipPos1);
		if (!IsValidClipPosition(clipPosMid))
    		return invalidHit;
		float2 uv = 0.5f * (clipPosMid.xy / clipPosMid.w + 1.0f);
		if (any(uv < 0.0f) || any(uv >= 1.0f))
    		return invalidHit;
		float sceneDepthMid = Scene_GetNdcDepth(uv, screenSize);
		if (!IsValidSceneDepth(sceneDepthMid))
			return invalidHit;

		float depthDeltaMid = clipPosMid.z / clipPosMid.w - sceneDepthMid;
		subStepLength *= 0.5f;
		if (depthDeltaMid >= 0.0f)
		{
			clipPos1 = clipPosMid;
			distance -= subStepLength;
		}
		else
			clipPos0 = clipPosMid;
	}

	// Validate the final back endpoint:
	if (!IsValidClipPosition(clipPos1))
		return invalidHit;
	float2 uv = 0.5f * (clipPos1.xy / clipPos1.w + 1.0f);
	if (any(uv < 0.0f) || any(uv >= 1.0f))
		return invalidHit;
	uint2 pixel = uv * screenSize;
	float sceneDepth = Scene_GetNdcDepth(pixel);
	if (!IsValidSceneDepth(sceneDepth))
		return invalidHit;

	// Apply thickness tolerance in view-space units:
	float3 pos1 = Camera_GetWorldPosition(clipPos1.xyz / clipPos1.w);
	float3 scenePosition = Camera_GetWorldPosition(pixel, sceneDepth, screenSize);
	float sceneViewDepth = Camera_GetDepth(scenePosition);
	float depthDelta = Camera_GetDepth(pos1) - sceneViewDepth;
	float surfaceThickness = max(minSurfaceThickness, relativeSurfaceThickness * sceneViewDepth);

	if (depthDelta < 0.0f || depthDelta > surfaceThickness)
		return invalidHit;

	return float4(pos1, distance);
}
SurfaceHit FindNextHit(inout FluidRay fluidRay, float2 screenSize)
{
	// Reject empty search interval:
	SurfaceHit hit = {math_zero3, math_zero3, 0.0f, HitState_missingFluid};
	if (fluidRay.stepCount >= stepCount)
		return hit;

	// Fluid ray march (step0):
	float3 currentPosition = fluidRay.ray.origin;
	float3 nextPosition = currentPosition + stepLength * fluidRay.ray.direction;
	float currentDensity = SampleDensity_World(currentPosition);
	float nextDensity = SampleDensity_World(nextPosition);
	bool nextInsideFluid = nextDensity > surfaceDensity;
	float distance = 0.0f;	// distance the ray marches.
	// Screen ray march (step0):
	float4 currentClipPosition = Camera_WorldToClip(float4(currentPosition, 1.0f));
	float4 nextClipPosition = Camera_WorldToClip(float4(nextPosition, 1.0f));
	float currentSceneDepth = Scene_GetNdcDepth(currentPosition, screenSize);
	float nextSceneDepth = Scene_GetNdcDepth(nextPosition, screenSize);
	bool currentValid = IsValidClipPosition(currentClipPosition) && IsValidSceneDepth(currentSceneDepth);
	bool nextValid = IsValidClipPosition(nextClipPosition) && IsValidSceneDepth(nextSceneDepth);
	float currentDepthDelta = currentValid ? currentClipPosition.z / currentClipPosition.w - currentSceneDepth : 0.0f;
	float nextDepthDelta = nextValid ? nextClipPosition.z / nextClipPosition.w - nextSceneDepth : 0.0f;

	// Ray march loop:
	bool hitFluid = false;
	bool hitGeometry = false;
	while (fluidRay.stepCount < stepCount)
	{
		fluidRay.stepCount++;

		// Detect fluidSurface/sceneGeometry hit:
		hitFluid = fluidRay.insideFluid != nextInsideFluid;
		hitGeometry = currentValid && nextValid && currentDepthDelta < 0.0f && nextDepthDelta >= 0.0f;

		// Refine fluid hit (can fail in case of hitGeometry, if it does we keep marching):
		if (hitFluid || hitGeometry)
		{
			float4 fluidRefinement = hitFluid ? RefineFluidHit(currentPosition, nextPosition, fluidRay.insideFluid) : float4(0,0,0, 1.0e30f);
			float4 geometryRefinement = hitGeometry ? RefineGeometryHit(currentClipPosition, nextClipPosition, screenSize) : float4(0,0,0, 1.0e30f);
			// Hit:
			if (min(fluidRefinement.w, geometryRefinement.w) < 1.0e30f)
			{
				float3 fallbackNormal = fluidRay.insideFluid ? fluidRay.ray.direction : -fluidRay.ray.direction;
				if (fluidRefinement.w <= geometryRefinement.w)
				{
					hit.position = fluidRefinement.xyz;
					hit.normal = GetDensityNormal(hit.position, fallbackNormal);
					hit.distance = distance + fluidRefinement.w;
					hit.hitState = fluidRay.insideFluid ? HitState_leavingFluid : HitState_enteringFluid;
					return hit;
				}
				else
				{
					hit.position = geometryRefinement.xyz;
					hit.normal = math_zero3;	// not needed on geometry hit.
					hit.distance = distance + geometryRefinement.w;
					hit.hitState = HitState_hitGeometry;
					return hit;
				}
			}
		}

		// Out of steps:
		if (fluidRay.stepCount >= stepCount)
			break;

		// Carry previous step (next -> current):
		currentPosition = nextPosition;
		currentDensity = nextDensity;
		currentClipPosition = nextClipPosition;
		currentSceneDepth = nextSceneDepth;
		currentValid = nextValid;
		currentDepthDelta = nextDepthDelta;

		// Prepare the new next fluid sample:
		nextPosition += stepLength * fluidRay.ray.direction;
		nextDensity = SampleDensity_World(nextPosition);
		nextInsideFluid = nextDensity > surfaceDensity;
		distance += stepLength;

		// Prepare the new next scene sample:
		nextClipPosition = Camera_PositionWorldToClip(nextPosition);
		nextSceneDepth = Scene_GetNdcDepth(nextPosition, screenSize);
		nextValid = IsValidClipPosition(nextClipPosition) && IsValidSceneDepth(nextSceneDepth);
		nextDepthDelta = nextValid ? nextClipPosition.z / nextClipPosition.w - nextSceneDepth : 0.0f;
	}

	// Miss:
	hit.distance = distance + stepLength;
	return hit;
}
float3 RayCascade(FluidRay initialRay, float3 sceneColor, uint2 sourcePixel, float2 screenSize)
{
	// initialRay must have a normalized direction, a prepared origin, and the correct insideFluid state.
	// Its stepCount contains the marching steps already consumed along this path.
	struct PendingRay
	{
		FluidRay fluidRay;
		float3 throughput;	// product of the reflection/transmission weights along this path.
		uint depth;			// number of surface interactions before this ray.
	};
	static const float throughputThreshold = 0.1f;

	// Depth-first traversal needs at most 6 (max value of surfaceInteractions) pending rays:
	PendingRay stack[6];
	PendingRay initial = {initialRay, math_one3, 0};
	stack[0] = initial;
	uint stackSize = 1;
	float3 accumulatedColor = math_zero3;
	float epsilon = stepLength * exp2(-float(refinementStepCount));

	// Build reflection+refracton tree iteratively always adding transmission first so reflection gets popped first:
	while (stackSize > 0)
	{
		// Pop next ray:
		stackSize--;
		PendingRay pending = stack[stackSize];
		SurfaceHit hit = FindNextHit(pending.fluidRay, screenSize);

		// Beer–Lambert absorption over the segment through water:
		if (pending.fluidRay.insideFluid)
			pending.throughput *= exp(-absorption * hit.distance);

		// Terminate ray cascade once remaining light intensity is to low:
		if (max(pending.throughput.r, max(pending.throughput.g, pending.throughput.b)) < throughputThreshold)
			continue;

		// Miss (includes out of step budget):
		if (hit.hitState == HitState_missingFluid)
		{
			// Initial ray misses -> sceneColor:
    		if (pending.depth == 0)
        		return sceneColor;
			// Environment fallback:
			accumulatedColor += pending.throughput * GetEnvironmentColor(pending.fluidRay.ray.direction);
			continue;
		}

		// Hit scene geometry:
		if (hit.hitState == HitState_hitGeometry)
		{
			float2 hitUv = Camera_WorldToPixel(hit.position, screenSize) / screenSize;
			float3 geometryColor = Scene_SampleColor(hitUv).rgb;
			accumulatedColor += pending.throughput * geometryColor;
			continue;
		}

		// Hit geometry in front of the first fluid surface:
		if (pending.depth == 0)
		{
			float4 surfaceClip = mul(camera_worldToClipMatrix, float4(hit.position, 1.0f));
			float surfaceNdcDepth = surfaceClip.z / surfaceClip.w;
			if (Scene_GetNdcDepth(sourcePixel) < surfaceNdcDepth)
				return sceneColor;
		}

		// At the limit, use environment color as fallback:
		SurfaceOptics optics = ComputeSurfaceOptics(pending.fluidRay, hit);
		uint nextDepth = pending.depth + 1;
		if (nextDepth >= surfaceInteractions || pending.fluidRay.stepCount >= stepCount)
		{
			float3 terminalColor = GetEnvironmentColor(optics.reflectionDirection);
			if (optics.weight < 1.0f)
			{
				float3 transmissionColor = GetEnvironmentColor(optics.transmissionDirection);
				terminalColor = lerp(transmissionColor, terminalColor, optics.weight);
			}
			accumulatedColor += pending.throughput * terminalColor;
			continue;
		}

		// Push transmission first so reflection is popped first:
		float3 incidentNormal = hit.hitState == HitState_enteringFluid ? hit.normal : -hit.normal;
		if (optics.weight < 1.0f)
		{
			PendingRay transmission = pending;
			transmission.fluidRay.ray.origin = hit.position - epsilon * incidentNormal;
			transmission.fluidRay.ray.direction = optics.transmissionDirection;
			transmission.fluidRay.insideFluid = !pending.fluidRay.insideFluid;
			transmission.throughput *= 1.0f - optics.weight;
			transmission.depth = nextDepth;
			stack[stackSize] = transmission;
			stackSize++;
		}
		if (optics.weight > 0.0f)
		{
			PendingRay reflection = pending;
			reflection.fluidRay.ray.origin = hit.position + epsilon * incidentNormal;
			reflection.fluidRay.ray.direction = optics.reflectionDirection;
			reflection.fluidRay.insideFluid = pending.fluidRay.insideFluid;
			reflection.throughput *= optics.weight;
			reflection.depth = nextDepth;
			stack[stackSize] = reflection;
			stackSize++;
		}
	}

	return accumulatedColor;
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
	float4 color = Scene_GetColor(sourcePixel);

	// Normalized fluid ray:
	FluidRay ray = {Camera_GetRay(sourcePixel, screenSize), 0, false};
	ray.insideFluid = InsideFluid_World(ray.ray.origin);

	// Jump ray to fluid bounds surface:
	float3 boundsPadding = GetFluidBoundsPadding();
	float boundsEnterDistance;
	float boundsExitDistance;
	if (!mathRay_TryIntersectRotatedBounds(ray.ray, worldToFluidMatrix, fluidBoundsMin - boundsPadding, fluidBoundsMax + boundsPadding, boundsEnterDistance, boundsExitDistance))
	{
		Scene_SetColor(sourcePixel, color);
		return;
	}
	ray.ray.origin = mathRay_GetPoint(ray.ray, max(boundsEnterDistance, 0.0f));

	// Ray marching with up to 4 ray splits on surface hit:
	color.xyz = RayCascade(ray, color.xyz, sourcePixel, screenSize);
	Scene_SetColor(sourcePixel, color);
	return;
}