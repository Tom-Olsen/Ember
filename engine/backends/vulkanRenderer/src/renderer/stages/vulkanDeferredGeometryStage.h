#pragma once



namespace vulkanRendererBackend
{
	// Forward declarations:
	struct FrameContext;



	class DeferredGeometryStage
	{
	public: // Methods:
		void Record(const FrameContext& frameContext);
	};
}