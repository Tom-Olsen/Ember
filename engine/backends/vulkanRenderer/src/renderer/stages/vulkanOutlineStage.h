#pragma once



namespace vulkanRendererBackend
{
	// Forward declarations:
	struct FrameContext;



	class OutlineStage
	{
	public: // Methods:
		OutlineStage() = delete;
		static void Record(const FrameContext& frameContext);
	};
}