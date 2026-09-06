#include "vulkanFrameRenderData.h"
#include "commonCamera.h"
#include "vulkanMaterial.h"
#include "vulkanMesh.h"
#include "vulkanPoolManager.h"
#include <algorithm>



namespace vulkanRendererBackend
{
	// Public methods:
	void FrameRenderData::SortDrawCalls(const emberCommon::Camera& camera)
	{
		sortedGizmoDrawCallPointers.clear();
		sortedGizmoDrawCallPointers.reserve(gizmoDrawCalls.size());
		for (GizmoDrawCall& drawCall : gizmoDrawCalls)
			sortedGizmoDrawCallPointers.push_back(&drawCall);

		// Ember::ToDo: frustum culling and sorting by dist to camera is missing.
		// Sort gizmo calls by renderQueue first, then handle transparent draw order, then group by vertex layout:
		std::sort(sortedGizmoDrawCallPointers.begin(), sortedGizmoDrawCallPointers.end(), [&camera](GizmoDrawCall* drawCallA, GizmoDrawCall* drawCallB)
		{
			int renderQueueA = static_cast<int>(drawCallA->pMaterial->GetRenderQueue());
			int renderQueueB = static_cast<int>(drawCallB->pMaterial->GetRenderQueue());
			if (renderQueueA != renderQueueB)
				return renderQueueA < renderQueueB;

			const bool transparentA = drawCallA->pMaterial->IsTransparent();
			const bool transparentB = drawCallB->pMaterial->IsTransparent();
			if (transparentA && transparentB)
			{
				const Float3 drawPositionA = Float3(drawCallA->localToWorldMatrix * Float4(0.0f, 0.0f, 0.0f, 1.0f));
				const Float3 drawPositionB = Float3(drawCallB->localToWorldMatrix * Float4(0.0f, 0.0f, 0.0f, 1.0f));
				const float distanceA = Float3::DistanceSq(drawPositionA, camera.position);
				const float distanceB = Float3::DistanceSq(drawPositionB, camera.position);
				if (distanceA != distanceB)
					return distanceA > distanceB;
			}

			auto layoutA = drawCallA->pMesh->GetVertexMemoryLayout();
			auto layoutB = drawCallB->pMesh->GetVertexMemoryLayout();
			if (layoutA != layoutB)
				return layoutA < layoutB;
			return drawCallA < drawCallB;
		});
	}
	void FrameRenderData::Reset()
	{
		for (GizmoDrawCall& drawCall : gizmoDrawCalls)
			PoolManager::ReturnCallDescriptorSetBinding(drawCall.descriptorSetBindingHandle);

		gizmoDrawCalls.clear();
		sortedGizmoDrawCallPointers.clear();
	}
}