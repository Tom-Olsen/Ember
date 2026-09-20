#include "computePushConstant.hlsli"
#define EMBER_ENABLE_FRAME_SET_RENDER_TARGETS // exposes sceneColorTexture0, sceneColorTexture1, and sceneDepthTexture to compute shaders.
#include "frameSet.hlsli"
#include "math.hlsli"
#include "shaderFeatureMacros.h"



float GetSceneDepth(uint2 pixel)
{
	return sceneDepthTexture[pixel];
}
float4 GetSceneColor(uint2 pixel)
{
	if (pc.sceneColorIndex == 0)
		return sceneColorTexture0[pixel];
	return sceneColorTexture1[pixel];
}
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