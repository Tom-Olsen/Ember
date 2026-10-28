#pragma once



namespace vulkanRendererBackend
{
	// Forward declarations:
	struct FrameContext;



	class ResourceUpdateStage
	{
	public: // Methods:
		ResourceUpdateStage() = delete;
		static void Record(const FrameContext& frameContext);
	};
}