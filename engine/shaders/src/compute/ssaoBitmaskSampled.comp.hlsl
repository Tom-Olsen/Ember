#include "computeShaderCommon.hlsli"
#include "bilateralUpsampling.hlsli"
#include "ssaoConstants.h"



// Bindings:
[[vk::image_format("r16f")]] RWTexture2D<float> ambientOcclusionMap : register(u200, CALL_SET);
// Sampling parameters:
cbuffer SsaoSamplingParameters : register(b300, SHADER_SET)
{
	uint sliceCount;
	uint samplesPerSide;
	float sampleRadius;		// in world units.
	float sampleBias;		// in world units.
	float sampleThickness;	// in world units, along the view ray away from the camera.
	uint rngSeed;
};



// Input angles are in radians, measured from V toward T in the slice plane (defined below).
// normalAngle is the angle of Q, the projected surface normal's direction.
// Output bit i represents angular sector i of the 180-degree arc centered on Q.
uint SampleMask(float frontAngle, float backAngle, float normalAngle)
{
	// Unwrap the shortest front-to-back arc, then wrap its midpoint relative to the normal.
	float angleSpan = backAngle - frontAngle;
	angleSpan -= 2.0f * math_PI * floor(angleSpan * (0.5f * math_PI_INV) + 0.5f);
	float middleAngle = frontAngle + 0.5f * angleSpan - normalAngle;
	middleAngle -= 2.0f * math_PI * floor(middleAngle * (0.5f * math_PI_INV) + 0.5f);
	float halfSpan = 0.5f * abs(angleSpan);

	// Subtracting normalAngle above puts Q at angle zero. The half-disk's arc spans [-pi/2, +pi/2] relative to Q.
	// Clip to that arc and map it to [0, sector count].
	float minimumSector = saturate((middleAngle - halfSpan) * math_PI_INV + 0.5f) * SSAO_BITMASK_VISIBILITY_SECTORS;
	float maximumSector = saturate((middleAngle + halfSpan) * math_PI_INV + 0.5f) * SSAO_BITMASK_VISIBILITY_SECTORS;
	// An interval narrower than half a sector cannot cover half of any sector.
	if (maximumSector - minimumSector < 0.5f)
		return 0;

	// A sector [i, i+1] must overlap the interval by at least 0.5, including boundary ties.
	// With interval width >= 0.5, this requires minimumSector - 0.5 <= i <= maximumSector - 0.5.
	// firstSector is inclusive, endSector is exclusive.
	uint firstSector = uint(ceil(max(0.0f, minimumSector - 0.5f)));
	uint endSector = uint(floor(maximumSector + 0.5f));
	if (endSector <= firstSector)
		return 0;
	uint sectorCount = endSector - firstSector;

	// A full hemisphere uses shift zero; an empty interval returns above, avoiding shifts by 32.
	uint sectorMask = 0xffffffffu >> (SSAO_BITMASK_VISIBILITY_SECTORS - sectorCount);
	return sectorMask << firstSector;
}



