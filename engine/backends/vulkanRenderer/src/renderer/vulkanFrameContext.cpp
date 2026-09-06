#include "vulkanFrameContext.h"



namespace vulkanRendererBackend
{
	FrameContext::FrameContext(uint32_t frameIndex, FrameResources& resources, FrameRenderData& frameRenderData)
		: frameIndex(frameIndex)
		, resources(resources)
		, frameRenderData(frameRenderData)
	{

	}
}