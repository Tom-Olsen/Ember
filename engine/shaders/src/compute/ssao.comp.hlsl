#include "computeShaderCommon.hlsli"
#include "bilateralUpsampling.hlsli"
#include "ssao.hlsli"



// Bindings:
[[vk::image_format("r16f")]] RWTexture2D<float> ambientOcclusionMap : register(u200, CALL_SET);



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
	uint2 sourcePixel = Bilateral_LowResolutionPixelToScreenPixel(threadID.xy, uint2(screenWidth, screenHeight), float2(aoWidth, aoHeight));

	// Compute ambient occlusion:
	float occlusion = 0.0f;
	float sourceNdcDepth = Scene_GetNdcDepth(sourcePixel);
	if (sourceNdcDepth < 1.0f)
		occlusion = Ssao_GetOcclusion(sourcePixel, sourceNdcDepth, float2(screenWidth, screenHeight));
	ambientOcclusionMap[threadID.xy] = occlusion;
}