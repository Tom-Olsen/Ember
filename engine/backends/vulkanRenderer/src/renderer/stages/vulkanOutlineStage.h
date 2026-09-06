#pragma once



namespace vulkanRendererBackend
{
	// Forward declarations:
	struct FrameContext;



	class OutlineStage
	{
	public: // Methods:
		void Record(const FrameContext& frameContext);
	};
}