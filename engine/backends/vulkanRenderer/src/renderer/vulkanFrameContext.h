#pragma once
// #include "commonCamera.h"
#include <cstdint>



namespace vulkanRendererBackend
{
	// Forward declarations:
	struct FrameResources;
	struct FrameRenderData;



	/// <summary>
	/// Non-owning view that passes data to render stages.
	/// </summary>
	struct FrameContext
	{
	public: // Members:
		uint32_t frameIndex;
		FrameResources& resources;
		FrameRenderData& frameRenderData;

		// uint32_t imageIndex;
		// float time;
		// float deltaTime;
		// const emberCommon::Camera& camera;

	public: // Methods:
		FrameContext(uint32_t frameIndex, FrameResources& resources, FrameRenderData& frameRenderData);
	};
}