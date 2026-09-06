#include "vulkanFrameContext.h"



namespace vulkanRendererBackend
{
	FrameContext::FrameContext(uint32_t frameIndex, uint32_t imageIndex, float time, float deltaTime, uint32_t shadowMapResolution, uint32_t shadowMapCount, float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor, FrameResources& resources, FrameRenderData& frameRenderData, SceneColorTexture2dPair& sceneColorTexturePair, emberBackendInterface::IGui* pGui)
		: frameIndex(frameIndex)
		, imageIndex(imageIndex)
		, time(time)
		, deltaTime(deltaTime)
		, shadowMapResolution(shadowMapResolution)
		, shadowMapCount(shadowMapCount)
		, depthBiasConstantFactor(depthBiasConstantFactor)
		, depthBiasClamp(depthBiasClamp)
		, depthBiasSlopeFactor(depthBiasSlopeFactor)
		, resources(resources)
		, frameRenderData(frameRenderData)
		, sceneColorTexturePair(sceneColorTexturePair)
		, pGui(pGui)
	{

	}
}