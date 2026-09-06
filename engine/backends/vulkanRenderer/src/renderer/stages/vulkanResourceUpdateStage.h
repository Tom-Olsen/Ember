#pragma once



namespace vulkanRendererBackend
{
	// Forward declarations:
	struct FrameContext;



	class ResourceUpdateStage
	{
	public: // Methods:
		void Record(const FrameContext& frameContext);
	};
}