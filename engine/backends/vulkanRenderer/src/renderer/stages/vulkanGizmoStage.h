#pragma once



namespace vulkanRendererBackend
{
	// Forward declarations:
	struct FrameContext;



	class GizmoStage
	{
	public: // Methods:
		GizmoStage() = delete;
		static void Record(const FrameContext& frameContext);
	};
}