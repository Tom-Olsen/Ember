#pragma once
#include "vulkanDeferredDrawCall.h"
#include "vulkanForwardDrawCall.h"
#include "vulkanGizmoDrawCall.h"
#include "vulkanOutlineDrawCall.h"
#include "vulkanShadowDrawCall.h"
#include <vector>



// Forward declarations:
namespace emberCommon
{
	struct Camera;
}



namespace vulkanRendererBackend
{
	// Forward declarations:
	class Mesh;



	/// <summary>
	/// CPU-side workload collected for a frameInFlight slot.
	/// </summary>
	struct FrameRenderData
	{
	public: // Members:
		// Resource update stage:
		std::vector<Mesh*> meshUpdates;

		// Gizmo stage:
		std::vector<GizmoDrawCall> gizmoDrawCalls;
		std::vector<GizmoDrawCall*> sortedGizmoDrawCallPointers;

		// Outline stage:
		std::vector<OutlineDrawCall> outlineDrawCalls;

		// Shadow stage:
		std::vector<ShadowDrawCall> shadowDrawCalls;

		// Deferred stage:
		std::vector<DeferredDrawCall> deferredDrawCalls;
		std::vector<DeferredDrawCall*> sortedDeferredDrawCallPointers;

		// Forward stage:
		std::vector<ForwardDrawCall> forwardDrawCalls;
		std::vector<ForwardDrawCall*> sortedForwardOpaqueDrawCallPointers;
		std::vector<ForwardDrawCall*> sortedForwardTransparentDrawCallPointers;

	public: // Methods:
		void SortDrawCalls(const emberCommon::Camera& camera);
		void Reset();
	};
}