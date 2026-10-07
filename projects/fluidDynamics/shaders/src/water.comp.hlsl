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
	//float3 absorption;				// user input.
	//float normalSampleDistance;		// user input.
	uint stepCount;				// user input. cpu setter enforces >= 1.
	uint refinementStepCount;	// user input. cpu setter enforces >= 0.
	float stepLength;				// user input. cpu setter enforces >= 0.01f.
	uint surfaceInteractions;					// user input. cpu setter clamps to [1,4].
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
static const uint enteringFluid = 0;
static const uint leavingFluid = 1;
static const uint missingFluid = 2;



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
	bool entering = hit.hitState == enteringFluid;
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
SurfaceHit FindFluidSurface(inout FluidRay fluidRay)
{
	// Reject empty search interval:
	SurfaceHit hit = {math_zero3, math_zero3, 0.0f, missingFluid};
	if (fluidRay.stepCount >= stepCount)
		return hit;

	// Ray march (step0):
	float3 currentPosition = fluidRay.ray.origin;
	float3 nextPosition = currentPosition + stepLength * fluidRay.ray.direction;
	float currentDensity = SampleDensity_World(currentPosition);
	float nextDensity = SampleDensity_World(nextPosition);
	bool nextInsideFluid = nextDensity > surfaceDensity;
	float distance = stepLength;	// missing distance from camera to fluid bounds surface. Not needed as that is air distance.
	fluidRay.stepCount++;
	for (; fluidRay.stepCount < stepCount; fluidRay.stepCount++)
	{
		if (fluidRay.insideFluid != nextInsideFluid)
			break;

		// Next step:
		currentPosition = nextPosition;
		nextPosition += stepLength * fluidRay.ray.direction;
		currentDensity = nextDensity;
		nextDensity = SampleDensity_World(nextPosition);
		nextInsideFluid = nextDensity > surfaceDensity;
		distance += stepLength;	// distance from origin to nextPosition.
	}

	// Miss:
	if (fluidRay.insideFluid == nextInsideFluid)
    	return hit;

	// Refine hit:
	float subStepLength = stepLength;
	for (uint i = 0; i < refinementStepCount; i++)
	{
		float3 midPosition = 0.5f * (currentPosition + nextPosition);
		float midDensity = SampleDensity_World(midPosition);
		bool midInsideFluid = midDensity > surfaceDensity;
		subStepLength = 0.5f * subStepLength;
		if (fluidRay.insideFluid != midInsideFluid)
		{
			nextPosition = midPosition;
			nextDensity = midDensity;
			distance -= subStepLength;	// distance from origin to nextPosition.
		}
		else
		{
			currentPosition = midPosition;
			currentDensity = midDensity;
		}
	}

	// Hit:
	float3 fallbackNormal = fluidRay.insideFluid ? fluidRay.ray.direction : -fluidRay.ray.direction;
	hit.position = 0.5f * (currentPosition + nextPosition);
	hit.normal = GetDensityNormal(hit.position, fallbackNormal);
	hit.distance = distance - 0.5f * subStepLength;	// distance from origin to nextPosition.
	hit.hitState = fluidRay.insideFluid ? leavingFluid : enteringFluid;
	return hit;
}
float3 RayCascade(FluidRay initialRay, float3 sceneColor)
{
	// initialRay must have a normalized direction, a prepared origin, and the correct insideFluid state.
	// Its stepCount contains the marching steps already consumed along this path.
	struct PendingRay
	{
		FluidRay fluidRay;
		float3 throughput;	// Product of the reflection/transmission weights along this path.
		uint depth;			// Number of surface interactions before this ray.
	};

	// Depth-first traversal needs at most 4 (max value of surfaceInteractions) pending rays:
	PendingRay stack[4];
	PendingRay initial = {initialRay, math_one3, 0};
	stack[0] = initial;
	uint stackSize = 1;
	float3 accumulatedColor = math_zero3;
	float epsilon = stepLength * exp2(-float(refinementStepCount));

	// Build reflection+refracton tree iteratively always adding transmission first so reflection gets popped first:
	while (stackSize > 0)
	{
		// Pop one ray. FindFluidSurface updates its consumed step count:
		stackSize--;
		PendingRay pending = stack[stackSize];
		SurfaceHit hit = FindFluidSurface(pending.fluidRay);

		// Miss (includes out of step budget):
		if (hit.hitState == missingFluid)
		{
			// Initial ray misses -> sceneColor:
    		if (pending.depth == 0)
        		return sceneColor;
			// Environment fallback:
			accumulatedColor += pending.throughput * GetEnvironmentColor(pending.fluidRay.ray.direction);
			continue;
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
		float3 incidentNormal = hit.hitState == enteringFluid ? hit.normal : -hit.normal;
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
	ray.ray.origin = mathRay_GetPoint(ray.ray, max(boundsEnterDistance, 0.0f));;

	// Ray marching with up to 4 ray splits on surface hit:
	color.xyz = RayCascade(ray, color.xyz);
	Scene_SetColor(sourcePixel, color);
	return;
}