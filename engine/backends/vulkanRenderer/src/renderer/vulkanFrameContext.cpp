#include "vulkanFrameContext.h"



namespace vulkanRendererBackend
{
	FrameContext::FrameContext(uint32_t frameIndex, uint32_t imageIndex, uint32_t transparentSceneColorIndex, float time, float deltaTime, uint32_t shadowMapResolution, uint32_t shadowMapCount, float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor, FrameResources& resources, FrameRenderData& frameRenderData, RenderTargetResources& renderTargets, emberBackendInterface::IGui* pGui, std::span<const ComputeCall> preRenderComputeCalls, std::span<const ComputeCall> midRenderComputeCalls, std::span<const ComputeCall> screenSpaceComputeCalls, std::span<const ComputeCall> postRenderComputeCalls)
		: frameIndex(frameIndex)
		, imageIndex(imageIndex)
		, transparentSceneColorIndex(transparentSceneColorIndex)
		, time(time)
		, deltaTime(deltaTime)
		, shadowMapResolution(shadowMapResolution)
		, shadowMapCount(shadowMapCount)
		, depthBiasConstantFactor(depthBiasConstantFactor)
		, depthBiasClamp(depthBiasClamp)
		, depthBiasSlopeFactor(depthBiasSlopeFactor)
		, resources(resources)
		, frameRenderData(frameRenderData)
		, renderTargets(renderTargets)
		, pGui(pGui)
		, preRenderComputeCalls(preRenderComputeCalls)
		, midRenderComputeCalls(midRenderComputeCalls)
		, screenSpaceComputeCalls(screenSpaceComputeCalls)
		, postRenderComputeCalls(postRenderComputeCalls)
	{

	}
}