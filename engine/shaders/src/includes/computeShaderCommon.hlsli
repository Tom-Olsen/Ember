#define EMBER_ENABLE_FRAME_SET_RENDER_TARGETS // exposes sceneColorTexture0, sceneColorTexture1, and sceneDepthTexture to compute shaders.
#include "descriptorSetMacros.h"
#include "computePushConstant.hlsli"
#include "math.hlsli"
#include "globalSet.hlsli"
#include "sceneSet.hlsli"
#include "frameSet.hlsli"
#include "shaderFeatureMacros.h"



// Scene depth:
float Scene_GetNdcDepth(uint2 pixel)
{// pixel in [0...screenSize.x-1]x[0...screenSize.y-1], ndc depth in [0,1].
	return sceneDepthTexture[pixel];
}
float Scene_GetNdcDepth(float2 pixelCenter)
{// pixelCenter in [0.5...screenSize.x-0.5]x[0.5...screenSize.y-0.5], ndc depth in [0,1].
	return Scene_GetNdcDepth(uint2(pixelCenter));
}
float Scene_GetNdcDepth(float2 uv, float2 screenSize)
{// uv in [0,1), ndcDepth in [0,1], ndc depth in [0,1].
	return Scene_GetNdcDepth(uv * screenSize);
}



// Distance from worldRay to scene:
float Scene_GetDistance(uint2 pixel, math_Ray worldRay, float2 screenSize, bool directionIsNormalized = false)
{// pixel in [0...screenSize.x-1]x[0...screenSize.y-1], ndc depth in [0,1].
	float sceneDepth = Scene_GetNdcDepth(pixel);
	if (sceneDepth >= 1.0f)
		return 1.0e30f;

	float3 scenePosition = Camera_GetWorldPosition(pixel, sceneDepth, screenSize);
	float result = dot(scenePosition - worldRay.origin, worldRay.direction);
	return directionIsNormalized ? result : result / length(worldRay.direction);
}
float Scene_GetDistance(float2 pixelCenter, math_Ray worldRay, float2 screenSize, bool directionIsNormalized = false)
{// pixelCenter in [0.5...screenSize.x-0.5]x[0.5...screenSize.y-0.5], ndc depth in [0,1].
	float sceneDepth = Scene_GetNdcDepth(pixelCenter);
	if (sceneDepth >= 1.0f)
		return 1.0e30f;

	float3 scenePosition = Camera_GetWorldPosition(pixelCenter, sceneDepth, screenSize);
	float result = dot(scenePosition - worldRay.origin, worldRay.direction);
	return directionIsNormalized ? result : result / length(worldRay.direction);
}
// Scene_GetDistance uv overload not possibel due to input argument clash wih pixelCenter version. 



// Scene Color:
float4 Scene_GetColor(uint2 pixel)
{
	if (pc.sceneColorIndex == 0)
		return sceneColorTexture0[pixel];
	return sceneColorTexture1[pixel];
}
#if EMBER_SCENE_COLOR_ACCESS == EMBER_SCENE_COLOR_ACCESS_READ || EMBER_SCENE_COLOR_ACCESS == EMBER_SCENE_COLOR_ACCESS_OUT_OF_PLACE
float4 Scene_SampleColor(float2 uv)
{
	if (pc.sceneColorIndex == 0)
		return sceneColorSampleTexture0.SampleLevel(colorSamplerClampEdge, uv, 0.0f);
	return sceneColorSampleTexture1.SampleLevel(colorSamplerClampEdge, uv, 0.0f);
}
#endif
void Scene_SetColor(uint2 pixel, float4 color)
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