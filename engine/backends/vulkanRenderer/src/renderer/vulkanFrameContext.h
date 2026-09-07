#pragma once
// #include "commonCamera.h"
#include <cstdint>
#include <span>



namespace emberBackendInterface
{
	class IGui;
}



namespace vulkanRendererBackend
{
	// Forward declarations:
	class SceneColorTexture2dPair;
	struct ComputeCall;
	struct FrameResources;
	struct FrameRenderData;



	/// <summary>
	/// Non-owning view that passes data to render stages.
	/// </summary>
	struct FrameContext
	{
	public: // Members:
		// Frame indexing:
		uint32_t frameIndex;
		uint32_t imageIndex;
		// Frame timings:
		float time;
		float deltaTime;
		// Frame shadow data:
		uint32_t shadowMapResolution;
		uint32_t shadowMapCount;
		float depthBiasConstantFactor;
		float depthBiasClamp;
		float depthBiasSlopeFactor;
		// Frame resources:
		FrameResources& resources;
		FrameRenderData& frameRenderData;
		SceneColorTexture2dPair& sceneColorTexturePair;
		emberBackendInterface::IGui* pGui;
		// Frame compute calls:
		std::span<const ComputeCall> preRenderComputeCalls;
		std::span<const ComputeCall> midRenderComputeCalls;
		std::span<const ComputeCall> screenSpaceComputeCalls;
		std::span<const ComputeCall> postRenderComputeCalls;

	public: // Methods:
		FrameContext(uint32_t frameIndex, uint32_t imageIndex, float time, float deltaTime, uint32_t shadowMapResolution, uint32_t shadowLightCount, float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor, FrameResources& resources, FrameRenderData& frameRenderData, SceneColorTexture2dPair& sceneColorTexturePair, emberBackendInterface::IGui* pGui, std::span<const ComputeCall> preRenderComputeCalls, std::span<const ComputeCall> midRenderComputeCalls, std::span<const ComputeCall> screenSpaceComputeCalls, std::span<const ComputeCall> postRenderComputeCalls);
	};
}