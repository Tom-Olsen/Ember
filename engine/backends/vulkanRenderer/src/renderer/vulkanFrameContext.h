#pragma once
// #include "commonCamera.h"
#include <cstdint>



namespace emberBackendInterface
{
	class IGui;
}



namespace vulkanRendererBackend
{
	// Forward declarations:
	class SceneColorTexture2dPair;
	struct FrameResources;
	struct FrameRenderData;



	/// <summary>
	/// Non-owning view that passes data to render stages.
	/// </summary>
	struct FrameContext
	{
	public: // Members:
		uint32_t frameIndex;
		uint32_t imageIndex;
		float time;
		float deltaTime;
		uint32_t shadowMapResolution;
		uint32_t shadowMapCount;
		float depthBiasConstantFactor;
		float depthBiasClamp;
		float depthBiasSlopeFactor;
		FrameResources& resources;
		FrameRenderData& frameRenderData;
		SceneColorTexture2dPair& sceneColorTexturePair;
		emberBackendInterface::IGui* pGui;

	public: // Methods:
		FrameContext(uint32_t frameIndex, uint32_t imageIndex, float time, float deltaTime, uint32_t shadowMapResolution, uint32_t shadowLightCount, float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor, FrameResources& resources, FrameRenderData& frameRenderData, SceneColorTexture2dPair& sceneColorTexturePair, emberBackendInterface::IGui* pGui);
	};
}