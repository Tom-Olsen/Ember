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
		ForwardStage() = delete;
		static void Record(const FrameContext& frameContext);
	};
}