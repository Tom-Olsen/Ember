#define EMBER_ENABLE_FRAME_SET_RENDER_TARGETS // exposes sceneColorTexture0, sceneColorTexture1, and sceneDepthTexture to compute shaders.
#include "descriptorSetMacros.h"
#include "computePushConstant.hlsli"
#include "math.hlsli"
#include "globalSet.hlsli"
#include "sceneSet.hlsli"
#include "frameSet.hlsli"
#include "shaderFeatureMacros.h"



float GetSceneNdcDepth(uint2 pixel)
{// ndc depth in [0,1].
	return sceneDepthTexture[pixel];
}
float4 GetSceneColor(uint2 pixel)
{
	if (pc.sceneColorIndex == 0)
		return sceneColorTexture0[pixel];
	return sceneColorTexture1[pixel];
}
#if EMBER_SCENE_COLOR_ACCESS == EMBER_SCENE_COLOR_ACCESS_READ || EMBER_SCENE_COLOR_ACCESS == EMBER_SCENE_COLOR_ACCESS_OUT_OF_PLACE
float4 SampleSceneColor(float2 uv)
{
	if (pc.sceneColorIndex == 0)
		return sceneColorSampleTexture0.SampleLevel(colorSamplerClampEdge, uv, 0.0f);
	return sceneColorSampleTexture1.SampleLevel(colorSamplerClampEdge, uv, 0.0f);
}
#endif
void SetSceneColor(uint2 pixel, float4 color)
{
	#if EMBER_SCENE_COLOR_ACCESS == EMBER_SCENE_COLOR_ACCESS_OUT_OF_PLACE
	uint sceneColorIndex = 1 - pc.sceneColorIndex;
	#else
	uint sceneColorIndex = pc.sceneColorIndex;
	#endif
	if (sceneColorIndex == 0)
		sceneColorTexture0[pixel] = color;
	else
		sceneColorTexture1[pixel] = color;
}