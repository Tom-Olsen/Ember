#pragma once



namespace vulkanRendererBackend
{
	// Forward declarations:
	struct FrameContext;



	class DeferredLightingStage
	{
	public: // Methods:
		void Record(const FrameContext& frameContext);
	};
}