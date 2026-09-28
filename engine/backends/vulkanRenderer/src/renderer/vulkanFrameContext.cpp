#include "vulkanFrameContext.h"



namespace vulkanRendererBackend
{
	FrameContext::FrameContext(const FrameExecutionData& frameExecutionData, uint32_t shadowMapResolution, uint32_t shadowMapCount, float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor, FrameResources& resources, FrameRenderData& frameRenderData, RenderTargetResources& renderTargets, emberBackendInterface::IVulkanGui* pVulkanGui, std::span<const ComputeCall> preRenderComputeCalls, std::span<const ComputeCall> midRenderComputeCalls, std::span<const ComputeCall> screenSpaceComputeCalls, std::span<const ComputeCall> postRenderComputeCalls)
		: frameExecutionData(frameExecutionData)
		, shadowMapResolution(shadowMapResolution)
		, shadowMapCount(shadowMapCount)
		, depthBiasConstantFactor(depthBiasConstantFactor)
		, depthBiasClamp(depthBiasClamp)
		, depthBiasSlopeFactor(depthBiasSlopeFactor)
		, resources(resources)
		, frameRenderData(frameRenderData)
		, renderTargets(renderTargets)
		, pVulkanGui(pVulkanGui)
		, preRenderComputeCalls(preRenderComputeCalls)
		, midRenderComputeCalls(midRenderComputeCalls)
		, screenSpaceComputeCalls(screenSpaceComputeCalls)
		, postRenderComputeCalls(postRenderComputeCalls)
	{

	}
}