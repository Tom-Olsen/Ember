#pragma once



namespace vulkanRendererBackend
{
	// Forward declarations:
	struct FrameContext;



	class PresentStage
	{
	public: // Methods:
		PresentStage() = delete;
		static void Record(const FrameContext& frameContext, bool hasSwapchainImage);

	private: // Methods:
		static void RecordFullScreenPresent(const FrameContext& frameContext);
		static void RecordImGuiPresent(const FrameContext& frameContext);
		static void RecordSkipPresent(const FrameContext& frameContext);
	};
}