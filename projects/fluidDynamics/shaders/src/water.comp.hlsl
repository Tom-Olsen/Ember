#define EMBER_SCENE_COLOR_ACCESS EMBER_SCENE_COLOR_ACCESS_OUT_OF_PLACE
#include "computeShaderCommon.hlsli"



// Bindings:
cbuffer CallValues : register(b300, CALL_SET)
{
	float4x4 worldToFluidMatrix;	// given by fluid simulation.
	float3 fluidBoundsMin;			// given by fluid simulation.
	float3 fluidBoundsMax;			// given by fluid simulation.
	float surfaceDensity;			// user input.
	float indexOfRefraction;		// user input.
	//float3 absorption;				// user input.
	//float normalSampleDistance;		// user input.
	uint maxStepCount;				// user input.
	uint maxRefinementStepCount;	// user input.
};
Texture3D<float> densityTexture : register(t100, CALL_SET);
TextureCube<float4> environmentMap : register(t101, CALL_SET);



// Structs:
struct FluidRay
{
	math_Ray ray;
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
    float3 refractionDirection;
    float reflectionWeight;		// 0 = refraction, 1 = reflection.
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
float3 RefractionDirection(float3 rayDirection, float3 fluidSurfaceNormal, bool entering)
{
	float3 orientedNormal = entering ? fluidSurfaceNormal : -fluidSurfaceNormal;
	float eta = entering ? 1.0f / indexOfRefraction : indexOfRefraction;
	return refract(rayDirection, orientedNormal, eta);
}
float SchlickFresnel(float3 rayDirection, float3 fluidSurfaceNormal, bool entering)
{
	float n1 = entering ? 1.0f : indexOfRefraction;
	float n2 = entering ? indexOfRefraction  : 1.0f;
	float3 orientedNormal = entering ? fluidSurfaceNormal : -fluidSurfaceNormal;
	float cosTheta = saturate(-dot(rayDirection, orientedNormal));
	float r = (n1 - n2) / (n1 + n2);
	float f0 = r * r;
	return f0 + (1.0f - f0) * pow(1.0f - cosTheta, 5.0f);
}
SurfaceOptics ComputeSurfaceOptics(FluidRay fluidRay, SurfaceHit hit)
{
	SurfaceOptics optics;
	bool entering = hit.hitState == enteringFluid;
	optics.reflectionDirection = reflect(fluidRay.ray.direction, hit.normal);
	optics.refractionDirection = RefractionDirection(fluidRay.ray.direction, hit.normal, entering);
	bool totalInternalReflection = dot(optics.refractionDirection, optics.refractionDirection) == 0.0f;
	float fresnel = SchlickFresnel(fluidRay.ray.direction, hit.normal, entering);
	optics.reflectionWeight = totalInternalReflection ? 1.0f : fresnel;
	return optics;
}



// Big helpers:
float3 GetDensityNormal(float3 position_World, float3 fallbackNormal)
{
	// Texel size:
	uint textureWidth, textureHeight, textureDepth;
	densityTexture.GetDimensions(textureWidth, textureHeight, textureDepth);
	float3 texelSize = normalSampleDistance / float3(textureWidth, textureHeight, textureDepth);

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
	return -gradient_World * rsqrt(gradientLengthSquared);
}
SurfaceHit FindFluidSurface(FluidRay fluidRay, float startDistance, float endDistance)
{
	// Reject empty search interval:
	SurfaceHit hit = {math_zero3, math_zero3, 0.0f, missingFluid};
	float marchDistance = endDistance - startDistance;
	if (marchDistance <= 0.0f)
		return hit;

	// Increase step length if needed to cover the interval within the step limit:
	float stepLength = marchDistance / float(maxStepCount);
	stepLength = max(stepLength, minStepLength);
	uint stepCount = min((uint)ceil(marchDistance / stepLength), maxStepCount);

	// Ray march:
	float3 currentPosition = mathRay_GetPoint(fluidRay.ray, startDistance);
	float3 nextPosition = currentPosition + stepLength * fluidRay.ray.direction;
	float currentDensity = SampleDensity_World(currentPosition);
	float nextDensity = SampleDensity_World(nextPosition);
	bool nextInsideFluid = nextDensity > surfaceDensity;
	float distance = startDistance + stepLength;
	for (uint step = 1; step < stepCount; step++)
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
	for (uint i = 0; i < maxRefinementStepCount; i++)
	{
		float3 midPosition = 0.5f * (currentPosition + nextPosition);
		float midDensity = SampleDensity_World(midPosition);
		bool midInsideFluid = midDensity > surfaceDensity;
		stepLength = 0.5f * stepLength;
		if (fluidRay.insideFluid != midInsideFluid)
		{
			nextPosition = midPosition;
			nextDensity = midDensity;
			distance -= stepLength;	// distance from origin to nextPosition.
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
	hit.distance = distance - 0.5f * stepLength;	// distance from origin to nextPosition.
	hit.hitState = fluidRay.insideFluid ? leavingFluid : enteringFluid;
	return hit;
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

	// Normalized fluid ray:
	FluidRay fluidRay = {Camera_GetRay(sourcePixel, screenSize), false};
	fluidRay.insideFluid = InsideFluid_World(fluidRay.ray.origin);

	// Skip rays that do not hit the fluid bounds:
	float3 boundsPadding = GetFluidBoundsPadding();
	float boundsEnterDistance;
	float boundsExitDistance;
	if (!mathRay_TryIntersectRotatedBounds(fluidRay.ray, worldToFluidMatrix, fluidBoundsMin - boundsPadding, fluidBoundsMax + boundsPadding, boundsEnterDistance, boundsExitDistance))
	{
		Scene_SetColor(sourcePixel, sourceColor);
		return;
	}

	// Find fluid surface:
	float marchStart = max(boundsEnterDistance, 0.0f);
	float marchEnd = min(boundsExitDistance, Scene_GetDistance(sourcePixel, fluidRay.ray, screenSize, true));
	SurfaceHit hit = FindFluidSurface(fluidRay, marchStart, marchEnd);

	// Missed fluid surface:
	if (hit.hitState == missingFluid)
	{
		Scene_SetColor(sourcePixel, sourceColor);
		return;
	}

	// Fluid normals:
	//sourceColor.xyz = hit.normal;

	// Reflect environment:
	//float3 reflectionDirection = reflect(fluidRay.ray.direction, hit.normal);
	//float3 reflectionColor = GetEnvironmentColor(reflectionDirection);
	//sourceColor.xyz = reflectionColor;

	// Refract environment:
	//float3 refractionDirection = RefractionDirection(fluidRay.ray.direction, hit.normal, hit.hitState == enteringFluid);
	//float3 refractionColor = GetEnvironmentColor(refractionDirection);
	//sourceColor.xyz = refractionColor;

	// Reflect + Refract environment via SchlickFresnel:
	SurfaceOptics optics = ComputeSurfaceOptics(fluidRay, hit);
	float3 reflectionColor = GetEnvironmentColor(optics.reflectionDirection);
	sourceColor.xyz = reflectionColor;
	if (optics.reflectionWeight < 1.0f)
	{
	    float3 refractionColor = GetEnvironmentColor(optics.refractionDirection);
	    sourceColor.xyz = lerp(refractionColor, reflectionColor, optics.reflectionWeight);
	}

	Scene_SetColor(sourcePixel, sourceColor);
	return;
}