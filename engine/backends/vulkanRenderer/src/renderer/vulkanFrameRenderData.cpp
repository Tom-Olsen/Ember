#include "vulkanFrameRenderData.h"
#include "commonCamera.h"
#include "vulkanMaterial.h"
#include "vulkanMesh.h"
#include "vulkanPoolManager.h"
#include <algorithm>
#include <functional>



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
				const float distanceA = Float3::DistanceSq(drawCallA->worldPosition, camera.position);
				const float distanceB = Float3::DistanceSq(drawCallB->worldPosition, camera.position);
				if (distanceA != distanceB)
					return distanceA > distanceB;
			}

			auto layoutA = drawCallA->pMesh->GetVertexMemoryLayout();
			auto layoutB = drawCallB->pMesh->GetVertexMemoryLayout();
			if (layoutA != layoutB)
				return layoutA < layoutB;
			return drawCallA < drawCallB;
		});

		sortedDeferredDrawCallPointers.clear();
		sortedDeferredDrawCallPointers.reserve(deferredDrawCalls.size());
		for (DeferredDrawCall& drawCall : deferredDrawCalls)
			sortedDeferredDrawCallPointers.push_back(&drawCall);
		std::sort(sortedDeferredDrawCallPointers.begin(), sortedDeferredDrawCallPointers.end(), [](DeferredDrawCall* drawCallA, DeferredDrawCall* drawCallB)
		{
			int renderQueueA = static_cast<int>(drawCallA->pMaterial->GetRenderQueue());
			int renderQueueB = static_cast<int>(drawCallB->pMaterial->GetRenderQueue());
			if (renderQueueA != renderQueueB)
				return renderQueueA < renderQueueB;

			auto layoutA = drawCallA->pMesh->GetVertexMemoryLayout();
			auto layoutB = drawCallB->pMesh->GetVertexMemoryLayout();
			if (layoutA != layoutB)
				return layoutA < layoutB;

			if (drawCallA->pMaterial != drawCallB->pMaterial)
				return std::less<Material*>()(drawCallA->pMaterial, drawCallB->pMaterial);
			return std::less<DeferredDrawCall*>()(drawCallA, drawCallB);
		});

		sortedForwardOpaqueDrawCallPointers.clear();
		sortedForwardOpaqueDrawCallPointers.reserve(forwardDrawCalls.size());
		sortedForwardTransparentDrawCallPointers.clear();
		sortedForwardTransparentDrawCallPointers.reserve(forwardDrawCalls.size());
		for (ForwardDrawCall& drawCall : forwardDrawCalls)
		{
			if (drawCall.pMaterial->GetForwardRenderMode() == emberCommon::ForwardRenderMode::transparent)
				sortedForwardTransparentDrawCallPointers.push_back(&drawCall);
			else
				sortedForwardOpaqueDrawCallPointers.push_back(&drawCall);
		}
		auto compareForwardDrawCalls = [&camera](ForwardDrawCall* drawCallA, ForwardDrawCall* drawCallB)
		{
			int renderQueueA = static_cast<int>(drawCallA->pMaterial->GetRenderQueue());
			int renderQueueB = static_cast<int>(drawCallB->pMaterial->GetRenderQueue());
			if (renderQueueA != renderQueueB)
				return renderQueueA < renderQueueB;

			const bool transparentA = drawCallA->pMaterial->IsTransparent();
			const bool transparentB = drawCallB->pMaterial->IsTransparent();
			if (transparentA && transparentB)
			{
				const float distanceA = Float3::DistanceSq(drawCallA->worldPosition, camera.position);
				const float distanceB = Float3::DistanceSq(drawCallB->worldPosition, camera.position);
				if (distanceA != distanceB)
					return distanceA > distanceB;
			}

			auto layoutA = drawCallA->pMesh->GetVertexMemoryLayout();
			auto layoutB = drawCallB->pMesh->GetVertexMemoryLayout();
			if (layoutA != layoutB)
				return layoutA < layoutB;

			if (drawCallA->pMaterial != drawCallB->pMaterial)
				return std::less<Material*>()(drawCallA->pMaterial, drawCallB->pMaterial);
			return std::less<ForwardDrawCall*>()(drawCallA, drawCallB);
		};
		std::sort(sortedForwardOpaqueDrawCallPointers.begin(), sortedForwardOpaqueDrawCallPointers.end(), compareForwardDrawCalls);
		std::sort(sortedForwardTransparentDrawCallPointers.begin(), sortedForwardTransparentDrawCallPointers.end(), compareForwardDrawCalls);
	}
	void FrameRenderData::Reset()
	{
		for (GizmoDrawCall& drawCall : gizmoDrawCalls)
			PoolManager::ReturnCallDescriptorSetBinding(drawCall.descriptorSetBindingHandle);
		for (OutlineDrawCall& drawCall : outlineDrawCalls)
			PoolManager::ReturnCallDescriptorSetBinding(drawCall.descriptorSetBindingHandle);
		for (ShadowDrawCall& drawCall : shadowDrawCalls)
			PoolManager::ReturnCallDescriptorSetBinding(drawCall.descriptorSetBindingHandle);
		for (DeferredDrawCall& drawCall : deferredDrawCalls)
			PoolManager::ReturnCallDescriptorSetBinding(drawCall.descriptorSetBindingHandle);
		for (ForwardDrawCall& drawCall : forwardDrawCalls)
			PoolManager::ReturnCallDescriptorSetBinding(drawCall.descriptorSetBindingHandle);

		gizmoDrawCalls.clear();
		sortedGizmoDrawCallPointers.clear();
		outlineDrawCalls.clear();
		shadowDrawCalls.clear();
		deferredDrawCalls.clear();
		sortedDeferredDrawCallPointers.clear();
		forwardDrawCalls.clear();
		sortedForwardOpaqueDrawCallPointers.clear();
		sortedForwardTransparentDrawCallPointers.clear();
	}
}