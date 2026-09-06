#pragma once



namespace vulkanRendererBackend
{
	// Forward declarations:
	struct FrameContext;



	class PresentStage
	{
	public: // Methods:
		void Record(const FrameContext& frameContext);

	private: // Methods:
		void RecordFullScreenPresent(const FrameContext& frameContext);
		void RecordImGuiPresent(const FrameContext& frameContext);
	};
}