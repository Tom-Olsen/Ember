#pragma once



namespace vulkanRendererBackend
{
	// Forward declarations:
	struct FrameContext;



	class ShadowStage
	{
	public: // Methods:
		void Record(const FrameContext& frameContext);
	};
}