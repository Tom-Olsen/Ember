#pragma once



namespace vulkanRendererBackend
{
	// Forward declarations:
	struct FrameContext;



	class GizmoStage
	{
	public: // Methods:
		void Record(const FrameContext& frameContext);
	};
}