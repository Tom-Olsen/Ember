#pragma once
#include "commonPipelineState.h"
#include <cstdint>



namespace vulkanRendererBackend
{
	struct MaterialRenderState
	{
	public: // Members:
		emberCommon::RenderMode renderMode;
		int32_t renderQueue;

	public: // Methods:
		// Constructors:
		MaterialRenderState();
		MaterialRenderState(emberCommon::RenderMode renderMode, int32_t renderQueue);
	};
}