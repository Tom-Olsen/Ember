#include "vulkanFrameContext.h"



namespace vulkanRendererBackend
{
	FrameContext::FrameContext(uint32_t frameIndex, float time, float deltaTime, FrameResources& resources, FrameRenderData& frameRenderData)
		: frameIndex(frameIndex)
		, time(time)
		, deltaTime(deltaTime)
		, resources(resources)
		, frameRenderData(frameRenderData)
	{

	}
}