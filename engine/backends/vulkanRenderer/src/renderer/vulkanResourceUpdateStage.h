#pragma once



namespace vulkanRendererBackend
{
	// Forward declarations:
	struct FrameContext;
}



namespace vulkanRendererBackend
{
	class ResourceUpdateStage
	{
	public: // Methods:
		void Record(const FrameContext& frameContext);
	};
}