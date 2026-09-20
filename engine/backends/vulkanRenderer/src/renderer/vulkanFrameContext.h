#pragma once
// #include "commonCamera.h"
#include "vulkanFrameExecutionData.h"
#include <cstdint>
#include <span>



namespace emberBackendInterface
{
	class IGui;
}



namespace vulkanRendererBackend
{
	// Forward declarations:
	class RenderTargetResources;
	struct ComputeCall;
	struct FrameResources;
	struct FrameRenderData;



	/// <summary>
	/// Non-owning view that passes data to render stages.
	/// </summary>
	struct FrameContext
	{
	public: // Members:
		// Frame execution data:
		const FrameExecutionData& frameExecutionData;
		// Frame shadow data:
		uint32_t shadowMapResolution;
		uint32_t shadowMapCount;
		float depthBiasConstantFactor;
		float depthBiasClamp;
		float depthBiasSlopeFactor;
		// Frame resources:
		FrameResources& resources;
		FrameRenderData& frameRenderData;
		RenderTargetResources& renderTargets;
		emberBackendInterface::IGui* pGui;
		// Frame compute calls:
		std::span<const ComputeCall> preRenderComputeCalls;
		std::span<const ComputeCall> midRenderComputeCalls;
		std::span<const ComputeCall> screenSpaceComputeCalls;
		std::span<const ComputeCall> postRenderComputeCalls;

	public: // Methods:
		FrameContext(const FrameExecutionData& frameExecutionData, uint32_t shadowMapResolution, uint32_t shadowLightCount, float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor, FrameResources& resources, FrameRenderData& frameRenderData, RenderTargetResources& renderTargets, emberBackendInterface::IGui* pGui, std::span<const ComputeCall> preRenderComputeCalls, std::span<const ComputeCall> midRenderComputeCalls, std::span<const ComputeCall> screenSpaceComputeCalls, std::span<const ComputeCall> postRenderComputeCalls);
	};
}