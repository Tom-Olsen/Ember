#pragma once



namespace vulkanRendererBackend
{
	// Forward declarations:
	struct FrameContext;



	class DeferredLightingStage
	{
	public: // Methods:
		DeferredLightingStage() = delete;
		static void Record(const FrameContext& frameContext);
	};
}