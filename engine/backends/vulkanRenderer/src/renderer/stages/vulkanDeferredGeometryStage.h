#pragma once



namespace vulkanRendererBackend
{
	// Forward declarations:
	struct FrameContext;



	class DeferredGeometryStage
	{
	public: // Methods:
		DeferredGeometryStage() = delete;
		static void Record(const FrameContext& frameContext);
	};
}