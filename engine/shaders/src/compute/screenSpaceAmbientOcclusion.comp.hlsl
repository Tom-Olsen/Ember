#include "computeShaderCommon.hlsli"
#include "deferredRenderingConstants.h"
#include "screenSpaceAmbientOcclusion.hlsli"



[numthreads(8, 8, 1)]
void main(uint3 threadID : SV_DispatchThreadID)
{
	if (threadID.x >= pc.threadCount.x || threadID.y >= pc.threadCount.y)
		return;

	// Screen size:
	uint screenWidth;
	uint screenHeight;
	sceneDepthTexture.GetDimensions(screenWidth, screenHeight);
	if (threadID.x >= screenWidth || threadID.y >= screenHeight)
		return;
	float2 screenSize = float2(screenWidth, screenHeight);
	uint2 sourcePixel = threadID.xy;

	// Scene depth and skip sky:
	float sourceNdcDepth = Scene_GetNdcDepth(sourcePixel);
	if (sourceNdcDepth >= 1.0f)
		return;

	// Evaluate and write ambient occlusion:
	float ambientOcclusion = Ssao_GetAmbientOcclusion(Ssao_GetOcclusion(sourcePixel, sourceNdcDepth, screenSize));
	float4 surfaceProperties = gbufferSurfacePropertiesStorageTexture[sourcePixel];
	surfaceProperties[DEFERRED_SURFACE_PROPERTIES_AMBIENT_OCCLUSION_CHANNEL] = ambientOcclusion;
	gbufferSurfacePropertiesStorageTexture[sourcePixel] = surfaceProperties;
}