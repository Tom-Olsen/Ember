#pragma once
#include <cstdint>



namespace vulkanRendererBackend
{
	struct FrameExecutionData
	{
	public: // Members:
		uint32_t frameIndex = 0;
		uint32_t imageIndex = 0;
		uint32_t forwardSceneColorIndex = 0;
		float time = 0.0f;
		float deltaTime = 0.0f;
	};
}