// Geometry notation:
// P = viewPosition, N = viewNormal, V = viewDirectionSampleToCamera.
// A slice chooses unit directions T = sliceTangent and S = sliceNormal, perpendicular to V and to each other.
// Its infinite plane is Pi = { P + a*V + b*T : a,b are real }.
// A disk of radius R > 0 in Pi restricts those coefficients to a*a + b*b <= R*R.
// Its half-disk keeps points X satisfying dot(N, X-P) >= 0.
// Nparallel = N - dot(N,S)*S lies along Pi.
// Q = normalize(Nparallel) is the half-disk's north-pole direction:
// P + R*Q is the midpoint of its curved, 180-degree boundary.
// The plane changes between slices; P, V and N stay fixed, and Q is recomputed.
// These definitions describe the angular sectors.
// Depth samples are located separately along a screen-space line,
// with sampleRadius controlling that line's search extent.
float Occlusion(uint2 sourcePixel, float sourceNdcDepth, float2 screenSize)
{
	// Pixel data in world space:
	float3 worldPosition = Camera_GetWorldPosition(sourcePixel, sourceNdcDepth, screenSize);
	float3 worldNormal = normalize(2.0f * gbufferNormalTexture.Load(int3(sourcePixel, 0)).xyz - 1.0f);

	// Pixel data in view space:
	float3 viewPosition = mul(camera_viewMatrix, float4(worldPosition, 1.0f)).xyz;
	float3 viewNormal = normalize(mul((float3x3)camera_viewMatrix, worldNormal));

	// Cached helper quantities:
	float3 viewDirectionSampleToCamera = -mul((float3x3)camera_viewMatrix, Camera_GetRayDirection(worldPosition));
	float3 sampleOrigin = viewPosition + sampleBias * viewNormal;
	float2 sourcePixelCenter = float2(sourcePixel) + 0.5f;
	float clipW = mul(camera_projMatrix, float4(viewPosition, 1.0f)).w;
	if (clipW <= 0.0f)
		return 0.0f;

	// Choose screen-space lines through sourcePixelCenter.
	// Each line defines a slice plane containing { P + t*V : t is real }.
	// For perspective projection this line also contains the camera.
	// The slice plane projects to the screen line, letting depth reads along it locate geometry in that plane.
	// V is the common axis of the planes, not generally Q.
	// Lines are spaced over one pi-wide interval.
	float occlusion = 0.0f;
	float rotationJitter = math_PI * mathRandom_Uniform01(sourcePixel.x, sourcePixel.y, rngSeed, 0.0f);
	for (uint sliceIndex = 0; sliceIndex < sliceCount; sliceIndex++)
	{
		float angle = rotationJitter + math_PI * float(sliceIndex) / float(sliceCount);
		float2 screenDirection; // init direction (cos(angle), sin(angle)) in pixel coordinates.
		sincos(angle, screenDirection.y, screenDirection.x);

		// View-space displacement corresponding to one pixel along screenDirection at P's depth:
		// 2/screenSize converts pixels to NDC.
		// Division by the projection diagonal and multiplication by clipW undo projection scaling and perspective division.
		// Z stays fixed.
		float3 screenStep = float3(2.0f * screenDirection / (screenSize * float2(camera_projMatrix[0][0], camera_projMatrix[1][1])), 0.0f) * clipW;
		float projectedRadius = sampleRadius / length(screenStep); // world/view units divided by units per pixel gives the search radius in pixels.

		// S is perpendicular to the plane spanned by screenStep and V.
		// T lies in that plane and is perpendicular to V.
		// This cross-product order makes T oppose screenStep's component perpendicular to V.
		// All angles below use that same orientation.
		float3 sliceNormal = normalize(cross(screenStep, viewDirectionSampleToCamera));
		float3 sliceTangent = cross(sliceNormal, viewDirectionSampleToCamera);

		// Remove N's component along S to obtain Nparallel.
		// Its normalized direction Q is the half-disk's north pole.
		// If Nparallel is almost zero, Q is unstable -> skip the slice.
		float3 projectedNormal = viewNormal - sliceNormal * dot(viewNormal, sliceNormal);
		if (dot(projectedNormal, projectedNormal) <= 1.0e-8f)
			continue;

		// atan2(T component, V component) measures the signed angle from V toward T.
		// Normalizing projectedNormal would scale both components equally, leaving this angle unchanged.
		float normalAngle = atan2(dot(projectedNormal, sliceTangent), dot(projectedNormal, viewDirectionSampleToCamera));

		// One mask is shared by both sides of the slice:
		uint occludedSectors = 0;
		for (uint sideIndex = 0; sideIndex < 2; sideIndex++)
		{
			// Search both directions along the same screen line:
			float side = sideIndex == 0 ? -1.0f : 1.0f;
			float stepJitter = mathRandom_Uniform01(sourcePixel.x, sourcePixel.y, 2u * sliceIndex + sideIndex, 1u + rngSeed, 0.0f);
			float stepSize = projectedRadius / float(samplesPerSide + 1);
			for (uint sampleIndex = 0; sampleIndex < samplesPerSide; sampleIndex++)
			{
				// Step:
				float sampleDistance = (float(sampleIndex) + 1.0f + stepJitter) * stepSize;
				float2 samplePixelCenter = sourcePixelCenter + side * sampleDistance * screenDirection;
				if (any(samplePixelCenter < 0.0f) || any(samplePixelCenter >= screenSize))
					break;

				// Sample screen pixel:
				uint2 samplePixel = uint2(samplePixelCenter);
				if (all(samplePixel == sourcePixel))
					continue;

				// Sample ndc depth:
				float sampleNdcDepth = Scene_GetNdcDepth(samplePixel);
				if (sampleNdcDepth >= 1.0f)
					continue;

				// Front and back offset, back is estimated with const sampleThickness:
				float3 sampleWorldPosition = Camera_GetWorldPosition(samplePixel, sampleNdcDepth, screenSize);
				float3 frontOffset = mul(camera_viewMatrix, float4(sampleWorldPosition, 1.0f)).xyz - sampleOrigin;
				float3 backOffset = frontOffset - viewDirectionSampleToCamera * sampleThickness;

				// Convert offsets to angles:
				float frontAngle = atan2(dot(frontOffset, sliceTangent), dot(frontOffset, viewDirectionSampleToCamera));
				float backAngle = atan2(dot(backOffset, sliceTangent), dot(backOffset, viewDirectionSampleToCamera));

				// Mark the sectors covered by the front-to-back interval:
				occludedSectors |= SampleMask(frontAngle, backAngle, normalAngle);
			}
		}

		// Accumulate occlusion without cosine weigthing. Physically inaccurate but much cheaper:
		occlusion += float(countbits(occludedSectors)) / float(SSAO_BITMASK_VISIBILITY_SECTORS);
	}
	// Average the blocked angular fraction across all requested slices.
	return occlusion / float(sliceCount);
}



[numthreads(8, 8, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
	if (threadID.x >= pc.threadCount.x || threadID.y >= pc.threadCount.y)
		return;

	// Ambient occlusion map size:
	uint aoWidth;
	uint aoHeight;
	ambientOcclusionMap.GetDimensions(aoWidth, aoHeight);
	if (threadID.x >= aoWidth || threadID.y >= aoHeight)
		return;

	// Screen size:
	uint screenWidth;
	uint screenHeight;
	sceneDepthTexture.GetDimensions(screenWidth, screenHeight);

	// Compute ambient occlusion:
	float occlusion = 0.0f;
	uint2 sourcePixel = Bilateral_LowResolutionPixelToScreenPixel(threadID.xy, uint2(screenWidth, screenHeight), float2(aoWidth, aoHeight));
	float sourceNdcDepth = Scene_GetNdcDepth(sourcePixel);
	if (sourceNdcDepth < 1.0f)
		occlusion = Occlusion(sourcePixel, sourceNdcDepth, float2(screenWidth, screenHeight));
	ambientOcclusionMap[threadID.xy] = occlusion;
}