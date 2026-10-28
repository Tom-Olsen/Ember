#pragma once



namespace vulkanRendererBackend
{
	// Forward declarations:
	struct FrameContext;



	class ShadowStage
	{
	public: // Methods:
		ShadowStage() = delete;
		static void Record(const FrameContext& frameContext);
	};
}