#pragma once
#include "vulkanRenderStage.h"



namespace vulkanRendererBackend
{
	// Forward declarations:
	struct FrameContext;



	template<RenderStage stage>
	class ForwardStage
	{
	public: // Methods:
		void Record(const FrameContext& frameContext);
	};
}