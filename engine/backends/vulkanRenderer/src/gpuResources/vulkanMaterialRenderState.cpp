#include "vulkanMaterialRenderState.h"
#include "commonRenderQueue.h"



namespace vulkanRendererBackend
{
	// Public methods:
	// Constructors:
	MaterialRenderState::MaterialRenderState()
		: renderMode(emberCommon::RenderMode::opaque)
		, renderQueue(emberCommon::RenderQueue::opaque)
	{

	}
	MaterialRenderState::MaterialRenderState(emberCommon::RenderMode renderMode, int32_t renderQueue)
		: renderMode(renderMode)
		, renderQueue(renderQueue)
	{

	}
}